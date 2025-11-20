#include "SoundMixer.h"
#include "Math.h"

#ifdef WINDOWS
#include "WindowsAudio.h"
#endif

void SoundMixer::Initialize(PlatformAudio* platformAudio) {
	this->platformAudio = platformAudio;
}

void SoundMixer::Release() {
	delete platformAudio;
}

void SoundMixer::Update(float deltaTime) {
	for(uint8_t i = 0; i < MAX_MUSIC_CHANNELS; i++) {
		if(trackChannels[i].track && trackChannels[i].fade.t < 1.0f) {
			trackChannels[i].fade.t += deltaTime / trackChannels[i].fade.duration;
			if(trackChannels[i].fade.t > 1.0f) {
				trackChannels[i].fade.t = 1.0f;
				if(trackChannels[i].pauseAtEnd) { 
					platformAudio->PauseTrack(i);
					trackChannels[i].nowPaused = true;
				}
			}
			platformAudio->SetTotalVolume(i, trackChannels[i].track->baseVolume * Tween::Lerp(trackChannels[i].fade.start, trackChannels[i].fade.end, trackChannels[i].fade.t));
		}
	}
}

void SoundMixer::SetMasterVolume(float volume) {
	platformAudio->SetMasterVolume(volume);
}

void SoundMixer::SetMusicVolume(float volume) {
	platformAudio->SetMusicVolume(volume);
}

void SoundMixer::SetEffectsVolume(float volume) {
	platformAudio->SetEffectsVolume(volume);
}

void SoundMixer::PlayEffect(const AudioSample* sfx, float volume, float pitchShift) {
	platformAudio->PlayEffect(sfx, volume, pitchShift);
}

void SoundMixer::StartTrack(const AudioSample* track, bool looped, float volume, float fadeInTime) {
	// find the correct slot for this track
	int8_t trackIndex = -1;
	for(int8_t i = 0; i < MAX_MUSIC_CHANNELS; i++) {
		if(trackChannels[i].track == track) {
			// restart the track if it is currently playing
			trackIndex = i;
			break;
		}
		else if(trackChannels[i].track == nullptr || platformAudio->IsDonePlaying(i)) {
			// prefer an empty slot over a paused slot
			trackIndex = i;
		}
		else if(trackChannels[i].nowPaused && trackIndex < 0) {
			// overwrite a paused track if all are reserved
			trackIndex = i;
		}
	}

	if(trackIndex < 0) return; // fails when adding a track and all available slots are playing

	if(trackChannels[trackIndex].track != nullptr) {
		platformAudio->EndTrack(trackIndex);
	}

	platformAudio->BeginTrack(track, trackIndex, looped);
	trackChannels[trackIndex].track = track;
	trackChannels[trackIndex].pauseAtEnd = false;
	trackChannels[trackIndex].nowPaused = false;
	trackChannels[trackIndex].mixVolume = volume;
	if(fadeInTime > 0.f) {
		trackChannels[trackIndex].fade.t = 0.f;
		trackChannels[trackIndex].fade.duration = fadeInTime;
		trackChannels[trackIndex].fade.start = 0.f;
		trackChannels[trackIndex].fade.end = volume;
		platformAudio->SetTotalVolume(trackIndex, 0.f);
	} else {
		trackChannels[trackIndex].fade.t = 1.f;
		platformAudio->SetTotalVolume(trackIndex, trackChannels[trackIndex].track->baseVolume * volume);
	}
}

void SoundMixer::SetPaused(const AudioSample* track, bool paused, float fadeDuration) {
	int8_t trackIndex = -1;
	for(int8_t i = 0; i < MAX_MUSIC_CHANNELS; i++) {
		if(trackChannels[i].track == track) {
			trackIndex = i;
			break;
		}
	}
	if(trackIndex < 0 || trackChannels[trackIndex].nowPaused == paused) return;

	if(paused) {
		if(fadeDuration > 0.f) {
			trackChannels[trackIndex].fade.t = 0.0f;
			trackChannels[trackIndex].fade.start = trackChannels[trackIndex].mixVolume;
			trackChannels[trackIndex].fade.end = 0.f;
			trackChannels[trackIndex].fade.pauseAtEnd = true;
		} else {
			trackChannels[trackIndex].fade.t = 1.0f;
			trackChannels[trackIndex].nowPaused = true;
			platformAudio->PauseTrack(trackIndex);
		}
	} else {
		platformAudio->ResumeTrack(trackIndex);
		trackChannels[trackIndex].nowPaused = false;
		if(fadeDuration > 0.f) {
			trackChannels[trackIndex].fade.t = 0.0f;
			trackChannels[trackIndex].fade.start = 0.f;
			trackChannels[trackIndex].fade.end = trackChannels[trackIndex].mixVolume;
			trackChannels[trackIndex].fade.pauseAtEnd = false;
			platformAudio->SetTotalVolume(trackIndex, 0.f);
		} else {
			trackChannels[trackIndex].fade.t = 1.0f;
			platformAudio->SetTotalVolume(trackIndex, track->baseVolume * trackChannels[trackIndex].mixVolume);
		}
	}
}

void SoundMixer::ChangeTrackVolume(const AudioSample* track, float volume, float fadeDuration) {
	int8_t trackIndex = -1;
	for(int8_t i = 0; i < MAX_MUSIC_CHANNELS; i++) {
		if(trackChannels[i].track == track) {
			trackIndex = i;
			break;
		}
	}
	if(trackIndex < 0) return;

	if(fadeDuration > 0.f) {
		trackChannels[trackIndex].fade.t = 0.f;
		trackChannels[trackIndex].fade.duration = fadeDuration;
		trackChannels[trackIndex].fade.start = trackChannels[trackIndex].mixVolume;
		trackChannels[trackIndex].fade.end = volume;
	} else {
		trackChannels[trackIndex].fade.t = 1.f;
		platformAudio->SetTotalVolume(trackIndex, track->baseVolume * trackChannels[trackIndex].mixVolume);
	}

	trackChannels[trackIndex].pauseAtEnd = false;
	trackChannels[trackIndex].mixVolume = volume;
}