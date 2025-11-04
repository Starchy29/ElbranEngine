#pragma once
#include "AppPointer.h"
#include <string>
#include "AudioData.h"
#include "FixedList.h"

struct FadeData {
	float targetVolume;
	float fadeRate;
	bool pauseAtEnd;
};

class SoundMixer {
public:
	SoundMixer() = default;
	virtual ~SoundMixer() {}

	void Update(float deltaTime);

	virtual void SetMasterVolume(float volume) = 0;
	virtual void SetMusicVolume(float volume) = 0;
	virtual void SetEffectsVolume(float volume) = 0;

	virtual AudioTrack LoadTrack(std::wstring directory, std::wstring fileName, bool looped) = 0;
	void PlayTrack(AudioTrack* track, bool restart, float fadeInTime = 0.f);
	void PauseTrack(AudioTrack* track, float fadeOutTime = 0.f);
	void SetTrackVolume(AudioTrack* track, float volume, float fadeDuration = 0.f);
	virtual void ReleaseAudioTrack(AudioTrack* track) = 0;

	virtual SoundEffect LoadEffect(std::wstring directory, std::wstring fileName) = 0;
	virtual void PlayEffect(SoundEffect* sfx, float volume = 1.f, float pitchShift = 0.f) = 0;
	virtual void ReleaseSoundEffect(SoundEffect* effect) = 0;

protected:
	virtual void RestartTrack(AudioTrack* track) = 0;
	virtual void ResumeTrack(AudioTrack* track) = 0;
	virtual void StopTrack(AudioTrack* track) = 0;
	virtual void SetTotalVolume(AudioTrack* track, float volume) = 0;

private:
	FixedList<AudioTrack*,5> faders;
	FixedList<FadeData,5> fadeData;

	void CancelFader(AudioTrack* track);
	void AddFader(AudioTrack* track, float targetVolume, float duration, bool pauseAtEnd);
};

