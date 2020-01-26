/*
 Name:		LedMatrix.ino
 Created:	1/20/2020 5:29:08 PM
 Author:	Henrik

 $85$FF$00$00
*/

#include "WiFi.h"
#include "WiFiClient.h"

constexpr auto NUM_LEDS_X = 15;
constexpr auto NUM_LEDS_Y = 15;

constexpr auto DATA_PIN = 2;

constexpr auto WIFI_SSID     = "ssid";
constexpr auto WIFI_PASSWORD = "password";

#include "LedMatrix.h"


Wifi<uint16_t, uint8_t, 1024> wifi;

// TODO: Fix chipset
LedMatrix<NUM_LEDS_X, NUM_LEDS_Y, ESPIChipsets::APA102, DATA_PIN, GRB> matrix;

void setup() {
	FastLED.addLeds<WS2812B, DATA_PIN, GRB>(matrix.leds, NUM_LEDS_X * NUM_LEDS_Y);
	Serial.begin(1000000, SERIAL_8E1);
	Serial.println("Booting");

	wifi.Setup(WIFI_SSID, WIFI_PASSWORD, &matrix.Buffer);
	wifi.UdpSetup(1234, [](AsyncUDPPacket packet) {
		while (packet.length() > matrix.Buffer.SpaceAvailable()) matrix.MatrixHandle();
		matrix.Buffer.PopulateBuffer(packet.data(), packet.length());
	});


	wifi.TcpSetup(4321);

	OtaSetup(WIFI_SSID, WIFI_PASSWORD);
	pinMode(LED_BUILTIN, OUTPUT);
	TaskHandle_t core0Loop;
	TaskHandle_t core1Loop;

	xTaskCreatePinnedToCore(core0, "Core0", 10000, NULL, 1, &core0Loop, 0);
	xTaskCreatePinnedToCore(core1, "Core1", 10000, NULL, 1, &core0Loop, 1);
}


void core0(void* params) {
	while (true) {
		digitalWrite(LED_BUILTIN, false);
		
		//if (matrix.DataAvailable() != 0) {
			matrix.MatrixHandle();
		//}

		digitalWrite(LED_BUILTIN, true);
	}

}
void core1(void* params) {
	while (true) {
		OtaHandle();
		wifi.TcpLoop(&tcpLoop);
		wifi.UdpLoop();
	}
}

void tcpLoop(WiFiClient* client) {
	while (client->available()) {

		uint8_t netBuffer[1024];
		uint16_t readLength = (uint16_t)min(client->available(), 1024);

		while (matrix.Buffer.SpaceAvailable() < readLength);

		client->readBytes(netBuffer, readLength);
		matrix.Buffer.PopulateBuffer(netBuffer, readLength);
	}
}

void loop() {

}