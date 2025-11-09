#pragma once
#include <stdint.h>

struct LoadedFile {
	static bool platformLittleEndian;

	uint64_t fileSize;
	uint64_t readLocation;
	uint8_t* bytes;
	uint8_t bitsLeftOfLastByte; // used for files that read bits at a time
	bool littleEndian;
	
	void Release();

	uint8_t ReadByte();
	uint16_t ReadBits(uint8_t numBits);
	void ReadBytes(uint64_t numBytes, uint8_t* outLocation);
	uint16_t ReadUInt16();
	int16_t ReadInt16();
	uint32_t ReadUInt32();
	int32_t ReadInt32();
	float ReadF2Dot14();
};

