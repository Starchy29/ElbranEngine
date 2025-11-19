#pragma once
#include <stdint.h>

// matches the structure of the 'fmt ' chunk in .wav files
struct WaveFormat {
	uint16_t formatTag;
	uint16_t channels;
	uint32_t samplesPerSec;
	uint32_t avgBytesPerSec;
	uint16_t blockAlign;
	uint16_t bitsPerSample;
	uint16_t extraBytes;

	// optional extensible portion
	union {
		uint16_t validBitsPerSample;
		uint16_t samplesPerBlock;
	} samples;
	uint32_t channelMask;
	uint8_t guid[16];
};

struct AudioSample {
	uint8_t* audioBuffer;
	uint32_t bufferLength;
	WaveFormat format;
	float baseVolume;

	inline void Release() { delete[] audioBuffer; }
};