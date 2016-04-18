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
#include "LedDriverWS2801.h"
#include "Configuration.h"

#define DEBUG
#include "Debug.h"

#define NUM_PIXEL 114

/**
 * Initialisierung.
 *
 * @param data Pin, an dem die Data-Line haengt.
 */
LedDriverWS2801::LedDriverWS2801(byte dataPin, byte clockPin) {
    _dataPin = dataPin;
    _clockPin = clockPin;
    _strip = new Adafruit_WS2801(NUM_PIXEL, _dataPin, _clockPin);
    _strip->begin();
    setColor(250, 255, 200);
    //test();
}

/**
 * init() wird im Hauptprogramm in init() aufgerufen.
 * Hier sollten die LED-Treiber in eine definierten
 * Ausgangszustand gebracht werden.
 */
void LedDriverWS2801::init() {
    setBrightness(50);
    clearData();
    wakeUp();
}

void LedDriverWS2801::printSignature() {
    Serial.println(F("WS2801"));
}

void LedDriverWS2801::clear() {
  for (int i=0; i < _strip->numPixels(); i++) {
        _strip->setPixelColor(i, 0);
    }
}

void LedDriverWS2801::test() {
    clear();
    for (int i=0; i < _strip->numPixels(); i++) {
        _setPixel(i, _wheel(i));
        _strip->show();
        delay(20);
    }
}

/**
 * Den Bildschirm-Puffer auf die LED-Matrix schreiben.
 *
 * @param onChange: TRUE, wenn es Aenderungen in dem Bildschirm-Puffer gab,
 *                  FALSE, wenn es ein Refresh-Aufruf war.
 */
void LedDriverWS2801::writeScreenBufferToMatrix(word matrix[16], boolean onChange) {
    if (onChange || _dirty) {
        _dirty = false;
        clear();

        uint32_t color = _color(_brightnessScaleColor(getRed()), _brightnessScaleColor(getGreen()), _brightnessScaleColor(getBlue()));

        for (byte y = 0; y < 10; y++) {
            for (byte x = 5; x < 16; x++) {
                word t = 1 << x;
                if ((matrix[y] & t) == t) {
                    _setPixel(15 - x, 9 - y, color);
                }
            }
        }

        // wir muessen die Eck-LEDs umsetzten...
        if ((matrix[1] & 0b0000000000011111) == 0b0000000000011111) {
            _setPixel(110, color); // 1
        }
        if ((matrix[0] & 0b0000000000011111) == 0b0000000000011111) {
            _setPixel(111, color); // 2
        }
        if ((matrix[3] & 0b0000000000011111) == 0b0000000000011111) {
            _setPixel(112, color); // 3
        }
        if ((matrix[2] & 0b0000000000011111) == 0b0000000000011111) {
            _setPixel(113, color); // 4
        }

        _strip->show();
    }
}

/**
 * Die Helligkeit des Displays anpassen.
 *
 * @param brightnessInPercent Die Helligkeit.
 */
void LedDriverWS2801::setBrightness(byte brightnessInPercent) {
    if (brightnessInPercent != _brightnessInPercent) {
        _brightnessInPercent = brightnessInPercent;
        _dirty = true;
    }
}

/**
 * Die aktuelle Helligkeit bekommen.
 */
byte LedDriverWS2801::getBrightness() {
    return _brightnessInPercent;
}

/**
 * Anpassung der Groesse des Bildspeichers.
 *
 * @param linesToWrite Wieviel Zeilen aus dem Bildspeicher sollen
 *                     geschrieben werden?
 */
void LedDriverWS2801::setLinesToWrite(byte linesToWrite) {
}

/**
 * Das Display ausschalten.
 */
void LedDriverWS2801::shutDown() {
    clear();
    _strip->show();
}

/**
 * Das Display einschalten.
 */
void LedDriverWS2801::wakeUp() {
}

/**
 * Den Dateninhalt des LED-Treibers loeschen.
 */
void LedDriverWS2801::clearData() {
    clear();
    _strip->show();
}

/**
 * Einen X/Y-koordinierten Pixel in der Matrix setzen.
 */
void LedDriverWS2801::_setPixel(byte x, byte y, uint32_t c) {
    _setPixel(x + (y * 11), c);
}

/**
 * Einen Pixel im Streifen setzten (die Eck-LEDs sind am Ende).
 */
void LedDriverWS2801::_setPixel(byte num, uint32_t c) {

    // Gerade und Ungerade LED reihen laufen gegeneinander.
    if (num < 110) {

        byte calculatedNum = num;
        
        if ((num / 11) % 2 == 0) {
          
            if (num > 10) { // LED unten rechts
              calculatedNum = num + 1;  
            } else {
              calculatedNum = num;
            }          
            _strip->setPixelColor(calculatedNum, c);
            
        } else {

            if (num > 98) { // LED oben rechts
              calculatedNum = ((num / 11) * 11) + 12 - (num % 11);
              _strip->setPixelColor(calculatedNum, c);
            } else {
              calculatedNum = ((num / 11) * 11) + 11 - (num % 11);
              _strip->setPixelColor(calculatedNum, c);
            }
            
        }

    // Die Werte >= 110 stehen fuer die Eck-LEDs
    } else {
        switch (num) {
            case 110:
                _strip->setPixelColor(11, c);
                break;
            case 111:
                _strip->setPixelColor(100, c);
                break;
            case 112:
                _strip->setPixelColor(112, c);
                break;
            case 113:
                _strip->setPixelColor(113, c);
                break;
        }
    }
}


/**
 * Hilfsfunktion fuer das Skalieren der Farben.
 */
byte LedDriverWS2801::_brightnessScaleColor(byte colorPart) {
    return map(_brightnessInPercent, 0, 100, 0, colorPart);
}

// Create a 24 bit color value from R,G,B
uint32_t LedDriverWS2801::_color(byte r, byte g, byte b)
{
    uint32_t c;
    c = r;
    c <<= 8;
    c |= g;
    c <<= 8;
    c |= b;
    return c;
}

//Input a value 0 to 255 to get a color value.
//The colours are a transition r - g -b - back to r
uint32_t LedDriverWS2801::_wheel(byte WheelPos)
{
    if (WheelPos < 85) {
      return _color(WheelPos * 3, 255 - WheelPos * 3, 0);
    } else if (WheelPos < 170) {
      WheelPos -= 85;
      return _color(255 - WheelPos * 3, 0, WheelPos * 3);
    } else {
      WheelPos -= 170; 
      return _color(0, WheelPos * 3, 255 - WheelPos * 3);
    }
}
