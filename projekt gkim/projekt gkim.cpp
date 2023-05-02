#include <exception>
#include <string.h>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <algorithm>
#include <vector>
#include <SDL.h>
using namespace std;

SDL_Window* window = NULL;
SDL_Surface* screen = NULL;

#define szerokosc 512
#define wysokosc 340

#define tytul "GKiM2022 - Projekt - zespol 23"


void setPixel(int x, int y, Uint8 R, Uint8 G, Uint8 B);
SDL_Color getPixel(int x, int y);

void czyscEkran(Uint8 R, Uint8 G, Uint8 B);

void Funkcja1();
void Funkcja2();
void Funkcja3();
void Funkcja4();
void Funkcja5();
void Funkcja6();
void Funkcja7();
void Funkcja8();
void Funkcja9();

Uint8 z24RGBdo5RGB(SDL_Color kolor);
SDL_Color z5RGBdo24RGB(Uint8 kolor5bit);

Uint8 z24RGBdo5BW(SDL_Color kolor);
SDL_Color z5BWdo24RGB(Uint8 kolor5bit);

void narysujPalete(int px, int py, SDL_Color paleta5[]);

int ileKolorw5 = 0;
bool R_less(SDL_Color const& l, SDL_Color const& r);
bool G_less(SDL_Color const& l, SDL_Color const& r);
bool B_less(SDL_Color const& l, SDL_Color const& r);
Uint8 najwiekszaRoznica(int start, int koniec);
void sortujKubelekColor(int start, int koniec);
void sortujKubelekColor(int start, int koniec, int ktoraWartosc);
void medianCutColor(int start, int koniec, int iteracja);
int dystansKolorow(const SDL_Color& kolA, const SDL_Color& kolB);
int znajdzSasiadaColor(SDL_Color kolor, SDL_Color paleta5[]);

SDL_Color paleta5D[32];
SDL_Color obrazek[(szerokosc / 2) * (wysokosc / 2)];

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
string trybObrazu(char numer_trybu);

void greyDithering();
void colorDithering5();
void colorDithering();

void zapiszObraz5BW();
void zapiszObraz5N();
void zapiszObraz5D();
void zapiszObraz(vector<Uint8>& obraz, Uint8 identyfikator, Uint16 szerokoscOb, Uint16 wysokoscOb);

void odczytajObraz5();

vector<Uint8> zapakuj5(vector<Uint8>& dane);
vector<Uint8> odpakuj5(const vector<Uint8>& dane);

vector<Uint8> kompresjaRLE(const vector<Uint8>& obraz);
vector<Uint8> dekompresjaRLE(const vector<Uint8>& wejscie);
vector<Uint8> dekompresjaRLE(const vector<Uint8>& wejscie, int start);


string trybObrazu(char numer_trybu)
{
    switch ((int)numer_trybu)
    {
    case 0:
        return "BW";
    case 1:
        return "BW_Kompresja";
    case 2:
        return "BW_Dithering";
    case 3:
        return "BW_Dithering_Kompresja";
    case 4:
        return "Narzucona";
    case 5:
        return "Narzucona_Kompresja";
    case 6:
        return "Narzucona_Dithering";
    case 7:
        return "Narzucona_Dithering_Kompresja";
    case 8:
        return "Dedykowana";
    case 9:
        return "Dedykowana_Kompresja";
    case 10:
        return "Dedykowana_Dithering";
    case 11:
        return "Dedykowana_Dithering_Kompresja";
    default:
        return "ERROR";
    }
}

vector<uint8_t> zapakuj5(vector<Uint8>& dane)
{
    vector<Uint8> pakunek;
    Uint8 fiveBitVal = 0;
    int counter = 0;

    for (int i = 0; i < dane.size() % 5; i++) //zeby bylo podzielne przez 5
        dane.push_back(0);

    for (int i = 0; i < dane.size(); i++, counter++)
    {
        fiveBitVal = 0;
        switch (counter % 5)
        {
        case 0:
            fiveBitVal = (dane[i] & 31) << 3;

            if (i < dane.size() - 1)
                fiveBitVal |= (dane[i + 1] >> 2);
            break;
        case 1:
            fiveBitVal = (dane[i] & 3) << 6;

            if (i < dane.size() - 1)
                fiveBitVal |= (dane[i + 1] & 31) << 1;
            if (i < dane.size() - 2)
            {
                fiveBitVal |= (dane[i + 2] & 16) >> 4;
                i++;
            }
            break;
        case 2:
            fiveBitVal = (dane[i] & 15) << 4;

            if (i < dane.size() - 1)
                fiveBitVal |= (dane[i + 1] & 30) >> 1;
            break;
        case 3:
            fiveBitVal = (dane[i] & 1) << 7;

            if (i < dane.size() - 1)
                fiveBitVal |= (dane[i + 1] & 31) << 2;

            if (i < dane.size() - 2)
            {
                fiveBitVal |= (dane[i + 2] & 24) >> 3;
                i++;
            }
            break;
        case 4:
            fiveBitVal = (dane[i] & 7) << 5;

            if (i < dane.size() - 1)
            {
                fiveBitVal |= (dane[i + 1] & 31);
                i++;
            }
            break;
        default:
            break;
        }

        pakunek.push_back(fiveBitVal);
    }

    return pakunek;
}

vector<uint8_t> odpakuj5(const vector<uint8_t>& pakunek)
{
    vector<uint8_t> dane;
    int counter = 0;
    for (int i = 0; i < pakunek.size(); i++, counter++)
    {
        switch (counter % 5)
        {
        case 0:
            dane.push_back((pakunek[i] & 248) >> 3);
            dane.push_back((pakunek[i] & 7) << 2);
            break;
        case 1:
            dane.back() |= (pakunek[i] & 192) >> 6;
            dane.push_back((pakunek[i] & 63) >> 1);
            dane.push_back((pakunek[i] & 1) << 4);
            break;
        case 2:
            dane.back() |= (pakunek[i] & 240) >> 4;
            dane.push_back((pakunek[i] & 15) << 1);
            break;
        case 3:
            dane.back() |= (pakunek[i] & 128) >> 7;
            dane.push_back((pakunek[i] & 124) >> 2);
            dane.push_back((pakunek[i] & 3) << 3);
            break;
        case 4:
            dane.back() |= (pakunek[i] & 224) >> 5;
            dane.push_back(pakunek[i] & 31);
            break;
        default:
            break;
        }
    }
    return dane;
}

vector<Uint8> kompresjaRLE(const vector<Uint8>& wejscie)
{
    vector<Uint8> compressed;
    for (size_t i = 0; i < wejscie.size();) {
        int count = 1;
        int value = wejscie[i];
        // po kompresji moga wystapic liczby wieksze niz 5 bit: na przyklad jak jeden kolor powtarza sie 254 razy
        // dlatego pakowanie cos nie chce dzialac // ...&& count < 254)
        while (i + count < wejscie.size() && wejscie[i + count] == value && count < 31)
        {
            ++count;
        }
        compressed.push_back(count);
        compressed.push_back(value);
        i += count;
    }
    return compressed;
}

vector<Uint8> dekompresjaRLE(const vector<Uint8>& wejscie)
{
    vector<Uint8> decompressed;
    for (size_t i = 0; i < wejscie.size() - 1; i += 2) {
        int count = wejscie[i];
        int value = wejscie[i + 1];
        for (int j = 0; j < count; ++j) {
            decompressed.push_back(value);
        }
    }
    return decompressed;
}
vector<Uint8> dekompresjaRLE(const vector<Uint8>& wejscie, int start)
{
    vector<Uint8> decompressed;
    for (size_t i = start; i < wejscie.size() - 1; i += 2) {
        int count = wejscie[i];
        int value = wejscie[i + 1];
        for (int j = 0; j < count; ++j) {
            decompressed.push_back(value);
        }
    }
    return decompressed;
}

void greyDithering() {
    int R, G, B;
    float BW;
    float bledy[(szerokosc / 2) + 2][(wysokosc / 2) + 1];
    memset(bledy, 0, sizeof(bledy)); //ywpelnienie tablicy bledow zerami

    float blad;
    int przesuniecie = 1;

    for (int x = 0; x < szerokosc / 2; x++) {
        for (int y = 0; y < wysokosc / 2; y++) {
            R = getPixel(x, y).r; //bierzemy kolor z obrazka
            G = getPixel(x, y).g;
            B = getPixel(x, y).b;

            BW = 0.299 * R + 0.587 * G + 0.114 * B; //zamieniamy go na BW

            BW += bledy[x + przesuniecie][y]; //przenoscimy wartosc 
            Uint8 wartosc = round(BW / 7.96875) * 7.96875;
            setPixel(x + szerokosc / 2, y, wartosc, wartosc, wartosc);
            blad = BW - wartosc;

            bledy[x + przesuniecie + 1][y    ] += blad * 7.0 / 16.0;
            bledy[x + przesuniecie - 1][y + 1] += blad * 3.0 / 16.0;
            bledy[x + przesuniecie    ][y + 1] += blad * 5.0 / 16.0;
            bledy[x + przesuniecie + 1][y + 1] += blad * 1.0 / 16.0;
        }
    }
    SDL_UpdateWindowSurface(window);
}

void colorDithering5() {
    int R, G, B;
    float bledyR[(szerokosc / 2) + 2][(wysokosc / 2) + 1];
    float bledyG[(szerokosc / 2) + 2][(wysokosc / 2) + 1];
    float bledyB[(szerokosc / 2) + 2][(wysokosc / 2) + 1];

    memset(bledyR, 0, sizeof(bledyR));
    memset(bledyG, 0, sizeof(bledyG));
    memset(bledyB, 0, sizeof(bledyB));

    float bladR = 0;
    float bladG = 0;
    float bladB = 0;
    
    int przesuniecie = 1;
    SDL_Color kolor;

    for (int x = 0; x < szerokosc / 2; x++) {
        for (int y = 0; y < wysokosc / 2; y++) {

            kolor = getPixel(x, y);
            R = kolor.r;
            G = kolor.g;
            B = kolor.b;

            R += bledyR[x + przesuniecie][y];
            G += bledyG[x + przesuniecie][y];
            B += bledyB[x + przesuniecie][y];

            Uint8 newR = round(R / 7.75) * 7.75;
            Uint8 newG = round(G / 7.75) * 7.75;
            Uint8 newB = round(B / 7.75) * 7.75;
            setPixel(x + szerokosc / 2, y + wysokosc / 2, newR, newG, newB);
            bladR = R - newR;
            bladG = G - newG;
            bladB = B - newB;

            bledyR[x + przesuniecie + 1][y    ] += bladR * 7.0 / 16.0;
            bledyR[x + przesuniecie - 1][y + 1] += bladR * 3.0 / 16.0;
            bledyR[x + przesuniecie    ][y + 1] += bladR * 5.0 / 16.0;
            bledyR[x + przesuniecie + 1][y + 1] += bladR * 1.0 / 16.0;

            bledyG[x + przesuniecie + 1][y    ] += bladG * 7.0 / 16.0;
            bledyG[x + przesuniecie - 1][y + 1] += bladG * 3.0 / 16.0;
            bledyG[x + przesuniecie    ][y + 1] += bladG * 5.0 / 16.0;
            bledyG[x + przesuniecie + 1][y + 1] += bladG * 1.0 / 16.0;

            bledyB[x + przesuniecie + 1][y    ] += bladB * 7.0 / 16.0;
            bledyB[x + przesuniecie - 1][y + 1] += bladB * 3.0 / 16.0;
            bledyB[x + przesuniecie    ][y + 1] += bladB * 5.0 / 16.0;
            bledyB[x + przesuniecie + 1][y + 1] += bladB * 1.0 / 16.0;
        }
    }
    SDL_UpdateWindowSurface(window);
}

//nie uzyte ale moze byc alternatywą
void colorDithering() { 
    int R, G, B;
    float bledyR[(szerokosc / 2) + 2][(wysokosc / 2) + 1];
    float bledyG[(szerokosc / 2) + 2][(wysokosc / 2) + 1];
    float bledyB[(szerokosc / 2) + 2][(wysokosc / 2) + 1];

    memset(bledyR, 0, sizeof(bledyR));
    memset(bledyG, 0, sizeof(bledyG));
    memset(bledyB, 0, sizeof(bledyB));

    float bladR = 0;
    float bladG = 0;
    float bladB = 0;

    Uint8 Rset = 0;
    Uint8 Gset = 0;
    Uint8 Bset = 0;

    int przesuniecie = 1;
    SDL_Color kolor;

    for (int x = 0; x < szerokosc / 2; x++) {
        for (int y = 0; y < wysokosc / 2; y++) {

            kolor = getPixel(x + szerokosc / 2, y);
            R = kolor.r;
            G = kolor.g;
            B = kolor.b;

            R += bledyR[x + przesuniecie][y];
            G += bledyG[x + przesuniecie][y];
            B += bledyB[x + przesuniecie][y];

            if (R < 128) {
                Rset = 0;
                bladR = R;
            }
            else {
                Rset = 255;
                bladR = R - 255;
            }

            if (G < 128) {
                Gset = 0;
                bladG = G;
            }
            else {
                Gset = 255;
                bladG = G - 255;
            }

            if (B < 128) {
                Bset = 0;
                bladB = B;
            }
            else {
                Bset = 255;
                bladB = B - 255;
            }
            setPixel(x + szerokosc / 2, y + wysokosc / 2 , Rset, Gset, Bset);

            bledyR[x + przesuniecie + 1][y] += bladR * 7.0 / 16.0;
            bledyR[x + przesuniecie - 1][y + 1] += bladR * 3.0 / 16.0;
            bledyR[x + przesuniecie][y + 1] += bladR * 5.0 / 16.0;
            bledyR[x + przesuniecie + 1][y + 1] += bladR * 1.0 / 16.0;

            bledyG[x + przesuniecie + 1][y] += bladG * 7.0 / 16.0;
            bledyG[x + przesuniecie - 1][y + 1] += bladG * 3.0 / 16.0;
            bledyG[x + przesuniecie][y + 1] += bladG * 5.0 / 16.0;
            bledyG[x + przesuniecie + 1][y + 1] += bladG * 1.0 / 16.0;

            bledyB[x + przesuniecie + 1][y] += bladB * 7.0 / 16.0;
            bledyB[x + przesuniecie - 1][y + 1] += bladB * 3.0 / 16.0;
            bledyB[x + przesuniecie][y + 1] += bladB * 5.0 / 16.0;
            bledyB[x + przesuniecie + 1][y + 1] += bladB * 1.0 / 16.0;
        }
    }
    SDL_UpdateWindowSurface(window);
}

void zapiszObraz5BW()
{
    SDL_Color kolor;
    const Uint16 szerokoscObrazka = szerokosc / 2;
    const Uint16 wysokoscObrazka = wysokosc / 2;
    Uint8 identyfikator = TRYB_BW;
    Uint8 wartosc;

    char czyKompresowac = 't';
    char czyDithering = 't';

    vector<Uint8> obraz;

    cout << "Zapisujemy plik w trybie " << trybObrazu(identyfikator) << " do 'obraz5.bin'" << endl;
    cout << "czy plik ma byc skompresowany ? (t/n)" << endl;
    cin >> czyKompresowac;
    cout << "czy obrazek ma byc z ditheringiem ? (t/n)" << endl;
    cin >> czyDithering;

    if (czyDithering == 't') {
        identyfikator = TRYB_BW_DITHERING;
        greyDithering();
        for (int y = 0; y < wysokoscObrazka; y++)
        {
            for (int x = 0; x < szerokoscObrazka; x++)
            {
                kolor = getPixel(x + szerokoscObrazka, y);
                wartosc = z24RGBdo5BW(kolor);
                obraz.push_back(wartosc);
                //setPixel(x + szerokoscObrazka, y, 0, 0, 0); //reset tla
            }
        }
    }
    else {
        for (int y = 0; y < wysokoscObrazka; y++)
        {
            for (int x = 0; x < szerokoscObrazka; x++)
            {
                kolor = getPixel(x, y);
                wartosc = z24RGBdo5BW(kolor);
                obraz.push_back(wartosc);
            }
        }
    }

    if (czyKompresowac == 't')
    {
        obraz = kompresjaRLE(obraz);
        identyfikator++;
    }

    zapiszObraz(obraz, identyfikator, szerokoscObrazka, wysokoscObrazka);
    cout << "zapisano" << endl;
}

void zapiszObraz5N()
{
    SDL_Color kolor;
    Uint16 szerokoscObrazka = szerokosc / 2;
    Uint16 wysokoscObrazka = wysokosc / 2;
    Uint8 wartosc;
    Uint8 identyfikator = TRYB_N;

    int dlugosc = 0;
    char czyKompresowac = 'n';
    char czyDithering = 't';
    vector<Uint8> obraz;

    cout << "Zapisujemy plik w trybie " << trybObrazu(identyfikator) << " do 'obraz5.bin'" << endl;
    cout << "czy plik ma byc skompresowany ? (t/n)" << endl;
    cin >> czyKompresowac;
    cout << "czy obrazek ma byc z ditheringiem ? (t/n)" << endl;
    cin >> czyDithering;

    if (czyDithering == 't') {
        identyfikator = TRYB_N_DITHERING;
        //for (int y = 0; y < wysokoscObrazka; y++)
        //{
        //    for (int x = 0; x < szerokoscObrazka; x++)
        //    {
        //        kolor = getPixel(x, y);
        //        wartosc = z24RGBdo5RGB(kolor);
        //        kolor = z5RGBdo24RGB(wartosc);
        //        setPixel(x + szerokoscObrazka, y, kolor.r, kolor.g, kolor.b);
        //    }
        //}
        colorDithering5();

        for (int y = 0; y < wysokoscObrazka; y++)
        {
            for (int x = 0; x < szerokoscObrazka; x++)
            {
                kolor = getPixel(x + szerokoscObrazka, y + wysokoscObrazka);
                wartosc = z24RGBdo5RGB(kolor);
                obraz.push_back(wartosc);
                //setPixel(x + szerokoscObrazka, y, 0, 0, 0); //reset tla
            }
        }
    }
    else {
        for (int y = 0; y < wysokoscObrazka; y++)
        {
            for (int x = 0; x < szerokoscObrazka; x++)
            {
                kolor = getPixel(x, y);
                wartosc = z24RGBdo5RGB(kolor);
                obraz.push_back(wartosc);
            }
        }
    }

    if (czyKompresowac == 't')
    {
        obraz = kompresjaRLE(obraz);
        identyfikator++;
    }

    zapiszObraz(obraz, identyfikator, szerokoscObrazka, wysokoscObrazka);
    cout << "zapisano" << endl;
}

void zapiszObraz5D()
{
    SDL_Color kolor;
    const Uint16 szerokoscObrazka = szerokosc / 2;
    const Uint16 wysokoscObrazka = wysokosc / 2;
    Uint8 identyfikator = TRYB_D;
    int wartosc = 0;

    int dlugosc = 0;
    char czyKompresowac = 'n';
    char czyDithering = 't';
    vector<Uint8> paleta;
    vector<Uint8> indexy;

    cout << "Zapisujemy plik w trybie " << trybObrazu(identyfikator) << " do 'obraz5.bin'" << endl;
    cout << "czy plik ma byc skompresowany ? (t/n)" << endl;
    cin >> czyKompresowac;
    cout << "czy obrazek ma byc z ditheringiem ? (t/n)" << endl;
    cin >> czyDithering;

    int ilePikseli = 0;
    for (int y = 0; y < wysokoscObrazka; y++)
    {
        for (int x = 0; x < szerokoscObrazka; x++)
        {
            kolor = getPixel(x, y);
            obrazek[ilePikseli] = { kolor.r, kolor.g, kolor.b };
            ilePikseli++;
        }
    }

    //tworzenie palety kolorow 5bit
    ileKolorw5 = 0; //musi tu byc resetowane bo przy drugim zapisanym obrazku w jednej sesji bierze poprzednia palete
    medianCutColor(0, ilePikseli, 5);

    //zapisywanie palety do pliku
    for (int i = 0; i < 32; i++)
    {
        paleta.push_back(paleta5D[i].r);
        paleta.push_back(paleta5D[i].g);
        paleta.push_back(paleta5D[i].b);
    }

    //zapisywanie danego indesku pixela dla naszej palety
    Uint8 indexKoloru;
    for (int y = 0; y < wysokoscObrazka; y++)
    {
        for (int x = 0; x < szerokoscObrazka; x++)
        {
            kolor = getPixel(x, y);
            indexKoloru = znajdzSasiadaColor(kolor, paleta5D);
            indexy.push_back(indexKoloru);
        }
    }

    if (czyDithering == 't') {
        identyfikator = TRYB_D_DITHERING;
        for (int y = 0; y < wysokoscObrazka; y++)
        {
            for (int x = 0; x < szerokoscObrazka; x++)
            {
                Uint8 indexKolor = indexy[y * szerokoscObrazka + x];
                setPixel(x + szerokoscObrazka, y, paleta5D[indexKolor].r, paleta5D[indexKolor].g, paleta5D[indexKolor].b);
            }
        }
        colorDithering5();
        indexy.clear();
        for (int y = 0; y < wysokoscObrazka; y++)
        {
            for (int x = 0; x < szerokoscObrazka; x++)
            {
                kolor = getPixel(x + szerokoscObrazka, y + wysokoscObrazka);
                indexKoloru = znajdzSasiadaColor(kolor, paleta5D);
                indexy.push_back(indexKoloru);
                //setPixel(x + szerokoscObrazka, y, 0, 0, 0); //reset tla
            }
        }
    }

    if (czyKompresowac == 't')
    {
        identyfikator++;
        indexy = kompresjaRLE(indexy);
    }

    for (auto& val : indexy)
        paleta.push_back(val);

    zapiszObraz(paleta, identyfikator, szerokoscObrazka, wysokoscObrazka);
    cout << "zapisano" << endl;
}

void zapiszObraz(vector<Uint8>& obraz, Uint8 identyfikator, Uint16 szerokoscOb, Uint16 wysokoscOb)
{
    cout << trybObrazu(identyfikator) << endl;

    ofstream wyjscie("obraz5.bin", ios::binary);

    wyjscie.write((char*)&identyfikator, sizeof(Uint8));
    wyjscie.write((char*)&szerokoscOb,   sizeof(Uint16));
    wyjscie.write((char*)&wysokoscOb,    sizeof(Uint16));

    // pomijamy tryb z paleta dedykowana bo zapisujemy na niej wartosci wieksze niz 5 bit
    if (identyfikator < TRYB_D)
        obraz = zapakuj5(obraz);

    for (auto& pixel : obraz)
        wyjscie.write((char*)&pixel, sizeof(Uint8));

    wyjscie.close();
}

void odczytajObraz5()
{
    SDL_Color kolor;
    Uint16 szerokoscObrazka = 0;
    Uint16 wysokoscObrazka = 0;
    Uint8 identyfikator;
    Uint8 wartosc = 0;

    cout << "Odczytujemy plik 'obraz5.bin' uzywamy metody read()" << endl;
    ifstream wejscie("obraz5.bin", ios::binary);

    wejscie.read((char*)&identyfikator,    sizeof(Uint8));
    wejscie.read((char*)&szerokoscObrazka, sizeof(Uint16));
    wejscie.read((char*)&wysokoscObrazka,  sizeof(Uint16));

    cout << "id:" << trybObrazu(identyfikator) << endl;
    cout << "szeroksoc:" << szerokoscObrazka << endl;
    cout << "wyskosc:" << wysokoscObrazka << endl;

    vector<Uint8> dane;
    while (!wejscie.eof())
    {
        wejscie.read((char*)&wartosc, sizeof(Uint8));
        dane.push_back(wartosc);
    }

    if (identyfikator < TRYB_D)
        dane = odpakuj5(dane);

    if (identyfikator > TRYB_D_DITHERING_KOMPRESJA) {
        cout << "[error]: nieprawidlowy format obrazu" << endl;
        return;
    }
    if (identyfikator == TRYB_BW || identyfikator == TRYB_BW_DITHERING)
    {
        for (int y = 0; y < wysokoscObrazka; y++)
        {
            for (int x = 0; x < szerokoscObrazka; x++)
            {
                //wejscie.read((char*)&wartosc, sizeof(Uint8));
                wartosc = dane[y * szerokoscObrazka + x];
                kolor = z5BWdo24RGB(wartosc);
                setPixel(x + szerokoscObrazka, y, kolor.r, kolor.g, kolor.b);
            }
        }
    }
    else if (identyfikator == TRYB_BW_KOMPRESJA || identyfikator == TRYB_BW_DITHERING_KOMPRESJA)
    {
        dane = dekompresjaRLE(dane);

        for (int y = 0; y < wysokoscObrazka; y++)
        {
            for (int x = 0; x < szerokoscObrazka; x++)
            {
                if (y * szerokoscObrazka + x >= dane.size())
                    break;
                kolor = z5BWdo24RGB(dane[y * szerokoscObrazka + x]);
                setPixel(x + szerokoscObrazka, y, kolor.r, kolor.g, kolor.b);
            }
        }
    }
    else if (identyfikator == TRYB_N || identyfikator == TRYB_N_DITHERING)
    {
        for (int y = 0; y < wysokoscObrazka; y++)
        {
            for (int x = 0; x < szerokoscObrazka; x++)
            {
                //wejscie.read((char*)&wartosc, sizeof(Uint8));
                wartosc = dane[y * szerokoscObrazka + x];
                kolor = z5RGBdo24RGB(wartosc);
                setPixel(x + szerokoscObrazka, y, kolor.r, kolor.g, kolor.b);
            }
        }
    }
    else if (identyfikator == TRYB_N_KOMPRESJA || identyfikator == TRYB_N_DITHERING_KOMPRESJA)
    {
        dane = dekompresjaRLE(dane);

        for (int y = 0; y < wysokoscObrazka; y++)
        {
            for (int x = 0; x < szerokoscObrazka; x++)
            {
                if (y * szerokoscObrazka + x >= dane.size())
                    break;
                kolor = z5RGBdo24RGB(dane[y * szerokoscObrazka + x]);
                setPixel(x + szerokoscObrazka, y, kolor.r, kolor.g, kolor.b);
            }
        }
    }
    else if (identyfikator == TRYB_D || identyfikator == TRYB_D_DITHERING)
    {
        Uint8 indexKolor;

        int count = 0;
        for (int i = 0; i < 32; i++)
        {
            //wejscie.read((char*)&(paleta5D[i].r), sizeof(Uint8));
            //wejscie.read((char*)&paleta5D[i].g, sizeof(Uint8));
            //wejscie.read((char*)&paleta5D[i].b, sizeof(Uint8));
            paleta5D[i].r = dane[count];
            paleta5D[i].g = dane[count + 1];
            paleta5D[i].b = dane[count + 2];
            count += 3;
        }
        narysujPalete(szerokosc / 2, wysokosc / 2, paleta5D);

        for (int y = 0; y < wysokoscObrazka; y++)
        {
            for (int x = 0; x < szerokoscObrazka; x++)
            {
                //wejscie.read((char*)&indexKolor, sizeof(Uint8));
                indexKolor = dane[y * szerokoscObrazka + x + (32 * 3)];
                setPixel(x + szerokoscObrazka, y, paleta5D[indexKolor].r, paleta5D[indexKolor].g, paleta5D[indexKolor].b);
            }
        }
    }
    else if (identyfikator == TRYB_D_KOMPRESJA || identyfikator == TRYB_D_DITHERING_KOMPRESJA)
    {
        int count = 0;
        for (int i = 0; i < 32; i++)
        {
            //wejscie.read((char*)&(paleta5D[i].r), sizeof(Uint8));
            //wejscie.read((char*)&paleta5D[i].g, sizeof(Uint8));
            //wejscie.read((char*)&paleta5D[i].b, sizeof(Uint8));
            paleta5D[i].r = dane[count];
            paleta5D[i].g = dane[count + 1];
            paleta5D[i].b = dane[count + 2];
            count += 3;
        }
        narysujPalete(szerokosc / 2, wysokosc / 2, paleta5D);

        dane = dekompresjaRLE(dane, count); //pomijamy palete
        Uint8 indexKolor;

        for (int y = 0; y < wysokoscObrazka; y++)
        {
            for (int x = 0; x < szerokoscObrazka; x++)
            {
                if (y * szerokoscObrazka + x >= dane.size())
                    break;
                indexKolor = dane[y * szerokoscObrazka + x];
                setPixel(x + szerokoscObrazka, y, paleta5D[indexKolor].r, paleta5D[indexKolor].g, paleta5D[indexKolor].b);
            }
        }
    }

    if (wejscie.good())
        wejscie.close();
}

void narysujPalete(int px, int py, SDL_Color paleta5[]) {

    int ktory_kolor = 0;
    int ile_pixel_x = (szerokosc / 2) / 8;
    int ile_pixel_y = (wysokosc / 2) / 4;

    for (int y = py; y < py + wysokosc / 2; y += ile_pixel_y)
        for (int x = px; x < px + szerokosc / 2; x += ile_pixel_x)
        {
            for (int i = 0; i < ile_pixel_x; i++)
            {
                for (int j = 0; j < ile_pixel_y; j++)
                    setPixel(x + i, y + j, paleta5[ktory_kolor].r, paleta5[ktory_kolor].g, paleta5[ktory_kolor].b);

            }
            if (ktory_kolor < 32)
                ktory_kolor++;
        }
}

void sortujKubelekColor(int start, int koniec)
{
    int minimum = start;
    for (int p = start; p < koniec; p++)
    {
        minimum = p;
        for (int i = p; i < koniec; i++)
        {
            if ((obrazek[i].r + obrazek[i].g + obrazek[i].b) < (obrazek[minimum].r + obrazek[minimum].g + obrazek[minimum].b))
                minimum = i;
        }
        swap(obrazek[p], obrazek[minimum]);
    }
}

bool R_less(SDL_Color const& l, SDL_Color const& r) { return l.r < r.r; }
bool G_less(SDL_Color const& l, SDL_Color const& r) { return l.g < r.g; }
bool B_less(SDL_Color const& l, SDL_Color const& r) { return l.b < r.b; }

void sortujKubelekColor(int start, int koniec, int ktoraWartosc)
{
    if (ktoraWartosc == 0) // sortujemy po R
        sort(&obrazek[start], &obrazek[koniec], &R_less);
    else if (ktoraWartosc == 1) // sortujemy po G
        sort(&obrazek[start], &obrazek[koniec], &G_less);
    else if (ktoraWartosc == 2) // sortujemy po B
        sort(&obrazek[start], &obrazek[koniec], &B_less);
}

Uint8 najwiekszaRoznica(int start, int koniec) // majwiekszy przedzial w skladowej dla danego kubelka 
{
    SDL_Color minKol = obrazek[start], maxKol = obrazek[start];

    for (int i = start + 1; i < koniec; i++)
    {
        if (minKol.r > obrazek[i].r) minKol.r = obrazek[i].r;
        if (minKol.g > obrazek[i].g) minKol.g = obrazek[i].g;
        if (minKol.b > obrazek[i].b) minKol.b = obrazek[i].b;

        if (maxKol.r < obrazek[i].r) maxKol.r = obrazek[i].r;
        if (maxKol.g < obrazek[i].g) maxKol.g = obrazek[i].g;
        if (maxKol.b < obrazek[i].b) maxKol.b = obrazek[i].b;
    }

    if (maxKol.r - minKol.r > maxKol.g - minKol.g)
    {
        if (maxKol.r - minKol.r > maxKol.b - minKol.b)
            return 0;
    }
    else
    {
        if (maxKol.g - minKol.g > maxKol.b - minKol.b)
            return 1;
    }
    return 2;
}

void medianCutColor(int start, int koniec, int iteracja)
{
    if (iteracja > 0)
    {
        // sort kubełek, a później dzielimy na dwa mniejsze
        // konieczne jest ustalić wzgledem ktorej składowej sortować kubełek
        // sortujKubelekColor(start, koniec); 
        Uint8 ktoraSkladowa = najwiekszaRoznica(start, koniec);
        sortujKubelekColor(start, koniec, ktoraSkladowa);
        int srodek = (start + koniec) / 2; // wyznaczamy środek podziału zakładając, że zawsze
        medianCutColor(start, srodek, iteracja - 1);
        medianCutColor(srodek + 1, koniec, iteracja - 1);
    }
    else
    {
        //budujemy palete uśredniając kolory z określonego kubełka
        int sumaR = 0;
        int sumaG = 0;
        int sumaB = 0;
        for (int p = start; p < koniec; p++)
        {
            sumaR += obrazek[p].r;
            sumaG += obrazek[p].g;
            sumaB += obrazek[p].b;
        }
        Uint8 noweR = sumaR / (koniec - start);
        Uint8 noweG = sumaG / (koniec - start);
        Uint8 noweB = sumaB / (koniec - start);
        SDL_Color nowyKolor = { noweR, noweG, noweB };
        paleta5D[ileKolorw5] = nowyKolor;
        ileKolorw5++;
    }
}

int dystansKolorow(const SDL_Color& kolA, const SDL_Color& kolB)
{
    int deltaR = kolA.r - kolB.r;
    int deltaG = kolA.g - kolB.g;
    int deltaB = kolA.b - kolB.b;

    return (deltaR * deltaR) + (deltaG * deltaG) + (deltaB * deltaB);
}

int znajdzSasiadaColor(SDL_Color kolor, SDL_Color paleta5[])
{
    int minimum = 195075;
    int indexMinimum = 0;

    SDL_Color kolorPaleta;
    int odleglosc;

    for (int i = 0; i < 32; i++)
    {
        kolorPaleta = paleta5[i];
        // odleglosc = abs((kolor.r + kolor.g + kolor.b) - (kolorPaleta.r + kolorPaleta.g + kolorPaleta.b));
        odleglosc = dystansKolorow(kolor, paleta5[i]);
        if (odleglosc < minimum)
        {
            minimum = odleglosc;
            indexMinimum = i;
        }
    }
    return indexMinimum;
}

Uint8 z24RGBdo5RGB(SDL_Color kolor)
{
    int R, G, B;
    int nowyR, nowyG, nowyB;

    R = kolor.r;
    G = kolor.g;
    B = kolor.b;

    nowyR = round(R * 3.0 / 255.0);
    nowyG = round(G * 3.0 / 255.0);
    nowyB = round(B * 1.0 / 255.0);

    return (nowyR << 3) + (nowyG << 1) + nowyB;
}

SDL_Color z5RGBdo24RGB(Uint8 kolor5bit)
{
    SDL_Color kolor;

    kolor.r = (kolor5bit & 0b11000) * 255.0 / 24.0;
    kolor.g = (kolor5bit & 0b00110) * 255.0 / 6.0;
    kolor.b = (kolor5bit & 0b00001) * 255.0 / 1.0;

    return kolor;
}

Uint8 z24RGBdo5BW(SDL_Color kolor)
{
    Uint8 BW = kolor.r * 0.299 +
        kolor.g * 0.587 +
        kolor.b * 0.114;

    BW = round(BW * 31.0 / 255.0);

    return BW;
}

SDL_Color z5BWdo24RGB(Uint8 kolor5bit)
{
    SDL_Color kolor;

    kolor.r = (kolor5bit) * 255.0 / 32;
    kolor.g = (kolor5bit) * 255.0 / 32;
    kolor.b = (kolor5bit) * 255.0 / 32;

    return kolor;
}

void Funkcja1() {

    zapiszObraz5BW();
    SDL_UpdateWindowSurface(window);
}

void Funkcja2() {

    zapiszObraz5N();
    SDL_UpdateWindowSurface(window);
}

void Funkcja3() {

    zapiszObraz5D();
    SDL_UpdateWindowSurface(window);
}

void Funkcja4() {

    odczytajObraz5();
    SDL_UpdateWindowSurface(window);
}

void Funkcja5() {

    SDL_UpdateWindowSurface(window);
}

void Funkcja6() {

    SDL_UpdateWindowSurface(window);
}

void Funkcja7() {

    SDL_UpdateWindowSurface(window);
}

void Funkcja8() {

    SDL_UpdateWindowSurface(window);
}

void Funkcja9() {

    SDL_UpdateWindowSurface(window);
}

void setPixel(int x, int y, Uint8 R, Uint8 G, Uint8 B)
{
    if ((x >= 0) && (x < szerokosc) && (y >= 0) && (y < wysokosc))
    {
        /* Zamieniamy poszczególne składowe koloru na format koloru piksela */
        Uint32 pixel = SDL_MapRGB(screen->format, R, G, B);

        /* Pobieramy informację ile bajtów zajmuje jeden piksel */
        int bpp = screen->format->BytesPerPixel;

        /* Obliczamy adres piksela */
        Uint8* p1 = (Uint8*)screen->pixels + (y * 2) * screen->pitch + (x * 2) * bpp;
        Uint8* p2 = (Uint8*)screen->pixels + (y * 2 + 1) * screen->pitch + (x * 2) * bpp;
        Uint8* p3 = (Uint8*)screen->pixels + (y * 2) * screen->pitch + (x * 2 + 1) * bpp;
        Uint8* p4 = (Uint8*)screen->pixels + (y * 2 + 1) * screen->pitch + (x * 2 + 1) * bpp;

        /* Ustawiamy wartość piksela, w zależnoœci od formatu powierzchni*/
        switch (bpp)
        {
        case 1: //8-bit
            *p1 = pixel;
            *p2 = pixel;
            *p3 = pixel;
            *p4 = pixel;
            break;

        case 2: //16-bit
            *(Uint16*)p1 = pixel;
            *(Uint16*)p2 = pixel;
            *(Uint16*)p3 = pixel;
            *(Uint16*)p4 = pixel;
            break;

        case 3: //24-bit
            if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
                p1[0] = (pixel >> 16) & 0xff;
                p1[1] = (pixel >> 8) & 0xff;
                p1[2] = pixel & 0xff;
                p2[0] = (pixel >> 16) & 0xff;
                p2[1] = (pixel >> 8) & 0xff;
                p2[2] = pixel & 0xff;
                p3[0] = (pixel >> 16) & 0xff;
                p3[1] = (pixel >> 8) & 0xff;
                p3[2] = pixel & 0xff;
                p4[0] = (pixel >> 16) & 0xff;
                p4[1] = (pixel >> 8) & 0xff;
                p4[2] = pixel & 0xff;
            }
            else {
                p1[0] = pixel & 0xff;
                p1[1] = (pixel >> 8) & 0xff;
                p1[2] = (pixel >> 16) & 0xff;
                p2[0] = pixel & 0xff;
                p2[1] = (pixel >> 8) & 0xff;
                p2[2] = (pixel >> 16) & 0xff;
                p3[0] = pixel & 0xff;
                p3[1] = (pixel >> 8) & 0xff;
                p3[2] = (pixel >> 16) & 0xff;
                p4[0] = pixel & 0xff;
                p4[1] = (pixel >> 8) & 0xff;
                p4[2] = (pixel >> 16) & 0xff;
            }
            break;

        case 4: //32-bit
            *(Uint32*)p1 = pixel;
            *(Uint32*)p2 = pixel;
            *(Uint32*)p3 = pixel;
            *(Uint32*)p4 = pixel;
            break;

        }
    }
}

void setPixelSurface(int x, int y, Uint8 R, Uint8 G, Uint8 B)
{
    if ((x >= 0) && (x < szerokosc * 2) && (y >= 0) && (y < wysokosc * 2))
    {
        /* Zamieniamy poszczególne składowe koloru na format koloru piksela */
        Uint32 pixel = SDL_MapRGB(screen->format, R, G, B);

        /* Pobieramy informację ile bajtów zajmuje jeden piksel */
        int bpp = screen->format->BytesPerPixel;

        /* Obliczamy adres piksela */
        Uint8* p = (Uint8*)screen->pixels + y * screen->pitch + x * bpp;

        /* Ustawiamy wartość piksela, w zależności od formatu powierzchni*/
        switch (bpp)
        {
        case 1: //8-bit
            *p = pixel;
            break;

        case 2: //16-bit
            *(Uint16*)p = pixel;
            break;

        case 3: //24-bit
            if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
                p[0] = (pixel >> 16) & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = pixel & 0xff;
            }
            else {
                p[0] = pixel & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = (pixel >> 16) & 0xff;
            }
            break;

        case 4: //32-bit
            *(Uint32*)p = pixel;
            break;
        }
    }
}

SDL_Color getPixel(int x, int y) {
    SDL_Color color;
    Uint32 col = 0;
    if ((x >= 0) && (x < szerokosc) && (y >= 0) && (y < wysokosc)) {
        //określamy pozycję
        char* pPosition = (char*)screen->pixels;

        //przesunięcie względem y
        pPosition += (screen->pitch * y * 2);

        //przesunięcie względem x
        pPosition += (screen->format->BytesPerPixel * x * 2);

        //kopiujemy dane piksela
        memcpy(&col, pPosition, screen->format->BytesPerPixel);

        //konwertujemy kolor
        SDL_GetRGB(col, screen->format, &color.r, &color.g, &color.b);
    }
    return (color);
}

SDL_Color getPixelSurface(int x, int y, SDL_Surface* surface) {
    SDL_Color color;
    Uint32 col = 0;
    if ((x >= 0) && (x < szerokosc) && (y >= 0) && (y < wysokosc)) {
        //określamy pozycję
        char* pPosition = (char*)surface->pixels;

        //przesunięcie względem y
        pPosition += (surface->pitch * y);

        //przesunięcie względem x
        pPosition += (surface->format->BytesPerPixel * x);

        //kopiujemy dane piksela
        memcpy(&col, pPosition, surface->format->BytesPerPixel);

        //konwertujemy kolor
        SDL_GetRGB(col, surface->format, &color.r, &color.g, &color.b);
    }
    return (color);
}


void ladujBMP(char const* nazwa, int x, int y)
{
    SDL_Surface* bmp = SDL_LoadBMP(nazwa);
    if (!bmp)
    {
        printf("Unable to load bitmap: %s\n", SDL_GetError());
    }
    else
    {
        SDL_Color kolor;
        for (int yy = 0; yy < bmp->h; yy++) {
            for (int xx = 0; xx < bmp->w; xx++) {
                kolor = getPixelSurface(xx, yy, bmp);
                setPixel(xx, yy, kolor.r, kolor.g, kolor.b);
            }
        }
        SDL_FreeSurface(bmp);
        SDL_UpdateWindowSurface(window);
    }

}


void czyscEkran(Uint8 R, Uint8 G, Uint8 B)
{
    SDL_FillRect(screen, 0, SDL_MapRGB(screen->format, R, G, B));
    SDL_UpdateWindowSurface(window);
}



int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    window = SDL_CreateWindow(tytul, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, szerokosc * 2, wysokosc * 2, SDL_WINDOW_SHOWN);

    if (window == NULL) {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    screen = SDL_GetWindowSurface(window);
    if (screen == NULL) {
        fprintf(stderr, "SDL_GetWindowSurface Error: %s\n", SDL_GetError());
        return false;
    }
    SDL_UpdateWindowSurface(window);


    bool done = false;
    SDL_Event event;
    // główna pętla programu
    while (SDL_WaitEvent(&event)) {
        // sprawdzamy czy pojawiło się zdarzenie
        switch (event.type) {
        case SDL_QUIT:
            done = true;
            break;

            // sprawdzamy czy został wciśnięty klawisz
        case SDL_KEYDOWN: {
            // wychodzimy, gdy wciśnięto ESC
            if (event.key.keysym.sym == SDLK_ESCAPE)
                done = true;
            if (event.key.keysym.sym == SDLK_1)
                Funkcja1();
            if (event.key.keysym.sym == SDLK_2)
                Funkcja2();
            if (event.key.keysym.sym == SDLK_3)
                Funkcja3();
            if (event.key.keysym.sym == SDLK_4)
                Funkcja4();
            if (event.key.keysym.sym == SDLK_5)
                Funkcja5();
            if (event.key.keysym.sym == SDLK_6)
                Funkcja6();
            if (event.key.keysym.sym == SDLK_7)
                Funkcja7();
            if (event.key.keysym.sym == SDLK_8)
                Funkcja8();
            if (event.key.keysym.sym == SDLK_9)
                Funkcja9();
            if (event.key.keysym.sym == SDLK_a)
                ladujBMP("23-1.bmp", 0, 0);
            if (event.key.keysym.sym == SDLK_s)
                ladujBMP("23-2.bmp", 0, 0);
            if (event.key.keysym.sym == SDLK_d)
                ladujBMP("obrazek3.bmp", 0, 0);
            if (event.key.keysym.sym == SDLK_f)
                ladujBMP("obrazek4.bmp", 0, 0);
            if (event.key.keysym.sym == SDLK_g)
                ladujBMP("obrazek5.bmp", 0, 0);
            if (event.key.keysym.sym == SDLK_h)
                ladujBMP("obrazek6.bmp", 0, 0);
            if (event.key.keysym.sym == SDLK_j)
                ladujBMP("obrazek7.bmp", 0, 0);
            if (event.key.keysym.sym == SDLK_b)
                czyscEkran(0, 0, 10);
            else
                break;
        }
        }
        if (done) break;
    }

    if (screen) {
        SDL_FreeSurface(screen);
    }

    if (window) {
        SDL_DestroyWindow(window);
    }


    SDL_Quit();
    return 0;
}
