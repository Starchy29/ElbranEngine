#pragma once
#include "xaudio2.h"
#include <wrl/client.h>

class SoundManager
{
	friend class Application;
	friend class Sound;

public:

	// prevent copying
	SoundManager(const SoundManager&) = delete;
	void operator=(const SoundManager&) = delete;

private:
	Microsoft::WRL::ComPtr<IXAudio2> engine;
	IXAudio2MasteringVoice* masteringVoice;

	SoundManager(HRESULT* outResult);
	~SoundManager();
};

