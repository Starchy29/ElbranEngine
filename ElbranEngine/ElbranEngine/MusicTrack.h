#pragma once
#include "xaudio2.h"
#include <string>

class MusicTrack
{
public:
	MusicTrack(std::wstring fileName, float baseVolume = 1.0f);
	~MusicTrack();

	void Play();
	void Pause();
	void Restart();
	void SetVolume(float volume);

	float GetCurrentVolume();

private:
	XAUDIO2_BUFFER soundBuffer;
	IXAudio2SourceVoice* voice;
	float baseVolume;
	float currentVolume;
};

