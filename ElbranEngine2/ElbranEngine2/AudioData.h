#pragma once

#ifdef WINDOWS
#include <xaudio2.h>
#include <vector>

struct AudioTrack {
	float baseVolume;
	float currentVolume;
	XAUDIO2_BUFFER soundBuffer;
	IXAudio2SourceVoice* voice;
};

struct SoundEffect {
	float baseVolume;
	XAUDIO2_BUFFER soundBuffer;
	WAVEFORMATEXTENSIBLE format;
	std::vector<IXAudio2SourceVoice*> voices;
};
#endif