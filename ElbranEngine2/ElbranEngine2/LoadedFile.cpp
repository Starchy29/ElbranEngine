#include "LoadedFile.h"
#include <cassert>

void LoadedFile::Release() {
	delete[] bytes;
}

uint8_t LoadedFile::ReadByte() {
	assert(readLocation >= 0 && readLocation < fileSize - 1);
	uint8_t result = bytes[readLocation];
	readLocation++;
	return result;
}

void LoadedFile::ReadBytes(uint64_t numBytes, uint8_t* outLocation) {
	assert(readLocation >= 0 && readLocation < fileSize - numBytes);
	for(uint64_t i = 0; i < numBytes; i++) {
		outLocation[i] = bytes[readLocation + i];
	}
	readLocation += numBytes;
}

uint16_t LoadedFile::ReadUInt16() {
	assert(readLocation >= 0 && readLocation < fileSize - 2);
	uint16_t result = 0;
	if(swappedEndian) {
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
	assert(readLocation >= 0 && readLocation < fileSize - 4);
	uint32_t result = 0;
	if(swappedEndian) {
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
