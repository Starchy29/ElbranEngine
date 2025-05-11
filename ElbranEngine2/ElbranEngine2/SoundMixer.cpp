#include "SoundMixer.h"

SoundMixer::SoundMixer() {
	faders = FixedList<AudioTrack*>(5);
	fadeData = FixedList<FadeData>(5);
}

SoundMixer::~SoundMixer() {
	delete[] faders.dataArray;
	delete[] fadeData.dataArray;
}

void SoundMixer::Update(float deltaTime) {
	for(int i = 0; i < faders.GetSize(); i++) {
		faders[i]->currentVolume += fadeData[i].fadeRate * deltaTime;
		if(fadeData[i].fadeRate > 0.f && faders[i]->currentVolume > fadeData[i].targetVolume ||
			fadeData[i].fadeRate < 0.f && faders[i]->currentVolume < fadeData[i].targetVolume
		) {
			SetTotalVolume(faders[i], faders[i]->baseVolume * fadeData[i].targetVolume);
			faders.RemoveAt(i);
			fadeData.RemoveAt(i);
			i--;
		} else {
			SetTotalVolume(faders[i], faders[i]->baseVolume * faders[i]->currentVolume);
		}
	}
}

void SoundMixer::PlayTrack(AudioTrack* track, bool restart, float fadeInTime) {
	if(restart) {
		RestartTrack(track);
	} else {
		ResumeTrack(track);
	}

	if(fadeInTime > 0.f) {
		SetTotalVolume(track, 0.f);
		// add fader
	} else {
		SetTotalVolume(track, track->baseVolume);
	}
}

void SoundMixer::PauseTrack(AudioTrack* track, float fadeOutTime) {
	if(fadeOutTime > 0.f) {
		// add fader
	} else {
		StopTrack(track);
	}
}

void SoundMixer::SetTrackVolume(AudioTrack* track, float volume, float fadeDuration) {
	if(fadeDuration > 0.f) {
		// add fader
	} else {
		SetTotalVolume(track, track->baseVolume * volume);
	}
}
