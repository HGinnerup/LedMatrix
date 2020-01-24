// Buffer.h

#pragma once


#include "arduino.h"


template <uint16_t BufferSize>
class SerialBuffer {

private:
	uint8_t  buffer[BufferSize];
	uint16_t bufferStart  = 0;
	uint16_t bufferLength = 0;

public:
	const uint16_t bufferSize = BufferSize;
	void ReadNBytes(uint8_t* buffer, uint16_t count);
	uint16_t PopulateBuffer();
	uint16_t Available();
	void Flush();
};


template <uint16_t BufferSize>
void SerialBuffer<BufferSize>::ReadNBytes(uint8_t* buffer, uint16_t count) {

	while (Serial.available() < count);
	Serial.readBytes(buffer, count);
	return;




	// TODO: will block if count is larger than buffersize
	while (count > this->bufferLength) this->PopulateBuffer();

	for (uint16_t i = 0; i < count; i++) {
		buffer[i] = this->buffer[(this->bufferStart + i) % this->bufferSize];
	}

	this->bufferStart = (this->bufferStart + count) % this->bufferSize;
	this->bufferLength -= count;
}

template <uint16_t BufferSize>
uint16_t SerialBuffer<BufferSize>::PopulateBuffer()
{
	if (Serial.available() == 0) return 0;

	uint16_t bytesReadBeforeLooparound = 0;
	uint16_t bytesReadAfterLooparound = 0;

	// Read till end of buffer, or till it reaches beginning of data
	bytesReadBeforeLooparound = Serial.readBytes(this->buffer + this->bufferStart, min(this->bufferSize - this->bufferStart, this->bufferSize - this->bufferLength));
	this->bufferLength += bytesReadBeforeLooparound;

	// Loop around and continue reading from start of buffer
	if (this->bufferLength + this->bufferStart == this->bufferSize) {
		bytesReadAfterLooparound = Serial.readBytes(this->buffer, this->bufferStart);
		this->bufferLength += bytesReadAfterLooparound;
	}

	Serial.write(buffer + this->bufferStart, bytesReadBeforeLooparound);
	Serial.write(buffer, bytesReadAfterLooparound);

	return bytesReadBeforeLooparound + bytesReadAfterLooparound;
}

template <uint16_t BufferSize>
uint16_t SerialBuffer<BufferSize>::Available() {
	return this->bufferLength + Serial.available();
}

template <uint16_t BufferSize>
void SerialBuffer<BufferSize>::Flush() {
	this->bufferLength = 0;
	Serial.flush();
}