# hydro
# Zasada działania: 
Ustawiamy godzinę nawadniania, np: 04:00 rano, oraz % wilgotności trawnika od jakiego rozpoczynamy zraszanie (aby pominąć podlewanie w czasie i po deszczu). O zadanej godzinie system pobiera % wilgotności i jeśli jest mniejsza niż ustawiona rozpoczyna zraszanie. Aby uzyskać duże ciśnienie wody, zraszacze włączane są po kolei na określoną ilość minut. Cykl powtarza się codziennie o ustawionej godzinie. W między czasie na wyświetlaczu widoczny jest stan systemu (podlewa lub stoi), aktualna godzina oraz wilgotność gleby (aktualizowana co godzinę). 

--------------------------------------------------------------------------------------------------------------------------
# UŻYTE ELEMENTY 
1x arduino mega
1x RTC Czujnik czasu rzeczywistego 
1x Czujnik wilgotności gleby 
2x 4ch MOFSET (zestaw przekaźników) 
1x LCD 
1x Potencjometr 

8x elektrozawór 12v  / 1/2 cala 
1x gniazdo 2 kanałowe (wtyczka internetowa) 
100m kabel internetowy + 2x wtyczka 
100m wąż ogrodowy 1/2 cala 
8x trójnik na wąż ogrodowy 
24x zaciski metalowe (skręcane)
16x szybkozłączka (gwint 1/2 cala - szybko złączka męska) 
16x szybkozłączka (wąż->żeńska)

8x zraszacz wysuwanych 
8x przyłączenie zraszacz->wąż 

3x kran (wejście wody do systemu+ 2x zakończenie np. na ogrodzie) 

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
[tu schemat]


--------------------------------------------------------------------------------------------------------------------------
# KOD
[wymagane biblioteki]
[kod]

--------------------------------------------------------------------------------------------------------------------------
# PLANY NA PRZYSZŁOŚĆ
 - czujnik przepływu cieczy (liczenie zużycia, wykrywanie awarii/przecieków) 
 - wifi do obsługi przez www / baza danych historii pomiarów / przeglądanie historii 
 - rozbudowa o 3 zraszacze z drugiej strony domu
 - rozbudowa o wąż kroplujący w sadzie 

--------------------------------------------------------------------------------------------------------------------------
# ZDJĘCIA 
[zdjęcia]
