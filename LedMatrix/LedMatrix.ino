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

CRGB leds[NUM_LEDS];

enum Actions {
	Invalid     = 0xFF,
	Render      = 0xF0,
	BitMap      = 0xF1,
	ClearBuffer = 0xF2
};

SerialBuffer<1024> Buffer;

void setPixelColor(uint8_t x, uint8_t y, CRGB color) {
	if (x < NUM_LEDS_X && y < NUM_LEDS_Y) {
		leds[y*NUM_LEDS_X+x] = color;
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
	//FastLED.addLeds<WS2812B, 3, GRB>(leds, NUM_LEDS);
	FastLED.addLeds<WS2812B, 2, GRB>(leds, NUM_LEDS);
	//Serial.begin(115200, SERIAL_8E1);
	Serial.begin(1000000, SERIAL_8E1);
}

bool lightOn = false;

void loop() {
	lightOn = !lightOn;
	digitalWrite(LED_BUILTIN, lightOn);

	uint8_t data;
	Buffer.ReadNBytes(&data, 1);

	// Positional data beyond the bounds of NUM_LEDS_X and NUM_LEDS_Y can be used for special cases.
	switch (data) {
		case (uint8_t)Actions::Invalid:
		break;
		case (uint8_t)Actions::Render:
			FastLED.show();
		break;
		case (uint8_t)Actions::ClearBuffer:
			Serial.flush();
		break;
		case (uint8_t)Actions::BitMap:
			while (Serial.available() <= 0);
			renderIncomingBitmap();
		break;
		default: // Render pixel
			uint8_t x = (data & 0xF0) >> 4;
			uint8_t y = (data & 0x0F);
			uint8_t color[3];
			Buffer.ReadNBytes(color, 3);
			setPixelColor(x, y, CRGB(color[0], color[1], color[2]));
		break;
	}
}


