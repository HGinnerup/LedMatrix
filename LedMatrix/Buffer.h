// Buffer.h

#pragma once


#include "arduino.h"

//class SerialBufferBase {
//public: 
//	const indexType bufferSize;
//	bool CanReadNBytes(indexType count);
//	void ReadNBytes(dataType* buffer, indexType count);
//	void PopulateBuffer();
//	virtual indexType PopulateBuffer(dataType* bytes, indexType length);
//	virtual indexType Available();
//	indexType SpaceAvailable();
//	void Flush();
//};

template <class indexType, class dataType, indexType BufferSize>
class SerialBuffer /*: BaseSerialBuffer*/ {

private:
	dataType  buffer[BufferSize];
	indexType bufferStart  = 0;
	indexType bufferLength = 0;

public:
	const indexType bufferSize = BufferSize;
	bool CanReadNBytes(indexType count);
	void ReadNBytes(dataType* buffer, indexType count);
	indexType PopulateBuffer();
	indexType PopulateBuffer(dataType* bytes, indexType length);
	indexType Available();
	indexType SpaceAvailable();
	void Flush();
};

template <class indexType, class dataType, indexType BufferSize>
bool SerialBuffer<indexType, dataType, BufferSize>::CanReadNBytes(indexType count) {
	return count <= this->bufferLength;
}


template <class indexType, class dataType, indexType BufferSize>
void SerialBuffer<indexType, dataType, BufferSize>::ReadNBytes(dataType* buffer, indexType count) {

	//while (Available() < count);
	//Serial.readBytes(buffer, count);
	//return;


	while (!CanReadNBytes(count)) this->PopulateBuffer();

	for (uint16_t i = 0; i < count; i++) {
		buffer[i] = this->buffer[(this->bufferStart + i) % this->bufferSize];
	}

	this->bufferStart = (this->bufferStart + count) % this->bufferSize;
	this->bufferLength -= count;
}

template <class indexType, class dataType, indexType BufferSize>
indexType SerialBuffer<indexType, dataType, BufferSize>::SpaceAvailable() {
	return this->bufferSize - this->bufferLength;
}

template <class indexType, class dataType, indexType BufferSize>
indexType SerialBuffer<indexType, dataType, BufferSize>::PopulateBuffer() {
	dataType serialData[bufferSize];
	indexType readCount = min(SpaceAvailable(), (indexType)Serial.available());
	Serial.readBytes(serialData, readCount);
	return PopulateBuffer(serialData, readCount);
}

template <class indexType, class dataType, indexType BufferSize>
indexType SerialBuffer<indexType, dataType, BufferSize>::PopulateBuffer(dataType* bytes, indexType length) {

	length = min(length, SpaceAvailable());
	if (length == 0) return 0;

	for (indexType i = 0; i < length; i++) {
		buffer[(this->bufferStart + i) % this->bufferSize] = bytes[i];
	}

	this->bufferLength += length;

	return length;
}

template <class indexType, class dataType, indexType BufferSize>
indexType SerialBuffer<indexType, dataType, BufferSize>::Available() {
	return this->bufferLength + Serial.available();
}

template <class indexType, class dataType, indexType BufferSize>
void SerialBuffer<indexType, dataType, BufferSize>::Flush() {
	this->bufferLength = 0;
	Serial.flush();
}
