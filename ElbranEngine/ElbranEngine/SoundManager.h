#pragma once
#include <xaudio2.h>
#include <wrl/client.h>
#include <string>
#include <memory>
#include "MusicTrack.h"

class SoundManager
{
	friend class Application;

public:
	static WAVEFORMATEXTENSIBLE LoadAudio(XAUDIO2_BUFFER* destination, std::wstring fileName);

	void StartSong(std::shared_ptr<MusicTrack> song, float fadeOutTime = 0.0f, float fadeInTime = 0.0f);

	void SetMasterVolume(float volume);
	void SetMusicVolume(float volume);
	void SetEffectsVolume(float volume);

	std::shared_ptr<MusicTrack> GetCurrentSong() const;
	Microsoft::WRL::ComPtr<IXAudio2> GetAudioEngine() const;

	float GetMasterVolume() const;
	float GetMusicVolume() const;
	float GetEffectsVolume() const;

	// prevent copying
	SoundManager(const SoundManager&) = delete;
	void operator=(const SoundManager&) = delete;

private:
	enum class FadeState {
		None,
		Out,
		In
	};

	Microsoft::WRL::ComPtr<IXAudio2> engine;
	IXAudio2MasteringVoice* masteringVoice;

	std::shared_ptr<MusicTrack> currentSong;
	std::shared_ptr<MusicTrack> nextSong;
	FadeState state;
	float timer;
	float fadeOutTime;
	float fadeInTime;

	float masterVolume;
	float musicVolume;
	float sfxVolume;

	SoundManager(HRESULT* outResult);
	~SoundManager();

	void Update(float deltaTime);

	static HRESULT FindChunk(HANDLE file, DWORD fourcc, DWORD& chunkSize, DWORD& chunkDataPosition);
	static HRESULT ReadChunkData(HANDLE file, void* buffer, DWORD buffersize, DWORD bufferoffset);
};

