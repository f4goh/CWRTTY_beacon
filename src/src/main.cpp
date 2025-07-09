/*
 CW and RTTY beacon with TTGO LoRa TBEAM for fox hunting
 F4GOH 7/2025
 */
#include <Arduino.h>
#include <RadioLib.h>
#include <TinyGPS++.h>
#include <power_management.h>
#include <Wire.h>
#include <Preferences.h>
#include <Menu.h>  
#include <Locator.h>  
#include "nvs_flash.h"
#include <Afficheur.h>          // Afficheur SSD1306


#define LED 4 //used for ptt
#define BP 38 //used to force tx

PowerManagement powerManagement;

HardwareSerial serialGps(1); // GPS sur hardware serial 1
TinyGPSPlus gps;
Afficheur *afficheur;
Preferences configuration;
Locator locator;
Menu *leMenu;


// Déclaration du module SX1276 sur le T-Beam
SX1276 lora = new Module(18, 26, 23, 26); // NSS, DIO0, RESET, DIO1 (non utilisé)

// Objet Morse et rtty
MorseClient morse(&lora);
RTTYClient rtty(&lora);


bool lectureGPS(unsigned long ms);
void sendBeacon();
void updateScreen();
void failed(int state);

void setup() {
    int state;
    Serial.begin(115200);
    pinMode(LED, OUTPUT);
    pinMode(BP, INPUT);
    digitalWrite(LED, LOW);

    //nvs_flash_erase(); // erase the NVS partition and...
    //nvs_flash_init(); // initialize the NVS partition.

    Wire.begin(SDA, SCL);
    if (!powerManagement.begin(Wire)) {
        Serial.println("AXP192 init done!");
    } else {
        Serial.println("AXP192 init failed!");
    }
    powerManagement.activateLoRa();
    powerManagement.activateOLED();
    powerManagement.activateGPS();
    powerManagement.activateMeasurement();
    powerManagement.offLed();
    
    serialGps.begin(9600, SERIAL_8N1, 34, 12);

    //splash scrren
    afficheur = new Afficheur;
    configuration.begin("beacon", false);
    afficheur->initAffichage(configuration.getString("call").c_str(), configuration.getBool("setcw"), configuration.getBool("setrtty"));
    configuration.end();

    //conf menu
    leMenu = new Menu; // Menu de configuration
    leMenu->setup();
    delete leMenu;

    configuration.begin("beacon", false);

    //fsk rtty and cw for lora init
    Serial.println("Initialisation du module LoRa en mode FSK/OOK...");
    state = lora.beginFSK(); // ⚠️ Important : utiliser beginFSK pour CW

    if (state != RADIOLIB_ERR_NONE) {
        Serial.print("Erreur d'initialisation : ");
        Serial.println(state);
        while (true);
    }

    // Définir la puissance de sortie (valeurs typiques : 2 à 20 dBm)
    state = lora.setOutputPower((int) configuration.getChar("setpwr")); // 17 dBm recommandé pour RA-02

    //state = lora.setOutputPower(17); // 17 dBm recommandé pour RA-02

    if (state == RADIOLIB_ERR_NONE) {
        Serial.println("Puissance définie !");
    } else {
        Serial.print("Erreur setOutputPower : ");
        failed(state);
    }

    // Configuration du Morse : 15 mots par minute, 1000 Hz de tonalité
    long long freqHz = configuration.getLong("freq");
    float freqMHz = freqHz / 1000000.0f; // conversion explicite en float

    Serial.println(freqHz);
    Serial.println(freqMHz);

    state = rtty.begin(freqMHz, 170, 45, T2, 2);
    //state = rtty.begin(432.5, 170, 45, T2, 2);

    if (state == RADIOLIB_ERR_NONE) {
        Serial.println(F("RTTY success!"));
    } else {
        failed(state);
    }
    state = morse.begin(freqMHz, (int) configuration.getChar("setwpm"));
    //state = morse.begin(432.5, 15);

    if (state == RADIOLIB_ERR_NONE) {
        Serial.println(F("CW success!"));
    } else {
        failed(state);
    }
    Serial.println("Init terminée");
}

void failed(int state) {        //blink error
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true) {
        powerManagement.blinkLed4();
    }
}

void loop() {
    if (digitalRead(BP) == 0) {
        updateScreen();
        sendBeacon();
    }
    if (lectureGPS(1000)) {
        digitalWrite(LED, digitalRead(LED)^1);
        if (gps.time.isValid()) {
            if (gps.time.hour() < 10) Serial.print(F("0"));
            Serial.print(gps.time.hour());
            Serial.print(F(":"));
            if (gps.time.minute() < 10) Serial.print(F("0"));
            Serial.print(gps.time.minute());
            Serial.print(F(":"));
            if (gps.time.second() < 10) Serial.print(F("0"));
            Serial.println(gps.time.second());
            if (gps.location.isValid()) {
                Serial.println(locator.getLocator(gps.location.lat(), gps.location.lng(), 8));
                if (configuration.getBool("setcont")) {
                    updateScreen();
                    sendBeacon();
                } else {
                    if (gps.time.second() == configuration.getChar("second")) {
                        updateScreen();
                        sendBeacon();
                    }
                }
            }
        }
    }

}

void sendBeacon() {
    String cwTxt, rttyTxt;
    int nb = configuration.getChar("setrepeat");

    cwTxt = configuration.getString("comment") + " " + configuration.getString("call") + " " + locator.getLocator(gps.location.lat(), gps.location.lng(), 8);
    rttyTxt = "......." + cwTxt + "....";
    Serial.println(cwTxt);
    digitalWrite(LED, HIGH);
    powerManagement.blinkLed1();
    if (configuration.getBool("setcw")) {
        for (int n = 0; n < nb; n++) {
            morse.startSignal();
            morse.print(cwTxt);
            delay(1000);
        }
    }
    if (configuration.getBool("setrtty")) {
        for (int n = 0; n < nb; n++) {
            rtty.print("\x1B");
            rtty.print("\x1B");
            rtty.print(rttyTxt);
            rtty.standby();
            delay(1000);
        }
    }
    digitalWrite(LED, LOW);
    powerManagement.offLed();
}

/**
 * 
 * @param ms le temps de lecture en ms
 * @return  bool indique la présence de nouvelle donnée
 */
bool lectureGPS(unsigned long ms) {
    bool newData = false;
    unsigned long start = millis();
    do {
        while (serialGps.available())
            if (gps.encode(serialGps.read())) {
                newData = true;
            }
    } while (millis() - start < ms);
    return newData;
}

void updateScreen() {
    long long freqHz = configuration.getLong("freq");
    float freqMHz = freqHz / 1000000.0f; // conversion explicite en float

    afficheur->afficher(
            configuration.getString("call").c_str(), // callsign
            freqMHz, // fréquence en MHz
            configuration.getChar("setpwr"), // puissance en dBm
            configuration.getBool("setrtty"), // RTTY activé
            configuration.getBool("setcw"), // CW activé
            configuration.getChar("setwpm"), // WPM
            configuration.getChar("setrepeat"), // Repeat
            configuration.getBool("setcont"), // mode continu
            locator.getLocator(gps.location.lat(), gps.location.lng(), 8), // locator
            gps.location.lat(), // latitude
            gps.location.lng(), // longitude
            configuration.getString("comment").c_str() // commentaire
            );

}
