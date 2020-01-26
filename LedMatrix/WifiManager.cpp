#include "WifiManager.h"

void WifiManager::Setup(const char* ssid, const char* password) {
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


void WifiManager::UdpSetup(uint16_t port, void (*onPacket)(AsyncUDPPacket packet)) {
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

void WifiManager::UdpLoop() {
}


void WifiManager::TcpSetup(uint16_t port) {
	tcpServer.begin(port);
}

void WifiManager::TcpLoop(void (*onRead)(WiFiClient* tcpClient)) {

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