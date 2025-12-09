#include "FileIO.h"
#include "Common.h"
#include "Math.h"

bool FileIO::platformLittleEndian = true;
LoadedFile (*FileIO::LoadFile)(const char* fileName) = 0;
bool (*FileIO::SaveFile)(const char* fileName, void* bytes, uint64_t fileSize) = 0;

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
	if(littleEndian != FileIO::platformLittleEndian) {
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
	if(littleEndian != FileIO::platformLittleEndian) {
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

int32_t LoadedFile::ParseInt() {
	int32_t total = 0;
	bool isDigit = true;
	while(isDigit && readLocation < fileSize) {
		total *= 10;
		char let = ReadByte();
		switch(let) {
		case '0':
			break;
		case '1':
			total += 1;
			break;
		case '2':
			total += 2;
			break;
		case '3':
			total += 3;
			break;
		case '4':
			total += 4;
			break;
		case '5':
			total += 5;
			break;
		case '6':
			total += 6;
			break;
		case '7':
			total += 7;
			break;
		case '8':
			total += 8;
			break;
		case '9':
			total += 9;
			break;
		default:
			total /= 10;
			isDigit = false;
			readLocation--;
			break;
		}
	}

	return total;
}

float LoadedFile::ParseFloat() {
	int32_t leftSide = ParseInt();
	if(readLocation >= fileSize || bytes[readLocation] != '.') return leftSide;
	uint64_t decimalIndex = readLocation;
	readLocation++; // skip past decimal point
	int32_t rightSide = ParseInt();
	uint32_t decimalShift = Math::Pow(10,  readLocation - decimalIndex - 1);
	return (float)(leftSide * decimalShift + rightSide) / decimalShift;
}
