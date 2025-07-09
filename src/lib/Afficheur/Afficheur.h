/* 
 * File:   Afficheur.cpp
 * Author: F4GOH
 * 
 * spécialisation de SSD1306Wire
 */

#ifndef AFFICHEUR_H
#define AFFICHEUR_H

#include <Arduino.h>
#include <SSD1306Wire.h>

const unsigned char icon_latitude[] PROGMEM = {
    0x00, //
    0x18, //    ██
    0x18, //    ██
    0x18, //    ██
    0xFF, // ████████
    0x18, //    ██
    0x18, //    ██
    0x18  //    ██
};

const unsigned char icon_longitude[] PROGMEM = {
    0x00,
    0x66, //  ██  ██
    0x66, //  ██  ██
    0xFF, // ████████
    0xFF, // ████████
    0x66, //  ██  ██
    0x66, //  ██  ██
    0x00
};

class Afficheur : public SSD1306Wire {
public:

    Afficheur();
    virtual ~Afficheur();

    void afficher(String callsign, float frequence, int power, bool rttyEn, bool cwEn, int wpm, int repeat, bool continous, String locator, float latitude, float longitude, String comment);
    void initAffichage(String callsign, bool cwEn, bool rttyEn);
    
private:


};

#endif /* AFFICHEUR_H */
