/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Locator.h
 * Author: ale
 *
 * Created on 7 juillet 2025, 16:07
 */

#ifndef LOCATOR_H
#define LOCATOR_H

#include <Arduino.h>

class Locator {
public:
    Locator();
    Locator(const Locator& orig);
    virtual ~Locator();    
    String getLocator(float latitude, float longitude, int nbChar = 6);
    
    void convLocator(String locator);
    float getLatitudeDec();
    float getLongitudeDec();
    String getLatitudeDmd();
    String getLongitudeDmd();

  private:
    float latitudeDec = 0.0;
    float longitudeDec = 0.0;


};

#endif /* LOCATOR_H */

