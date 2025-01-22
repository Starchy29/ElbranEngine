#pragma once
#include <xaudio2.h>
#include <string>
#include <vector>

#define XAUDIO2_HELPER_FUNCTIONS

class SoundEffect
{
public:
	float baseVolume;

	SoundEffect(std::wstring fileName);
	~SoundEffect();

	void Play(float volume = 1.f, float pitchShift = 0.f);
	void StopAllInstances();

private:
	XAUDIO2_BUFFER soundBuffer;
	WAVEFORMATEXTENSIBLE format;
	std::vector<IXAudio2SourceVoice*> voices;
};

