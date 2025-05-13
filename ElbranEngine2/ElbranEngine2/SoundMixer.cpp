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
		faders[i]->mixVolume += fadeData[i].fadeRate * deltaTime;
		if(fadeData[i].fadeRate > 0.f && faders[i]->mixVolume > fadeData[i].targetVolume ||
			fadeData[i].fadeRate < 0.f && faders[i]->mixVolume < fadeData[i].targetVolume
		) {
			SetTotalVolume(faders[i], faders[i]->baseVolume * fadeData[i].targetVolume);
			if(fadeData[i].pauseAtEnd) {
				StopTrack(faders[i]);
			}
			faders.RemoveAt(i);
			fadeData.RemoveAt(i);
			i--;
		} else {
			SetTotalVolume(faders[i], faders[i]->baseVolume * faders[i]->mixVolume);
		}
	}
}

void SoundMixer::PlayTrack(AudioTrack* track, bool restart, float fadeInTime) {
	CancelFader(track);

	if(restart) {
		RestartTrack(track);
	} else {
		ResumeTrack(track);
	}

	if(fadeInTime > 0.f) {
		AddFader(track, 1.0f, fadeInTime, false);
	} else {
		track->mixVolume = 1.0f;
		SetTotalVolume(track, track->baseVolume);
	}
}

void SoundMixer::PauseTrack(AudioTrack* track, float fadeOutTime) {
	CancelFader(track);

	if(fadeOutTime > 0.f) {
		AddFader(track, 0.f, fadeOutTime, true);
	} else {
		track->mixVolume = 0.f;
		StopTrack(track);
	}
}

void SoundMixer::SetTrackVolume(AudioTrack* track, float volume, float fadeDuration) {
	CancelFader(track);
	if(volume == track->mixVolume) return;

	if(fadeDuration > 0.f) {
		AddFader(track, volume, fadeDuration, false);
	} else {
		track->mixVolume = volume;
		SetTotalVolume(track, track->baseVolume * track->mixVolume);
	}
}

void SoundMixer::CancelFader(AudioTrack* track) {
	int numFaders = faders.GetSize();
	for(int i = 0; i < numFaders; i++) {
		if(faders[i] == track) {
			faders.RemoveAt(i);
			fadeData.RemoveAt(i);
			break;
		}
	}
}

void SoundMixer::AddFader(AudioTrack* track, float targetVolume, float duration, bool pauseAtEnd) {
	faders.Add(track);
	FadeData parameters;
	parameters.targetVolume = targetVolume;
	parameters.fadeRate = (targetVolume - track->mixVolume) / duration;
	parameters.pauseAtEnd = pauseAtEnd;
	fadeData.Add(parameters);
}