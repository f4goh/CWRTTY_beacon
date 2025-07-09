/* 
 * File:   Menu.h
 * Author: philippe SIMIER Touchard Washington Le Mans
 *
 * Created on 22 avril 2022, 17:11
 */

#ifndef MENU_H
#define MENU_H
#include <Arduino.h>
#include <console.h>
#include <Preferences.h>        // Classe pour écrire et lire la configuration

#define FREQUENCY 432500000
#define TIMEOUT_MENU 10

class Menu {
    
public:
    Menu();
    Menu(const Menu& orig);
    virtual ~Menu();
    
    void run();
    void setup();    

 private:
    // Méthodes associées aux commandes
    static void _call_(ArgList& L, Stream& S);
    static void _second_(ArgList& L, Stream& S);
    static void _freq_(ArgList& L, Stream& S);
    static void _symbol_(ArgList& L, Stream& S);
    static void _comment_(ArgList& L, Stream& S);
    static void _setCw_(ArgList& L, Stream& S);    
    static void _setRtty_(ArgList& L, Stream& S);
    static void _setRepeat_(ArgList& L, Stream& S);    
    static void _config_(ArgList& L, Stream& S);
    static void _raz_(ArgList& L, Stream& S);
    static void _exit_(ArgList& L, Stream& S);
    static void _help_(ArgList& L, Stream& S);
    static void _unknown(String& L, Stream& S);    
    static void _setPwr_(ArgList& L, Stream& S);
    static void _setWpm_(ArgList& L, Stream& S);
    static void _setCont_(ArgList& L, Stream& S);

    bool exitFlag;
    Console *con;
    Preferences *configuration;
    static Menu* anchor;    
    bool acceptCmd(String cmd, int longMin, int longMax);    
};

#endif /* MENU_H */
