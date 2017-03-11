
//
// pumpkinbot 2014
//
// hack o'lantern 2014
//
// author: @jefforulez
//

#ifndef rulez_pumpkinbot2014_h
#define rulez_pumpkinbot2014_h

#include <Arduino.h>

#include <SoftwareSerial.h>
#include <SPI.h>

#include "Emic2TtsModule.h" 

// tts
void sayGiveMeCandy() ;

// rfid
void blockWhileRFIDPresent() ;

// leds
void ledsOn() ;
void ledsOff() ;

#endif
