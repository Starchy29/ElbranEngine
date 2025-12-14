#pragma once
#include "AudioData.h"

#ifdef WINDOWS
class WindowsAudio;
typedef WindowsAudio PlatformAudio;
#endif

class SoundMixer {
public:
	void Initialize(PlatformAudio* platformAudio);
	void Release();

	void Update(float deltaTime);

	void SetMasterVolume(float volume);
	void SetMusicVolume(float volume);
	void SetEffectsVolume(float volume);

	void PlayEffect(const AudioSample* sfx, float volume = 1.f, float pitchShift = 0.f);
	void StartTrack(const AudioSample* track, bool looped, float volume = 1.f, float fadeInTime = 0.f);
	void SetPaused(const AudioSample* track, bool paused, float fadeDuration = 0.f);
	void ChangeTrackVolume(const AudioSample* track, float volume, float fadeDuration = 0.f);

private:
	struct TrackChannel {
		const AudioSample* track;
		float mixVolume;
		struct {
			float start;
			float end;
			float duration;
			float t;
			bool pauseAtEnd;
		} fade;
		bool nowPaused;
	} trackChannels[MAX_MUSIC_CHANNELS];

	PlatformAudio* platformAudio;
};

