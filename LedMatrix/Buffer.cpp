// 
// 
// 

#include "Buffer.h"


//template <uint16_t BufferSize>
//void Buffer<BufferSize>::ReadNBytes(uint8_t* buffer, uint16_t count)
//{
//	for (uint32_t i = 0; i < count; i++) {
//		while (Serial.available() == 0);
//		buffer[i] = Serial.read();
//	}
//	Serial.write(buffer, count);
//}

/*
template <uint16_t BufferSize>
void SerialBuffer<BufferSize>::ReadNBytes(uint8_t* buffer, uint16_t count)
{
	while (count > this->bufferLength) this->PopulateBuffer();

	for (uint16_t i = 0; i < count; i++) {
		buffer[(this->bufferStart + i) % this->bufferSize];
	}

	this->bufferStart = (this->bufferStart + count) % this->bufferSize;
	this->bufferLength -= count;
}

template <uint16_t BufferSize>
uint16_t SerialBuffer<BufferSize>::PopulateBuffer()
{
	if (Serial.available() == 0) return 0;

	uint16_t bytesReadBeforeLooparound = 0;
	uint16_t bytesReadAfterLooparound  = 0;

	// Read till end of buffer, or till it reaches beginning of data
	bytesReadBeforeLooparound = Serial.readBytes(this->buffer + this->bufferStart, min(this->bufferSize - this->bufferStart, this->bufferSize - this->bufferLength));
	this->bufferLength += bytesReadBeforeLooparound;

	// Loop around and continue reading from start of buffer
	if (this->bufferLength + this->bufferStart == this->bufferSize) {
		bytesReadAfterLooparound = Serial.readBytes(this->buffer, this->bufferStart);
		this->bufferLength += bytesReadAfterLooparound;
	}

	Serial.write(buffer+this->bufferStart, bytesReadBeforeLooparound);
	Serial.write(buffer, bytesReadAfterLooparound);

	return bytesReadBeforeLooparound + bytesReadAfterLooparound;
}

*/

