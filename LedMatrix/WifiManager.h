// Tcp.h

#pragma once
#include "arduino.h"
#include "Buffer.h"
#include "Matrix.h"

#include "WiFi.h"
#include "AsyncUDP.h"
#include "src/AsyncTCP-master/src/AsyncTCP.h"

class WifiManager {
public:
	void Setup(const char* ssid, const char* password);

	void UdpSetup(uint16_t port, void (*onPacket)(AsyncUDPPacket packet));
	void UdpLoop();

	void TcpSetup(uint16_t port, void (*handler)(struct pbuf* packet));
	//void TcpLoop(void (*onRead)(WiFiClient* tcpClient));

private:
	AsyncUDP udpServer;
	WiFiServer tcpServer;
	bool tcpIsBeingProcessed = false;
	//WiFiClient tcpClient;
	
};


