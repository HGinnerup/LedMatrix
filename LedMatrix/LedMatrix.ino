/*
 Name:		LedMatrix.ino
 Created:	1/20/2020 5:29:08 PM
 Author:	Henrik

 $85$FF$00$00
*/

#define NUM_LEDS_X 15
#define NUM_LEDS_Y 15

#define DATA_PIN 2

#define WIFI_SSID     "SSID"
#define WIFI_PASSWORD "PASSWORD"



#include "LedMatrix.h"

//#define NUM_LEDS (NUM_LEDS_X * NUM_LEDS_Y)
//
//CRGB leds[NUM_LEDS];

AsyncUDP udp;

//SerialBuffer<uint16_t, uint8_t, 1024> Buffer;

// TODO: Fix chipset
LedMatrix<NUM_LEDS_X, NUM_LEDS_Y, ESPIChipsets::APA102, DATA_PIN, GRB> matrix;
//CRGB leds[NUM_LEDS_X * NUM_LEDS_Y];

void setup() {

	//FastLED.addLeds<WS2812B, 3, GRB>(leds, NUM_LEDS);
	FastLED.addLeds<WS2812B, DATA_PIN, GRB>(matrix.leds, NUM_LEDS_X * NUM_LEDS_Y);
	//FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS_X * NUM_LEDS_Y);
	//Serial.begin(115200, SERIAL_8E1);
	Serial.begin(1000000, SERIAL_8E1);

	OtaSetup(WIFI_SSID, WIFI_PASSWORD);
	pinMode(LED_BUILTIN, OUTPUT);

    if (udp.listen(1234)) {
        Serial.print("UDP Listening on IP: ");
        Serial.println(WiFi.localIP());
        udp.onPacket([](AsyncUDPPacket packet) {

            while (packet.length() > matrix.Buffer.SpaceAvailable()) matrix.MatrixHandle();

            matrix.Buffer.PopulateBuffer(packet.data(), packet.length());

            });
    }

}



bool lightOn = false;

void loop() {
	lightOn = !lightOn;
	digitalWrite(LED_BUILTIN, lightOn);
	OtaHandle();
		
	
	if (matrix.DataAvailable() != 0) {
		matrix.MatrixHandle();
	}

	//delay(1000);
}
