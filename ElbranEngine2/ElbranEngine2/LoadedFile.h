#pragma once
#include <stdint.h>

struct LoadedFile {
	uint64_t fileSize;
	uint64_t readLocation;
	uint8_t* bytes;
	bool swappedEndian;
	
	void Release();

	uint8_t ReadByte();
	uint16_t ReadUInt16();
	int16_t ReadInt16();
	uint32_t ReadUInt32();
	int32_t ReadInt32();
	float ReadF2Dot14();
};

