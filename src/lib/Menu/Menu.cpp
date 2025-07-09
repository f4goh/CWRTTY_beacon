/* 
 * File:   Menu.cpp
 * Author: philippe SIMIER Touchard Washington le Mans
 * 
 * Created on 22 avril 2022, 17:11
 */

#include "Menu.h"

Menu::Menu() :
exitFlag(false),
con(new Console()),
configuration(new Preferences()) {
    anchor = this;
}

Menu::Menu(const Menu& orig) {
}

Menu::~Menu() {

    delete configuration;
    anchor = NULL;
}

void Menu::run() {

    char car;
    int cpt = 0;
    exitFlag = false;
    Serial.println(F("Press m key to enter menu"));
    while ((cpt < TIMEOUT_MENU) && !exitFlag) {

        Serial.print(".");
        cpt++;

        if (Serial.available() > 0) {
            car = Serial.read();
            if (car == 'm') {
                Serial.println(F("help command for info"));
                while (exitFlag == false) {
                    con->run();
                }
            }
        }
        delay(1000);
    }
    Serial.println(F("\nConsole exit"));
}

void Menu::setup() {

    con->onCmd("help", _help_);
    con->onCmd("call", _call_);
    con->onCmd("second", _second_);
    con->onCmd("freq", _freq_);
    con->onCmd("comment", _comment_);
    con->onCmd("setrtty", _setRtty_);
    con->onCmd("setcw", _setCw_);
    con->onCmd("setwpm", _setWpm_);
    con->onCmd("setrepeat", _setRepeat_);
    con->onCmd("setpwr", _setPwr_);
    con->onCmd("setcont", _setCont_);
    con->onCmd("show", _config_);
    con->onCmd("raz", _raz_);
    con->onCmd("exit", _exit_);

    con->onUnknown(_unknown);
    con->start();

    anchor->configuration->begin("beacon", false);

    if (anchor->configuration->getBool("config", false) == false) {
        anchor->configuration->putString("call", "NOCALL");
        anchor->configuration->putLong("freq", FREQUENCY);
        anchor->configuration->putChar("second", 10);
        anchor->configuration->putString("comment", "BEACON TEST DE");
        anchor->configuration->putBool("setrtty", true);
        anchor->configuration->putBool("setcw", true);        
        anchor->configuration->putChar("setrepeat", 2);
        anchor->configuration->putChar("setwpm", 15);
        anchor->configuration->putChar("setpwr", 17);
        anchor->configuration->putBool("setcont", 1);
        anchor->configuration->putBool("config", true);
    }
    anchor->configuration->end();
    this->run();
}

void Menu::_exit_(ArgList& L, Stream& S) {
    anchor->exitFlag = true;

}

void Menu::_help_(ArgList& L, Stream& S) {
    S.println(F("Available commands"));
    S.println(F("Set new callsign                   : call F4GOH"));
    S.println(F("Set frequency                      : freq 432500000"));
    S.println(F("Set power dBm                      : setpwr 17"));
    S.println(F("Set second txing into the minute   : second 20"));
    S.println(F("Set new comment                    : comment HELLO"));
    S.println(F("Set Rtty enable (0 or 1)           : setrtty 1"));
    S.println(F("Set CW enable (0 or 1)             : setcw 1"));
    S.println(F("Set Wpm (5..25)                    : setwpm 15"));
    S.println(F("Set Repeat (1..3) nb repeat        : setrepeat 1"));
    S.println(F("Set Continous  mode                : setcont 1"));
    S.println(F("Show configuration                 : show"));
    S.println(F("Reset default configuration        : raz"));
    S.println(F("Exit menu                          : exit"));
}

bool Menu::acceptCmd(String cmd, int longMin, int longMax) {

    if (cmd.length() >= longMin && cmd.length() <= longMax) {
        return true;
    } else {
        Serial.println("Erreur");
        return false;
    }
}

void Menu::_unknown(String& L, Stream& S) {
    S.print(L);
    S.println(" : command not found");
}

void Menu::_call_(ArgList& L, Stream& S) {
    String p;
    bool ret;
    p = L.getNextArg();
    p.toUpperCase();
    ret = anchor->acceptCmd(p, 3, 10);
    if (ret == true) {
        anchor->configuration->begin("beacon", false);
        S.printf("call is %s\n\r", p.c_str());
        anchor->configuration->putString("call", p);
        anchor->configuration->end();

    }
}

void Menu::_second_(ArgList& L, Stream& S) {
    String p;
    bool ret;
    p = L.getNextArg();
    ret = anchor->acceptCmd(p, 2, 2);
    if (ret == true) {
        anchor->configuration->begin("beacon", false);
        S.printf("seconds is %s\n\r", p.c_str());
        anchor->configuration->putChar("second", (int8_t) p.toInt());
        anchor->configuration->end();
    }
}

void Menu::_freq_(ArgList& L, Stream& S) {
    String p;
    bool ret;
    p = L.getNextArg();
    ret = anchor->acceptCmd(p, 9, 9);
    if (ret == true) {
        anchor->configuration->begin("beacon", false);
        S.printf("freq is %s\n\r", p.c_str());
        anchor->configuration->putLong("freq", p.toInt());
        anchor->configuration->end();
    }
}

void Menu::_comment_(ArgList& L, Stream& S) {
    String arg;
    String p = "";

    // Concatène tous les arguments avec un espace entre eux
    while (!(arg = L.getNextArg()).isEmpty()) {
        if (!p.isEmpty()) {
            p += " ";
        }
        p += arg;
    }
    p.toUpperCase();  // Convertit en majuscules
    // Envoie la commande à l'ancre
    bool ret = anchor->acceptCmd(p, 3, 50);
    if (ret) {
        anchor->configuration->begin("beacon", false);
        S.printf("comment is \"%s\"\n\r", p.c_str());
        anchor->configuration->putString("comment", p);
        anchor->configuration->end();
    } else {
        S.println("Commande rejetée.");
    }
}
/*
void Menu::_comment_(ArgList& L, Stream& S) {
    String arg;
    bool ret;
    String p = "";
    while (!(arg = L.getNextArg()).isEmpty()) {
        p = p + arg + " ";
    }
    p.toUpperCase();
    ret = anchor->acceptCmd(p, 3, 50);
    if (ret == true) {
        anchor->configuration->begin("beacon", false);
        S.printf("comment is %s\n\r", p.c_str());
        anchor->configuration->putString("comment", p);
        anchor->configuration->end();
    }
}
*/
void Menu::_setRtty_(ArgList& L, Stream& S) {
    String p;
    bool ret;
    p = L.getNextArg();
    ret = anchor->acceptCmd(p, 1, 1);
    if (ret == true) {
        anchor->configuration->begin("beacon", false);
        anchor->configuration->putBool("setrtty", (int8_t) p.toInt());
        Serial.printf("Rtty is : %s\n\r", anchor->configuration->getBool("setrtty") ? "Enable" : "Disable");
        anchor->configuration->end();
    }
}

void Menu::_setCw_(ArgList& L, Stream& S) {
    String p;
    bool ret;
    p = L.getNextArg();
    ret = anchor->acceptCmd(p, 1, 1);
    if (ret == true) {
        anchor->configuration->begin("beacon", false);
        anchor->configuration->putBool("setcw", (int8_t) p.toInt());
        Serial.printf("Cw is : %s\n\r", anchor->configuration->getBool("setcw") ? "Enable" : "Disable");
        anchor->configuration->end();
    }

}

void Menu::_setCont_(ArgList& L, Stream& S) {
    String p;
    bool ret;
    p = L.getNextArg();
    ret = anchor->acceptCmd(p, 1, 1);
    if (ret == true) {
        anchor->configuration->begin("beacon", false);
        anchor->configuration->putBool("setcont", (int8_t) p.toInt());
        Serial.printf("Continous mode is : %s\n\r", anchor->configuration->getBool("setcont") ? "Enable" : "Disable");
        anchor->configuration->end();
    }

}

void Menu::_setRepeat_(ArgList& L, Stream& S) {
    String p;
    bool ret;
    p = L.getNextArg();
    ret = anchor->acceptCmd(p, 1, 10);
    if (ret == true) {
        anchor->configuration->begin("beacon", false);
        anchor->configuration->putChar("setrepeat", (int8_t) p.toInt());
        Serial.printf("Repeat is : %d\n\r", anchor->configuration->getChar("setrepeat"));
        anchor->configuration->end();
    }
}

void Menu::_setPwr_(ArgList& L, Stream& S) {
    String p;
    bool ret;
    p = L.getNextArg();
    ret = anchor->acceptCmd(p, 1, 2);
    if (ret == true) {
        if ((int8_t) p.toInt() <= 20 && (int8_t) p.toInt()>=2) {
            anchor->configuration->begin("beacon", false);
            anchor->configuration->putChar("setpwr", (int8_t) p.toInt());
            Serial.printf("Power is : %d dBm\n\r", anchor->configuration->getChar("setpwr"));
            anchor->configuration->end();
        } else {
            Serial.printf("Power must be 2..20 dBm");
        }
    }
}

void Menu::_setWpm_(ArgList& L, Stream& S) {
    String p;
    bool ret;
    p = L.getNextArg();
    ret = anchor->acceptCmd(p, 1, 2);
    if (ret == true) {
        if ((int8_t) p.toInt() <= 25 && (int8_t) p.toInt()>=5) {
            anchor->configuration->begin("beacon", false);
            anchor->configuration->putChar("setwpm", (int8_t) p.toInt());
            Serial.printf("WPM is : %d \n\r", anchor->configuration->getChar("setwpm"));
            anchor->configuration->end();
        } else {
            Serial.printf("WPM must be 5..25");
        }
    }
}

void Menu::_config_(ArgList& L, Stream& S) {
    anchor->configuration->begin("beacon", false);
    S.printf("Call is              : %s\n\r", anchor->configuration->getString("call").c_str());
    S.printf("Frequency is         : %d\n\r", anchor->configuration->getLong("freq"));
    S.printf("Power is             : %d dBm\n\r", anchor->configuration->getChar("setpwr"));
    S.printf("Comment is           : %s\n\r", anchor->configuration->getString("comment").c_str());
    S.printf("Transmit at second   : %d \n\r", anchor->configuration->getChar("second"));
    S.printf("Rtty is              : %s\n\r", anchor->configuration->getBool("setrtty") ? "Enable" : "Disable");
    S.printf("CW is                : %s\n\r", anchor->configuration->getBool("setcw") ? "Enable" : "Disable");
    S.printf("WPM is              : %d\n\r", anchor->configuration->getChar("setwpm"));
    S.printf("Repeat is            : %d\n\r", anchor->configuration->getChar("setrepeat"));
    S.printf("Continous mode       : %d\n\r", anchor->configuration->getBool("setcont"));
    anchor->configuration->end();
}

void Menu::_raz_(ArgList& L, Stream& S) {
    anchor->configuration->begin("beacon", false);
    anchor->configuration->putBool("config", false);
    anchor->configuration->clear(); 
    //anchor->configuration->clear();  
    ESP.restart();
}


Menu* Menu::anchor = NULL;
