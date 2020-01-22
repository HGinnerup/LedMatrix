// 
// 
// 

#include "LedStrip.h"

class LedStrip {
public:
    LedStrip::LedStrip(int pin, CRGB* leds, int ledCount);

    void SetPixel(int x, CRGB color);
    int   Pin;
    CRGB* Leds;
    int   LedCount;

private:

};

LedStrip::LedStrip(int pin, CRGB* leds, int ledCount) {
    Pin      = pin;
    Leds     = leds;
    LedCount = ledCount;
};

void LedStrip::SetPixel(int x, CRGB color) {

}
