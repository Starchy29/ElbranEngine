#pragma once
#include "AppPointer.h"
#include <string>
#include "AudioData.h"

#define MAX_MUSIC_CHANNELS 3
#define MAX_SFX 20

class SoundMixer {
public:
	SoundMixer(); // delete this and 0-initalize automaatically
	virtual ~SoundMixer() {}

	void Update(float deltaTime);

	virtual void SetMasterVolume(float volume) = 0;
	virtual void SetMusicVolume(float volume) = 0;
	virtual void SetEffectsVolume(float volume) = 0;

	virtual void PlayEffect(AudioSample* sfx, float volume = 1.f, float pitchShift = 0.f) = 0;
	void StartTrack(AudioSample* track, bool looped, float volume = 1.f, float fadeInTime = 0.f);
	void SetPaused(AudioSample* track, bool paused, float fadeDuration = 0.f);
	void ChangeTrackVolume(AudioSample* track, float volume, float fadeDuration = 0.f);

protected:
	virtual void BeginTrack(AudioSample* track, int8_t trackIndex, bool looped) = 0;
	virtual void PauseTrack(int8_t trackIndex) = 0;
	virtual void ResumeTrack(int8_t trackIndex) = 0;
	virtual void EndTrack(int8_t trackIndex) = 0;
	virtual void SetTotalVolume(int8_t trackIndex, float volume) = 0;
	virtual bool IsDonePlaying(int8_t trackIndex) = 0; // for non-looped tracks

private:
	struct TrackChannel {
		AudioSample* track;
		float mixVolume;
		struct {
			float start;
			float end;
			float duration;
			float t;
			bool pauseAtEnd;
		} fade;
		bool pauseAtEnd;
		bool nowPaused;
	} trackChannels[MAX_MUSIC_CHANNELS];
};

