#include "Application.h"
#include "LoadedFile.h"
#include "Common.h"

void LoadedFile::Release() {
	delete[] bytes;
}

uint8_t LoadedFile::ReadByte() {
	ASSERT(readLocation >= 0 && readLocation <= fileSize - 1);
	bitsLeftOfLastByte = 0;
	uint8_t result = bytes[readLocation];
	readLocation++;
	return result;
}

uint16_t LoadedFile::ReadBits(uint8_t numBits) {
	ASSERT(numBits <= 16);
	
	uint16_t result = 0;
	for(uint8_t i = 0; i < numBits; i++) {
		if(bitsLeftOfLastByte == 0) {
			readLocation++;
			bitsLeftOfLastByte = 8;
		}
		uint8_t nextBit = (bytes[readLocation-1] >> (8 - bitsLeftOfLastByte)) & 0b00000001;
		result = result & ((uint16_t)nextBit << i);
	}

	return result;
}

void LoadedFile::ReadBytes(uint64_t numBytes, uint8_t* outLocation) {
	ASSERT(readLocation >= 0 && readLocation <= fileSize - numBytes);
	bitsLeftOfLastByte = 0;
	for(uint64_t i = 0; i < numBytes; i++) {
		outLocation[i] = bytes[readLocation + i];
	}
	readLocation += numBytes;
}

uint16_t LoadedFile::ReadUInt16() {
	ASSERT(readLocation >= 0 && readLocation <= fileSize - 2);
	bitsLeftOfLastByte = 0;
	uint16_t result = 0;
	if(littleEndian != app->littleEndian) {
		result = bytes[readLocation];
		result = result << 8;
		result += bytes[readLocation+1];
	} else {
		result = bytes[readLocation+1];
		result = result << 8;
		result += bytes[readLocation];
	}
	readLocation += 2;
	return result;
}

int16_t LoadedFile::ReadInt16() {
	return (int16_t)ReadUInt16();
}

uint32_t LoadedFile::ReadUInt32() {
	ASSERT(readLocation >= 0 && readLocation <= fileSize - 4);
	bitsLeftOfLastByte = 0;
	uint32_t result = 0;
	if(littleEndian != app->littleEndian) {
		result = bytes[readLocation];
		result = result << 8;
		result += bytes[readLocation+1];
		result = result << 8;
		result += bytes[readLocation+2];
		result = result << 8;
		result += bytes[readLocation+3];
	} else {
		result = bytes[readLocation+3];
		result = result << 8;
		result += bytes[readLocation+2];
		result = result << 8;
		result += bytes[readLocation+1];
		result = result << 8;
		result += bytes[readLocation];
	}
	readLocation += 4;
	return result;
}

int32_t LoadedFile::ReadInt32() {
	return (int32_t)ReadUInt32();
}

float LoadedFile::ReadF2Dot14() {
	return ReadInt16() / (float)(1 << 14);
}
