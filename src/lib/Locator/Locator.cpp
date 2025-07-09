/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Locator.cpp
 * Author: ale
 * 
 * Created on 7 juillet 2025, 16:07
 */

#include "Locator.h"

Locator::Locator() {
}

Locator::Locator(const Locator& orig) {
}

Locator::~Locator() {
}


String Locator::getLocator(float lat, float lon, int nbChar) {
  if (lat < -90.0 || lat > 90.0 || lon < -180.0 || lon > 180.0) {
    return "INVALID";
  }

  lon += 180.0;
  lat += 90.0;

  char locator[9];

  // Field (A-R)
  locator[0] = 'A' + int(lon / 20);
  locator[1] = 'A' + int(lat / 10);

  // Square (0-9)
  locator[2] = '0' + int(fmod(lon, 20) / 2);
  locator[3] = '0' + int(fmod(lat, 10) / 1);

  // Subsquare (A-X)
  locator[4] = 'A' + int(fmod(lon, 2) * 12);
  locator[5] = 'A' + int(fmod(lat, 1) * 24);

  // Extended square (0-9)
  locator[6] = '0' + int(fmod(lon * 120, 10));
  locator[7] = '0' + int(fmod(lat * 240, 10));

  locator[8] = '\0';

  // Clamp nbChar to valid range
  if (nbChar < 2) nbChar = 2;
  if (nbChar > 8) nbChar = 8;
  if (nbChar % 2 != 0) nbChar--;  // Ensure even number

  return String(locator).substring(0, nbChar);
}


void Locator::convLocator(String locator) {
  locator.toUpperCase();
  if (locator.length() < 4) return;

  float lon = (locator[0] - 'A') * 20.0 - 180.0;
  float lat = (locator[1] - 'A') * 10.0 - 90.0;

  lon += (locator[2] - '0') * 2.0;
  lat += (locator[3] - '0') * 1.0;

  if (locator.length() >= 6) {
    lon += (locator[4] - 'A') * (2.0 / 24.0);
    lat += (locator[5] - 'A') * (1.0 / 24.0);
  }

  if (locator.length() >= 8) {
    lon += (locator[6] - '0') * (2.0 / 240.0);
    lat += (locator[7] - '0') * (1.0 / 240.0);
  }

  longitudeDec = lon;
  latitudeDec = lat;
}

float Locator::getLatitudeDec() {
  return latitudeDec;
}

float Locator::getLongitudeDec() {
  return longitudeDec;
}

String Locator::getLatitudeDmd() {
  char buffer[16];
  char hemi = (latitudeDec >= 0) ? 'N' : 'S';
  float absLat = abs(latitudeDec);
  int deg = int(absLat);
  float min = (absLat - deg) * 60.0;
  sprintf(buffer, "%02d%05.2f%c", deg, min, hemi);
  return String(buffer);
}

String Locator::getLongitudeDmd() {
  char buffer[16];
  char hemi = (longitudeDec >= 0) ? 'E' : 'W';
  float absLon = abs(longitudeDec);
  int deg = int(absLon);
  float min = (absLon - deg) * 60.0;
  sprintf(buffer, "%03d%05.2f%c", deg, min, hemi);
  return String(buffer);
}


