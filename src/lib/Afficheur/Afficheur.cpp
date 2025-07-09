/* 
 * File:   Afficheur.cpp
 * Author: F4GOH
 * 
 */

#include "Afficheur.h"

// Constructeur de Afficheur
// Sur la carte Ballon adresse 0x3c connecté sur SDA et SCL

Afficheur::Afficheur() :
SSD1306Wire(0x3c, SDA, SCL, GEOMETRY_128_64) {
    init();
    setFont(ArialMT_Plain_10);
    flipScreenVertically();

}

Afficheur::~Afficheur() {
}

/**
 * Méthode pour afficher un message
 * @param message String le message à afficher
 */
void Afficheur::afficher(String callsign, float frequence, int power, bool rttyEn, bool cwEn, int wpm, int repeat, bool continous, String locator, float latitude, float longitude, String comment) {
    clear();
    setFont(ArialMT_Plain_10);

    // Ligne 1 : Callsign et puissance
    drawString(0, 0, callsign);
    drawString(80, 0, String(power) + " dBm");

    // Ligne 2 : Fréquence et mode continu
    drawString(0, 11, String(frequence, 1) + " MHz");
    drawString(80, 11, "Cont:" + String(continous ? "1" : "0"));

    // Ligne 3 : RTTY, CW, WPM, Repeat (plus compact)
    drawString(0, 22, "R:" + String(rttyEn ? "1" : "0") +
            "   C:" + String(cwEn ? "1" : "0") +
            "   W:" + String(wpm) +
            "   R:" + String(repeat));

    // Ligne 4 : Locator
    drawString(0, 33, "Locator: " + locator);

    // Ligne 5 : Latitude et Longitude (réparties sur toute la largeur)
    //drawString(0, 44, "φ" + String(latitude, 4));
    //drawString(64, 44, "λ" + String(longitude, 4));
    drawXbm(0, 46, 8, 8, icon_latitude);
    drawString(10, 44, String(latitude, 4));

    drawXbm(60, 46, 8, 8, icon_longitude);
    drawString(70, 44, String(longitude, 4));

    // Ligne 6 : Commentaire centré
    int commentX = (128 - getStringWidth(comment)) / 2;
    drawString(commentX < 0 ? 0 : commentX, 53, comment);

    display();
}

void Afficheur::initAffichage(String callsign, bool cwEn, bool rttyEn) {
    clear();

    // Ligne 1 : Modes actifs (CW / RTTY)
    setFont(ArialMT_Plain_10);
    String modes = "";
    if (cwEn) modes += "CW ";
    if (rttyEn) modes += "RTTY";
    drawString(0, 0, modes);

    // Ligne 2 : Callsign en grand
    setFont(ArialMT_Plain_24);
    int xCall = (128 - getStringWidth(callsign)) / 2;
    drawString(xCall < 0 ? 0 : xCall, 16, callsign);

    // Ligne 3 : Titre "BEACON"
    setFont(ArialMT_Plain_16);
    int xBeacon = (128 - getStringWidth("BEACON")) / 2;
    drawString(xBeacon < 0 ? 0 : xBeacon, 48, "BEACON");

    display();
}

