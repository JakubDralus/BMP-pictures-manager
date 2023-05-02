# Program do obróbki obrazów BMP

- Kierunek: Informatyka w Inżynierii Komputerowej 
- Przedmiot: Grafika Komputerowa i Multimedia 


## 1. Podstawowe założenia projektu 
Celem projektu jest stworzenie aplikacji pozwalającej na modyfikacje danego obrazu **(.bmp o wymiarach 256x170)** 
Aplikacja daje możliwość konwersji z pliku BMP do nowego rodzaju pliku graficznego oraz konwersji powrotnej. 
Do dyspozycji użytkownik będzie miał dostęp do jednego z trzech formatów obrazka, paletę narzuconą, 
paletę dedykowaną oraz w skali szarości. Użytkownik będzie miał możliwość skompresowania danego obrazu, 
do kompresji zostanie użyty algorytm RLE. Jest to algorytm kompresji danych, który polega na zastąpieniu ciągu powtarzających się 
znaków ich liczbą oraz samym znakiem. W naszym projekcie obsługiwany jest alfabet wejściowy 5-bitowy. 
Przy użyciu każdej z 2 palet użytkownik ma także możliwość wyboru czy obraz ma zostać poddany ditheringowi, 
który pozwala na uzyskanie bardziej złożonych i naturalnych kolorów, mimo że w rzeczywistości są one wygenerowane z ograniczonej liczby barw. 
 
## 2. Spis funkcji używanych w projekcie 
  
- **zapiszobraz 5BW()** : 
Funkcja zapisuje dane z obrazka do pliku, w trybie odcieni szarości. W funkcji zachodzi konwersja z 24-bit RGB na 5-bit BW 	 

- **zapiszObraz 5N()** :
Funkcja zapisuje dane z obrazka do pliku, w trybie palety narzuconej. W funkcji zachodzi konwersja z 24-bit RGB na 5-bit RGB 	 
 
- **zapiszObraz 5D()** : 
Funkcja tworzy dedykowaną paletę kolorów 5-bit, na bazie pobranych danych z obrazka. Następnie zapisuje utworzoną paletę i indexy pixeli dla palety do pliku. 
 	 
- **odczytajObr az5()** : 
Funkcja na bazie wyborów użytkownika odczytuje dane i dokonuje konwersji powrotnej. Funkcja działa dla wszystkich możliwych kombinacji wyborów. 	 
 
- **MedianCutColor()** : algorytm generujący n-bitową kolorową paletę, wykorzystując algorytm median cut 
- **znajdzSasiadaColor()** : sprawdza “odległość” miedzy kolorami i wyznacza najmniejszy index. 
- **Dithering()** : polega na dodawaniu do obrazu szumu lub innych elementów, aby uzyskać bardziej złożony i naturalny wygląd. 
- **KompresjaRLE()** : kompresuje dane wejściowe 
- **dekompresjaRLE()** : dekompresuje dane wejściowe 
- **odpakuj5()** : odpakowuje skompresowane dane 
- **zapiszObraz()** : zapis obrazu wraz z danymi dotyczącymi identyfikatora, szerokości i wysokości obrazka, do pliku obraz5.bin . 
- **narysujPalete()** : funkcja pomocnicza wyświetlająca paletę 5D 
 
 
## 3. Identyfikatory użyte w projekcie
```c++
//skala szarości 
#define TRYB_BW 0 
#define TRYB_BW_KOMPRESJA 1 
#define TRYB_BW_DITHERING 2 
#define TRYB_BW_DITHERING_KOMPRESJA 3 
 
//paleta narzucona 
#define TRYB_N 4 
#define TRYB_N_KOMPRESJA 5 
#define TRYB_N_DITHERING 6 
#define TRYB_N_DITHERING_KOMPRESJA 7 
 
//paleta dedykowana 
#define TRYB_D 8 
#define TRYB_D_KOMPRESJA 9 
#define TRYB_D_DITHERING 10 
#define TRYB_D_DITHERING_KOMPRESJA 11 
```
 
## 4. Pierwsze 5 bajtów w pliku 

![image](https://user-images.githubusercontent.com/129612952/235689047-4d3b769d-3006-40a1-a8c7-bd0cfe90f57a.png)

- W pierwszym bajcie znajduje się identyfikator obrazu (tutaj 1, czyli BW) 
- W 2 i 3 bajcie znajduje się szerokość obrazu (256(10)) 
- W 4 i 5 bajcie znajduje się wysokość obrazu (170(10)) 
- W kolejnych znajdują się informacje o kolejnych pikselach obrazka od góry z lewej do prawej. 
 
## 5. Przykład działania
![dzialanie programu](https://user-images.githubusercontent.com/129612952/235689953-fbf79518-6da0-4dab-bd28-e708ed7a9f90.png)
