/**
 * LedDriverWS2801
 * Implementierung auf der Basis von WS2801-Streifen.
 *
 * @mc       Arduino/RBBB
 * @autor    Sebastian Porombka / mail _AT_ porombka _DOT_ com
 * @version  1.0
 * @created  16.4.2016
 * @updated  16.4.2016
 *
 * Versionshistorie:
 * V 1.0:  - Erstellt.
 *
 * Verkabelung: Unten Links die erste Reihe, dann die LED unten
 * rechts, dann schlangenförmig die Reihen hoch, dann die LED 
 * rechts oben, die letzte Reihe, dann die LED links oben und 
 * dann LED linkt unten.
 *
 */
#ifndef LED_DRIVER_WS2801_H
#define LED_DRIVER_WS2801_H

#include "Arduino.h"
#include "LedDriver.h"
#include <Adafruit_WS2801.h>

class LedDriverWS2801 : public LedDriver {
public:
    LedDriverWS2801(byte dataPin, byte clockPin);

    void init();
    void test();
    void clear();

    void printSignature();

    void writeScreenBufferToMatrix(word matrix[16], boolean onChange);

    void setBrightness(byte brightnessInPercent);
    byte getBrightness();

    void setLinesToWrite(byte linesToWrite);

    void shutDown();
    void wakeUp();

    void clearData();

private:
    byte _brightnessInPercent;

    boolean _dirty;

    void _setPixel(byte x, byte y, uint32_t c);
    void _setPixel(byte num, uint32_t c);

    uint32_t _color(byte r, byte g, byte b);
    uint32_t _wheel(byte wheelPos);

    byte _brightnessScaleColor(byte colorPart);

    byte _dataPin;
    byte _clockPin;


    
    Adafruit_WS2801 *_strip;
};

#endif
