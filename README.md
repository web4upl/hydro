# hydro
# Zasada działania: 
Ustawiamy godzinę nawadniania, np: 04:00 rano, oraz % wilgotności trawnika od jakiego rozpoczynamy zraszanie (aby pominąć podlewanie w czasie i po deszczu). O zadanej godzinie system pobiera % wilgotności i jeśli jest mniejsza niż ustawiona rozpoczyna zraszanie. Aby uzyskać duże ciśnienie wody, zraszacze włączane są po kolei na określoną ilość minut. Cykl powtarza się codziennie o ustawionej godzinie. W między czasie na wyświetlaczu widoczny jest stan systemu (podlewa lub stoi), aktualna godzina oraz wilgotność gleby (aktualizowana co godzinę). 

--------------------------------------------------------------------------------------------------------------------------
# UŻYTE ELEMENTY 
1x arduino mega 

1x RTC Czujnik czasu rzeczywistego #DS1302 #SPI

1x Czujnik wilgotności gleby #FC-28

2x 4ch MOFSET (zestaw przekaźników) #IRF540 V4.0 

1x LCD  2x16

1x Potencjometr 

1x Zasilacz ATX 
1x Zasilacz komputerowy 20V 


8x elektrozawór 12v  / 1/2 cala 

1x gniazdo 2 kanałowe (wtyczka internetowa) 

100m kabel internetowy RJ45 + 2x wtyczka 

100m wąż ogrodowy 1/2 cala 

8x trójnik na wąż ogrodowy 

24x zaciski metalowe (skręcane)

16x szybkozłączka (gwint 1/2 cala - szybko złączka męska) 

16x szybkozłączka (wąż->żeńska)


8x zraszacz wysuwany Rain Bird 3504 

8x przyłączenie zraszacz->wąż 


3x kran (wejście wody do systemu + 2x zakończenie np. na ogrodzie) 

8x rura PCV + 16 zatyczek do rur PCV 

--------------------------------------------------------------------------------------------------------------------------
# SCHEMAT POŁĄCZENIOWY 
Przewodem internetowym 8 żył rozprowadzone jest zasilanie do zaworów. 
- 2 przewody (z pierwszego kabla) na + [plus] (szeregowo do każdego zaworu) 
- po jednym przewodzie dla każdego zaworu na - (GND) 
     - pierwszy kabel (RJ-45): 
          - 2x zasilanie do każdego zaworu (8 szt.) 
          - 6x GND do najdalszych zaworów
     - drugi kabel (RJ-45)
          - 2x GND do pozostałych zaworów, 
          - 2x czujnik wilgotności
          - 4x wolne na przyszłość  
Oba przewody wchodzą do kotłowni i są wpięte do urządzenia przez podwójne gniazdo RJ45 

Opis pinów ARDUINO 
Vcc -> ATX 5V (always on)
GND -> ATX GND 
13  -> ATX ON (uruchomienie zasilacza/SLEEP OFF) 

52, 53, 50, 51, 48, 49 - LCD 

A15 -> Odczyt czujnika wilgotności 
A14 -> Sygnał do servo (opcjonalnie)  

A0, A1, A2, A3, A4, A5, A6, A7 - elektrozawory 

--------------------------------------------------------------------------------------------------------------------------
# KOD
Biblioteki 
  Servo.h
  Wire.h
  RTClib.h
  LiquidCrystal.h

kod w plikuL hydro3.ino 
--------------------------------------------------------------------------------------------------------------------------
# PLANY NA PRZYSZŁOŚĆ
 - czujnik przepływu cieczy (liczenie zużycia, wykrywanie awarii/przecieków) 
 - wifi do obsługi przez www / baza danych historii pomiarów / przeglądanie historii 
 - rozbudowa o 3 zraszacze z drugiej strony domu
 - rozbudowa o wąż kroplujący w sadzie 

--------------------------------------------------------------------------------------------------------------------------
# ZDJĘCIA 
[zdjęcia]
