/*
 Name:		LedMatrix.ino
 Created:	1/20/2020 5:29:08 PM
 Author:	Henrik

 $85$FF$00$00
*/

#include "Buffer.h"
#include <FastLED.h>

#define NUM_LEDS_X 15
#define NUM_LEDS_Y 15

#define NUM_LEDS (NUM_LEDS_X * NUM_LEDS_Y)

CRGB leds[NUM_LEDS_Y][NUM_LEDS_X];

enum Actions {
	Invalid     = 0xFF,
	Render      = 0xF0,
	BitMap      = 0xF1,
	ClearBuffer = 0xF2
};

SerialBuffer<1024> Buffer;

void setPixelColor(uint8_t x, uint8_t y, CRGB color) {
	if (x < NUM_LEDS_X && y < NUM_LEDS_Y) {
		leds[y][x] = color;
	}
}

void renderIncomingBitmap() {
	uint8_t buffer[4] = {0};
	// http://www.ue.eti.pg.gda.pl/fpgalab/zadania.spartan3/zad_vga_struktura_pliku_bmp_en.html

	// 2 bytes Signature
	Buffer.ReadNBytes(buffer, 2);
	if (buffer[0] != 'B' || buffer[1] != 'M') {
		Serial.println("Invalid Bitmap");
		Serial.flush();
		return;
	}

	// 4 bytes FileSize
	uint32_t fileSize = 0;
	Buffer.ReadNBytes((uint8_t*)&fileSize, 4);

	
	// 4 bytes Reserved
	Buffer.ReadNBytes(buffer, 4);

	// 4 bytes DataOffset
	uint32_t dataOffset = 0;
	Buffer.ReadNBytes((uint8_t*)&dataOffset, 4);

	// 4 bytes Size
	Buffer.ReadNBytes(buffer, 4);

	// 4 bytes Width
	uint32_t width = 0;
	Buffer.ReadNBytes((uint8_t*)&width, 4);

	// 4 bytes Height
	uint32_t height = 0;
	Buffer.ReadNBytes((uint8_t*)&height, 4);

	// 2 bytes Planes
	Buffer.ReadNBytes(buffer, 2);

	// 2 bytes BitCount
	uint16_t bitCount = 0;
	Buffer.ReadNBytes((uint8_t*)&bitCount, 2);

	// 4 bytes Compression
	Buffer.ReadNBytes(buffer, 4);

	// 4 bytes ImageSize
	Buffer.ReadNBytes(buffer, 4);

	// 4 bytes XpixelsPerM
	Buffer.ReadNBytes(buffer, 4);

	// 4 bytes YpixelsPerM
	Buffer.ReadNBytes(buffer, 4);

	// 4 bytes ColorsUsed
	Buffer.ReadNBytes(buffer, 4);

	// 4 bytes ColorsImportant
	uint32_t numberOfImportantColors = 0;
	Buffer.ReadNBytes((uint8_t*)&numberOfImportantColors, 4);

	//ColorTable
	CRGB* colors = NULL;
	if (bitCount > 8) {
		colors = (CRGB*)malloc(numberOfImportantColors * sizeof(CRGB));
		if (colors == NULL) {
			Serial.print("Failed to allocate ");
			Serial.print(numberOfImportantColors * sizeof(CRGB));
			Serial.println(" bytes for bitmap colors");

			Serial.flush();
			return;
		}

		// 4 bytes R-G-B-Reserved
		for (uint32_t i = 0; i < numberOfImportantColors; i++) {
			Serial.readBytes(buffer, 4);
			colors[i] = CRGB(buffer[0], buffer[1], buffer[2]);
		}
	}

	//RasterData
	for (uint32_t y = 0; y < height; y++) {
		for (uint32_t x = 0; x < width; x++) {
			switch (bitCount) {
			//case 1:
			//	Serial.readBytes(buffer, 1);
			//	setPixelColor(x, y, CRGB(buffer[0], buffer[1], buffer[2]));
			//	break;
			//case 4:
			//	break;
			//case 8:
			//	break;
			//case 16:
			//	break;
			case 24:
				Buffer.ReadNBytes(buffer, 3);
				setPixelColor(x, y, CRGB(buffer[2], buffer[1], buffer[0]));
				break;
			default:
				Serial.print("Unknown pixelDepth for Bitmap: ");
				Serial.println(bitCount);
				break;
			}
		}
		Serial.readBytes(buffer, 3);
	}


	if (colors != NULL) free(colors);
	FastLED.show();
	
}


void setup() {
	FastLED.addLeds<WS2812B, 52, GRB>(leds[ 0], NUM_LEDS_X);
	FastLED.addLeds<WS2812B, 50, GRB>(leds[ 1], NUM_LEDS_X);
	FastLED.addLeds<WS2812B, 48, GRB>(leds[ 2], NUM_LEDS_X);
	FastLED.addLeds<WS2812B, 46, GRB>(leds[ 3], NUM_LEDS_X);
	FastLED.addLeds<WS2812B, 44, GRB>(leds[ 4], NUM_LEDS_X);
	FastLED.addLeds<WS2812B, 42, GRB>(leds[ 5], NUM_LEDS_X);
	FastLED.addLeds<WS2812B, 40, GRB>(leds[ 6], NUM_LEDS_X);
	FastLED.addLeds<WS2812B, 38, GRB>(leds[ 7], NUM_LEDS_X);
	FastLED.addLeds<WS2812B, 36, GRB>(leds[ 8], NUM_LEDS_X);
	FastLED.addLeds<WS2812B, 34, GRB>(leds[ 9], NUM_LEDS_X);
	FastLED.addLeds<WS2812B, 32, GRB>(leds[10], NUM_LEDS_X);
	FastLED.addLeds<WS2812B, 30, GRB>(leds[11], NUM_LEDS_X);
	FastLED.addLeds<WS2812B, 28, GRB>(leds[12], NUM_LEDS_X);
	FastLED.addLeds<WS2812B, 26, GRB>(leds[13], NUM_LEDS_X);
	FastLED.addLeds<WS2812B, 24, GRB>(leds[14], NUM_LEDS_X);

	Serial.begin(115200, SERIAL_8E1);

	//FastLED.clear();
}

//void ReadNBytes(uint8_t* buffer, uint32_t count) {
//
//}

bool updated = true;
void loop() {

	/* Update to use buffer, rather than updating only when serial available*/
	if (Serial.available() > 0) {
		uint8_t data;
		Buffer.ReadNBytes(&data, 1);

		// Positional data beyond the bounds of NUM_LEDS_X and NUM_LEDS_Y can be used for special cases.
		switch (data) {

		case Actions::Invalid:
			break;
		case Actions::Render:
			FastLED.show();
			break;
		case Actions::ClearBuffer:
			Serial.flush();
			break;
		case Actions::BitMap:
			while (Serial.available() <= 0);
			renderIncomingBitmap();
			break;

		default: // Render pixel
			uint8_t x = (data & 0xF0) >> 4;
			uint8_t y = (data & 0x0F);
			uint8_t color[3];
			Buffer.ReadNBytes(color, 3);

			setPixelColor(x, y, CRGB(color[0], color[1], color[2]));
			//setPixelColor(x, y, CRGB(Serial.read(), Serial.read(), Serial.read()));

			break;
		}
		updated = true;
	}
	else if(updated) {
		//FastLED.show();
		updated = false;
	}
}


