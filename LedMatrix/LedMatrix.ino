/*
 Name:		LedMatrix.ino
 Created:	1/20/2020 5:29:08 PM
 Author:	Henrik

 $85$FF$00$00
*/

#include "WiFi.h"
#include "WiFiClient.h"
#include <NetBIOS.h>
#include "src/AsyncTCP-master/src/AsyncTCP.h"

constexpr auto NUM_LEDS_X = 15;
constexpr auto NUM_LEDS_Y = 15;

constexpr auto DATA_PIN = 2;

constexpr auto WIFI_SSID     = "ssid";
constexpr auto WIFI_PASSWORD = "password";

#include "LedMatrix.h"


WifiManager wifi;

// TODO: Fix chipset
LedMatrix<NUM_LEDS_X, NUM_LEDS_Y, ESPIChipsets::APA102, DATA_PIN, GRB> matrix;

void setup() {
	FastLED.addLeds<WS2812B, DATA_PIN, GRB>(matrix.leds, NUM_LEDS_X * NUM_LEDS_Y);
	Serial.begin(115200, SERIAL_8E1);
	Serial.println("Booting");

	wifi.Setup(WIFI_SSID, WIFI_PASSWORD);
	NBNS.begin("LedMatrix");

	wifi.UdpSetup(1234, [](AsyncUDPPacket packet) {
		//while (packet.length() > matrix.Buffer.SpaceAvailable()) matrix.MatrixHandle();
		//matrix.Buffer.PopulateBuffer(packet.data(), packet.length());

		uint8_t* dataPtr = packet.data();
		uint16_t len = packet.length();
		while (len > 0) {
			Serial.printf("LenRemaining: %d\r\n", len);
			uint16_t lenToAdd = (uint16_t)min(len, matrix.Buffer.SpaceAvailable());
			matrix.Buffer.PopulateBuffer(dataPtr, lenToAdd);
			dataPtr += lenToAdd;
			len -= lenToAdd;
			while (matrix.Buffer.Available() > 4) {
				matrix.MatrixHandle();
			}
		}
	});

	wifi.TcpSetup(4321, 1024, [](pbuf* packet) {
		uint8_t* dataPtr = (uint8_t*)(packet->payload);
		auto len = packet->len;
		while (len > 0) {
			Serial.printf("LenRemaining: %d\r\n", len);
			auto lenToAdd = min(len, matrix.Buffer.SpaceAvailable());
			Serial.printf("Adding: %d bytes to buffer\r\n", lenToAdd);
			matrix.Buffer.PopulateBuffer(dataPtr, lenToAdd);
			dataPtr += lenToAdd;
			len -= lenToAdd;
			while (matrix.Buffer.Available() > 4) {
				matrix.MatrixHandle();
			}
		}
	});

	OtaSetup(WIFI_SSID, WIFI_PASSWORD);
	pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
	OtaHandle();

	digitalWrite(LED_BUILTIN, false);

	if (matrix.DataAvailable() > 4) {
		matrix.MatrixHandle();
	}

	digitalWrite(LED_BUILTIN, true);
}