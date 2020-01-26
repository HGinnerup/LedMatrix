// Tcp.h

#pragma once
#include "arduino.h"
#include "Buffer.h"
#include "Matrix.h"

#include "WiFi.h"
#include "AsyncUDP.h"


template <class indexType, class dataType, indexType bufferSize>
class Wifi {
public:
	void Setup(const char* ssid, const char* password, SerialBuffer<indexType, dataType, bufferSize>* buffer);

	void UdpSetup(uint16_t port, void (*onPacket)(AsyncUDPPacket packet));
	void UdpLoop();

	void TcpSetup(uint16_t port);
	void TcpLoop(void (*onRead)(WiFiClient* tcpClient));

private:
	SerialBuffer<indexType, dataType, bufferSize>* buffer;
	AsyncUDP udpServer;
	WiFiServer tcpServer;
	WiFiClient tcpClient;
};

template <class indexType, class dataType, indexType bufferSize>
void Wifi<indexType, dataType, bufferSize>::Setup(const char* ssid, const char* password, SerialBuffer<indexType, dataType, bufferSize>* buffer) {
	WiFi.mode(WIFI_STA);
	WiFi.begin(ssid, password);
	while (WiFi.waitForConnectResult() != WL_CONNECTED) {
		Serial.println("Connection Failed! Rebooting...");
		delay(5000);
		ESP.restart();
	}
	Serial.print("IP: ");
	Serial.println(WiFi.localIP());
}

// TODO: Fix pointers for templated classes, 
template <class indexType, class dataType, indexType bufferSize>
void Wifi<indexType, dataType, bufferSize>::UdpSetup(uint16_t port, void (*onPacket)(AsyncUDPPacket packet)) {
	if (udpServer.listen(port)) {
		udpServer.onPacket(onPacket);
	}
}

// TODO: Fix pointers for templated classes, 
//template <class indexType, class dataType, indexType bufferSize>
//void Wifi<indexType, dataType, bufferSize>::UdpSetup(uint16_t port, LedMatrix<15, 15, WS2801, 2, RBG>* matrix) {
//	if (udpServer.listen(port)) {
//		//Serial.print("UDP Listening on IP: ");
//		//Serial.println(WiFi.localIP());
//		udpServer.onPacket([](AsyncUDPPacket packet) {
//
//			while (packet.length() > matrix->Buffer.SpaceAvailable()) matrix->MatrixHandle();
//
//			matrix->Buffer.PopulateBuffer(packet.data(), packet.length());
//
//			});
//	}
//}

template <class indexType, class dataType, indexType bufferSize>
void Wifi<indexType, dataType, bufferSize>::UdpLoop() {
}


template <class indexType, class dataType, indexType bufferSize>
void Wifi<indexType, dataType, bufferSize>::TcpSetup(uint16_t port) {
	tcpServer.begin(port);
//	this->tcpServer = WiFiServer(port);
}

template <class indexType, class dataType, indexType bufferSize>
void Wifi<indexType, dataType, bufferSize>::TcpLoop(void (*onRead)(WiFiClient* tcpClient)) {

	if (!this->tcpClient) {
		this->tcpClient = this->tcpServer.available();
		Serial.println("New tcpClient.");
	}

    if (this->tcpClient) {
		if (this->tcpClient.connected()) {
			if (this->tcpClient.available()) {
				onRead(&(this->tcpClient));
			}
		}
		else {
			this->tcpClient.stop();
			Serial.println("Client Disconnected.");
		}
    }
}

