  #include <Servo.h>
  #include <Wire.h>
  #include "RTClib.h"
  #include <LiquidCrystal.h>

  // LiquidCrystal lcd(7, 8, 9, 10, 11, 12);
  LiquidCrystal lcd(52, 53, 50, 51, 48, 49);

  Servo myservo;
  RTC_Millis rtc;
  // DS1302 (ce_pin, sck_pin, io_pin);
  // DS1302 rtc(38, 40, 42);


  /*  ========== TU ZMIEŃ USTAWIENIA SYSTEMU ================ */
  /*  ======================================================= */
  int debug = 0;                    // "1" - odpala serial na 9600 i wyswietla info na monitor (wykonuje podlewanie co 10min.)
  int GodzinaStart = 6;             // godzina uruchomienia
  int MinutaStart = 0;              // minuta uruchomienia
  int CzasPodlewania = 8;           // minuty
  int IloscPobran = 10;             // ile razy pobrac wilgotnosc do sredniej
  int IloscPobranFast = 3;          // ile razy pobrac wilgotnosc do sredniej (wersja szybka-aktualizacja co godz. na wyświetlaczu)
  int StartZraszanieProcent = 15;   // od ilu procent rozpoczynam zraszanie [0:b.sucho,99:b.mokro]

  byte tabPunkty[] = {A0, A2, A3, A4, A5, A6, A7,   A1};       // piny dla kazdego zraszacza / A1 - na koncu owoce
  int Wilgotnosc = A15;                                        // pin czujnika wilgotnosci
  /*  ======================================================= */


  int Run = 0;
  int AktGodzina = -1;
  unsigned long CzasStop = 0;
  int pos = 0;

  int StartZrasacz = 0;
  int AktZraszacz = StartZrasacz;

  unsigned long MX_CzasPodlewania = CzasPodlewania * 60;
  int Punktow = sizeof(tabPunkty);                // szt.

  int OdczytWilgotnosc = 1;
  int IloscPobranWilgotnosc = 0;
  int AktOdczytWilgotnosc = 0;
  int Prev_AktOdczytWilgotnosc = AktOdczytWilgotnosc;
  int SumaWilgotnosc = 0;
  int WilgoRosnaca = 0;  // porownuje z poprzednim odczytem: -1 malejaco / 0-stoi / 1-rosnaco

  int SprSzybko=0;
  int OstatnieWykonienie=0;  // Ikonka ostatnie wykonanie

  uint8_t clock[8] = {0x0,0xe,0x15,0x17,0x11,0xe,0x0};
  uint8_t stop[8] =  {0x0,0x1b,0xe,0x4,0xe,0x1b,0x0};
  uint8_t start[8] = {0x0,0x1,0x3,0x16,0x1c,0x8,0x0};
  byte kropla[8] = {  0b00000,  0b01000,  0b01100,  0b01110,  0b11111,  0b11101,  0b01110, 0b00000 };

  byte rosnie[8] = {
       0b00000,
       0b00100,
       0b01110,
       0b11111,
       0b00000,
       0b00000,
       0b00000,
       0b00000
  };
  byte maleje[8] = {
       0b00000,
       0b00000,
       0b00000,
       0b11111,
       0b01110,
       0b00100,
       0b00000,
       0b00000
  };
  byte stoi[8] = {
       0b00000,
       0b00000,
       0b00001,
       0b00011,
       0b00001,
       0b00000,
       0b00000,
       0b00000
  };


  // --------
  void setup() {

    if ( debug ) Serial.begin(9600);

    lcd.begin( 16,2 );
    lcd.createChar(0, clock);
    lcd.createChar(1, stop);
    lcd.createChar(2, kropla);
    lcd.createChar(4, start);
    lcd.createChar(5, rosnie);
    lcd.createChar(6, maleje);
    lcd.createChar(7, stoi);



    // WYSWIETLACZ LCD
    lcd.setCursor(0,0);
    lcd.print("DropGRASS Start...");


    // SLEEP Zasilacza
    pinMode(13, OUTPUT);
    digitalWrite(13, HIGH);

    // LAMPKA INFO
    pinMode(12, OUTPUT);

    // WILGOTNOSC GLEBA
    pinMode(Wilgotnosc, INPUT);

    /*
    for (int i = 2; i <= Punktow; i++) {
      pinMode(i, OUTPUT);
      digitalWrite(i, LOW);
      Serial.println( "Ładuje pin: " + String(i) );


      delay(100);
    }
    */
    for (int thisPin = 0; thisPin < Punktow; thisPin++) {
        pinMode(tabPunkty[thisPin], OUTPUT);
        digitalWrite(tabPunkty[thisPin], LOW);
    }


    // SERVO
    myservo.attach(A14);  // attaches the servo on pin 9 to the servo object
    myservo.write(1);
    // Lampka(0);


    // Informacyjna wilgotnosc przy starcie
    SprawdzWilgotnoscFast();
    /*
    while ( IloscPobranWilgotnosc < 3 ) {
         IloscPobranWilgotnosc++;
         SprawdzWilgotnosc();
         delay(300);
    }
    AktOdczytWilgotnosc = SumaWilgotnosc / 3;
    IloscPobranWilgotnosc = 0;
    SumaWilgotnosc = 0;
    */


    // Start ZEGAR
    rtc.begin(DateTime(__DATE__, __TIME__));
    /*
    rtc.begin();
    if (! rtc.isrunning()) {
      lcd.print("RTC is NOT running!");
      // following line sets the RTC to the date & time this sketch was compiled
      rtc.adjust(DateTime(__DATE__, __TIME__));
    }
    */
    /*
    while(! rtc.isrunning()) {
         lcd.setCursor(0,0);
         lcd.print("Czekam na RTC...");
    }
    */


  }


  void loop() {

      lcd.clear();

      DateTime now = rtc.now();

      AktGodzina = now.hour();
      int AktMinuty = now.minute();


      // if ( OdczytWilgotnosc && SprSzybko==0 && AktMinuty == 0   ) {
      if ( OdczytWilgotnosc && SprSzybko==0 && AktMinuty % 10 == 0   ) {
           // szybkie zczytanie co godz.
           SprawdzWilgotnoscFast();
           SprSzybko = 1;
      }
      //else if ( AktMinuty != 0 )
      else if ( AktMinuty % 5 != 0 )
         SprSzybko = 0;


      if ( OdczytWilgotnosc && AktGodzina == GodzinaStart && AktMinuty == MinutaStart ) {
      // if ( OdczytWilgotnosc && ( AktMinuty % 10 ) == 0  ) { // co 10 min.
      // if ( OdczytWilgotnosc && AktMinuty == 0  ) { // co 1 godz

          lcd.setCursor(0,0);
          lcd.print( "ODCZYT %RH");
          IloscPobranWilgotnosc++;

          Prev_AktOdczytWilgotnosc = AktOdczytWilgotnosc;

          if ( IloscPobranWilgotnosc <= IloscPobran ) {
               SprawdzWilgotnosc();
          }
          else {
               AktOdczytWilgotnosc = SumaWilgotnosc / IloscPobran;

               if ( AktOdczytWilgotnosc == Prev_AktOdczytWilgotnosc )
                  WilgoRosnaca = 0;
               else if ( AktOdczytWilgotnosc < Prev_AktOdczytWilgotnosc )
                  WilgoRosnaca = -1;
               else if ( AktOdczytWilgotnosc > Prev_AktOdczytWilgotnosc )
                  WilgoRosnaca = 1;


               OdczytWilgotnosc = 0;
               IloscPobranWilgotnosc=0;
          }

      }


      else if ( !Run && !OdczytWilgotnosc ) { // z ustawien

          // jesli wilg. 0 pewnie cos nie tak z czujnikiem
          if ( AktOdczytWilgotnosc > 0 && AktOdczytWilgotnosc > StartZraszanieProcent ) {
              OstatnieWykonienie = 0;
              OdczytWilgotnosc = 1;
              lcd.setCursor(0,0);
              lcd.print( "STOP ---- CH.W.W" );
              delay(60000);
          }
          else {

              Run = 1;
              OstatnieWykonienie = 1;
              // AktZraszacz = AktZraszacz+1;
              UruchomZasilacz( 1 );
              UruchomZraszacz();
              CzasStop = now.unixtime() + MX_CzasPodlewania;

              if ( debug ) Serial.println( "System start ["+ String(AktZraszacz) +"]" );

              lcd.setCursor(0,0);
              lcd.print("START ["+ String(AktZraszacz) +"]    ");

              // Ustawiam znak KROPLA
              // dalem 1 zamast 0 na start - dzwiek na sucho
              for (pos = 1; pos <= 180; pos += 3) {

                   if ( pos > 180 ) pos = 180;

                   myservo.write(pos);
                   delay(5);

               }

          }

      }


      else if ( Run ) {

          if ( CzasStop > now.unixtime() ) {

               if ( debug ) Serial.println( "Obsługa zraszacza... ["+ String(AktZraszacz) +" z "+ String(Punktow) +"]" );

               lcd.setCursor(0,0);
               lcd.write(byte(4));
               lcd.print(" PUNKT "+ String(AktZraszacz+1) +"z"+ String(Punktow) +"  ");

          }
          else {
               if ( debug ) Serial.println( "STOP! ------------------------------------------- Zmieniam zraszacz" );

               lcd.setCursor(0,0);
               lcd.write(byte(4));
               lcd.print("NAST. PUNKT   ");

               // digitalWrite(AktZraszacz, LOW);
               digitalWrite(tabPunkty[AktZraszacz], LOW);

               //if ( AktZraszacz < Punktow ) {
               if ( AktZraszacz < (Punktow-1) ) {
                  AktZraszacz = AktZraszacz+1;
                  UruchomZraszacz();

                  CzasStop = now.unixtime() + MX_CzasPodlewania;

               }
               else {
                  AktZraszacz = StartZrasacz;
                  Run = 0;
                  OdczytWilgotnosc = 1;
                  SumaWilgotnosc = 0;
                  IloscPobranWilgotnosc = 0;


                  UruchomZasilacz(0);
                  //Lampka(1);

                  // Ustawiam znak STOP
                  // dalem 1 zamast 0 na start - dzwiek na sucho
                  for (pos = 180; pos >= 1; pos -= 3) {

                       if ( pos < 1 ) pos = 1;

                       myservo.write(pos);
                       delay(5);

                   }

               }
          }



      }

      else {
         lcd.setCursor( 0,0 );
         lcd.write(byte(1));
         lcd.print( "STOP        " );
         if ( OstatnieWykonienie )
            lcd.print( "[T]" );
         else
            lcd.print( "[N]" );
         // lcd.print( "lena <3 xD" );
      }



      WyswietlGodzine();
      WyswietlWilgotnosc();

      if ( debug ) Serial.println( "-----------------------------------------------" );


      delay( 1000 );

  }




  void UruchomZraszacz(  ) {

       //Lampka(2);

       // digitalWrite(AktZraszacz, HIGH);
       digitalWrite(tabPunkty[AktZraszacz], HIGH);

  }



  void UruchomZasilacz( int stan ) {

      if ( stan == 0 ) {
         if ( debug ) Serial.println("2. Gasze zasilacz" );
         digitalWrite(13, HIGH);
      }
      else {
         if ( debug ) Serial.println("1. Uruchamiam zasilacz" );
         digitalWrite(13, LOW);
      }

  }

  void SprawdzWilgotnosc() {
     int stan = analogRead( Wilgotnosc );
     int procent = map( stan, 0, 1023, 100, 0 );

     SumaWilgotnosc += procent;

     lcd.setCursor(0,0);
     lcd.print( "" );
     lcd.write(byte(2));
     // lcd.write(byte(3));
     lcd.print( " " + String(procent) + "% = " + String(SumaWilgotnosc) + "/" + String(IloscPobranWilgotnosc) );
  }

  void SprawdzWilgotnoscFast() {

       int Prev_AktOdczytWilgotnosc = AktOdczytWilgotnosc;

       SumaWilgotnosc = 0;
       for ( int i = 0 ; i<IloscPobranFast ; i++ ) {

           int stan = analogRead( Wilgotnosc );
           int procent = map( stan, 0, 1023, 100, 0 );

           SumaWilgotnosc += procent;

           lcd.setCursor(0,0);
           lcd.print( "" );
           lcd.write(byte(2));
           // lcd.write(byte(3));
           lcd.print( " " + String(procent) + "% = " + String(SumaWilgotnosc) + "/" + String(IloscPobranFast) );
           delay( 500 );
       }

       AktOdczytWilgotnosc = SumaWilgotnosc / IloscPobranFast;

       if ( AktOdczytWilgotnosc == Prev_AktOdczytWilgotnosc )
          WilgoRosnaca = 0;
       else if ( AktOdczytWilgotnosc < Prev_AktOdczytWilgotnosc )
          WilgoRosnaca = -1;
       else if ( AktOdczytWilgotnosc > Prev_AktOdczytWilgotnosc )
          WilgoRosnaca = 1;

       SumaWilgotnosc = 0;

  }

  void WyswietlWilgotnosc() {

     lcd.setCursor(6,1);
     lcd.print( "     " );
     lcd.write(byte(2));
     // lcd.write(byte(3));

     if ( AktOdczytWilgotnosc<10 )
        lcd.print( "0" );
     lcd.print( String(AktOdczytWilgotnosc) + "%" );
     if ( WilgoRosnaca < 0 ) {
         lcd.write( byte(6) );
     }
     else if ( WilgoRosnaca > 0 ) {
         lcd.write( byte(5) );
     }
     else {
         lcd.write( byte(7) );
     }
  }

  void WyswietlGodzine() {
      DateTime now = rtc.now();

      const char *Miesiace[12] = {
         "Styczeń", "Luty", "Marzec", "Kwiecień", "Maj", "Czerwiec",
         "Lipiec", "Sierpień", "Wrzesień", "Pażdziernik", "Listopad", "Grudzien"
      };

      String godzina = "";


      if ( now.hour() < 10 ) {
         if ( debug ) Serial.print("0");
         // lcd.print( "0" );
         godzina += "0";
      }
      if ( debug ) Serial.print(now.hour());
      //lcd.print( now.hour() );
      godzina += now.hour();

      if ( debug ) Serial.print( ':' );
      // lcd.print( ':' );
      godzina += ":";

      if ( now.minute() < 10 ) {
         if ( debug ) Serial.print("0");
         // lcd.print("0");
         godzina += "0";
      }
      if ( debug ) Serial.print(now.minute());
      // lcd.print(now.minute());
      godzina += now.minute();

      lcd.setCursor(0 ,1);
      lcd.write(byte(0));

      lcd.print( godzina );


      /*
    char buf[100];
    strncpy(buf,"DD.MM.YYYY  hh:mm:ss\0",100);
    lcd.print(now.format(buf));
    */


      if ( debug ) Serial.print( "     | [" );

      if ( debug ) Serial.print(now.day());
      if ( debug ) Serial.print(' ');
      if ( debug ) Serial.print( Miesiace[now.month()-1] );
      if ( debug ) Serial.println("]");

  }


  void Lampka( int modul ) {

     // start systemu
     if ( modul == 0 ) {
          digitalWrite(12, HIGH);
          delay( 1500 );
          digitalWrite(12, LOW);
          delay( 700 );

          digitalWrite(12, HIGH);
          delay( 1500 );
          digitalWrite(12, LOW);
          delay( 700 );

          digitalWrite(12, HIGH);
          delay( 1500 );
          digitalWrite(12, LOW);
          delay( 700 );
     }

     // koniec pracy
     else if ( modul == 1 ) {
            digitalWrite(12, HIGH);
            delay( 100 );
            digitalWrite(12, LOW);
            delay( 100 );
            digitalWrite(12, HIGH);
            delay( 100 );
            digitalWrite(12, LOW);
            delay( 100 );
            digitalWrite(12, HIGH);
            delay( 100 );
            digitalWrite(12, LOW);
            delay( 100 );
     }

     // zmiana zraszacza
     else if ( modul == 2 ) {
            digitalWrite(12, HIGH);
            delay( 100 );
            digitalWrite(12, LOW);
     }

  }
