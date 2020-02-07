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


//void WifiManager::TcpSetup(uint16_t port) {
//	tcpServer.begin(port);
//	tcpServer.setTimeout(600);
//}

//void WifiManager::TcpSetup(uint16_t port, uint16_t maxPacketSize, void (*handler)(struct pbuf* packet)) {
//	AsyncServer* tcpServerAsync = new AsyncServer(port);
//
//	tcpServerAsync->onClient([handler, maxPacketSize](void* _, AsyncClient* clientSetup) {
//		Serial.write("onClient\n\n");
//		clientSetup->onPacket([handler](void* a, AsyncClient* client, struct pbuf* packet) {
//			Serial.write("onPacket\n\n");
//			(*handler)(packet);
//			}, &clientSetup);
//		clientSetup->onConnect([](void* a, AsyncClient* client) { Serial.write("onConnect\n\n");    }, &clientSetup);
//		clientSetup->onDisconnect([](void* a, AsyncClient* client) { Serial.write("onDisconnect\n\n"); }, &clientSetup);
//		clientSetup->onData([](void* a, AsyncClient* client, void* data, size_t len) {
//
//			Serial.printf("\n data received from client %s \n", client->remoteIP().toString().c_str());
//			Serial.write((char*)data, len);
//
//			});
//		}, tcpServerAsync);
//
//
//	tcpServerAsync->begin();
//}

void WifiManager::TcpSetup(uint16_t port, void (*handler)(struct pbuf* packet)) {
#include <HardwareSerial.h>
	AsyncServer* tcpServerAsync = new AsyncServer(port);
	auto tcpIsBeingProcessedPtr = &tcpIsBeingProcessed;

	tcpServerAsync->onClient([handler, tcpIsBeingProcessedPtr](void* _, AsyncClient* clientSetup) {
		Serial.println("onClient");
		clientSetup->onPacket(    [handler, clientSetup, tcpIsBeingProcessedPtr](void* a, AsyncClient* client, struct pbuf* packet) {
			Serial.println("onPacket");
			Serial.printf("PacketPtr: %d, PayloadPtr: %d, PayloadLen: %d\r\n", packet, packet->payload, packet->len);

			if (!(*tcpIsBeingProcessedPtr)) {
				*tcpIsBeingProcessedPtr = true;
				while (packet != NULL) {
					handler(packet);
					Serial.println("Finished processing");

					clientSetup->ackPacket(packet);
					Serial.println("AckPacketSent");
					packet = packet->next;
				}
				*tcpIsBeingProcessedPtr = false;
			}
			else {
				Serial.println("Queued processing of packet");
			}

		}, &clientSetup);
		clientSetup->onConnect(   [](void* a, AsyncClient* client)                      { Serial.println("onConnect");    }, &clientSetup);
		clientSetup->onDisconnect([](void* a, AsyncClient* client)                      { Serial.println("onDisconnect"); }, &clientSetup);
		clientSetup->onError([](void* arg, AsyncClient* client, int8_t error) { Serial.printf("\n connection error %s from client %s \n", client->errorToString(error), client->remoteIP().toString().c_str()); }, &clientSetup);
		clientSetup->onTimeout([](void* arg, AsyncClient* client, uint32_t time) { Serial.printf("\n client ACK timeout ip: %s \n", client->remoteIP().toString().c_str()); }, &clientSetup);
		clientSetup->onData([](void* a, AsyncClient* client, void* data, size_t len) {
			Serial.println("onData");
			Serial.println((char*)data);
		});
		}, tcpServerAsync);
	tcpServerAsync->begin();
}

//void WifiManager::TcpLoop(void (*onRead)(WiFiClient* tcpClient)) {
//
//	//if (!this->tcpClient) {
//	//	this->tcpClient = this->tcpServer.available();
//	//	Serial.println("New tcpClient.");
//	//}
//
//	//if (this->tcpClient) {
//	//	if (this->tcpClient.connected()) {
//	//		if (this->tcpClient.available()) {
//	//			onRead(&(this->tcpClient));
//	//		}
//	//	}
//	//	else {
//	//		this->tcpClient.stop();
//	//		Serial.println("Client Disconnected.");
//	//	}
//	//}
//}