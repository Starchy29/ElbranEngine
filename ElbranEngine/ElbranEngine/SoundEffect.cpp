#include "SoundEffect.h"
#include "Application.h"

SoundEffect::SoundEffect(std::wstring fileName) {
	baseVolume = 1.0f;

	SoundManager* sounds = APP->Audio();
	format = sounds->LoadAudio(&soundBuffer, fileName);
	soundBuffer.LoopCount = XAUDIO2_NO_LOOP_REGION;

	IXAudio2SourceVoice* firstVoice = nullptr;
	sounds->GetAudioEngine()->CreateSourceVoice(&firstVoice, (WAVEFORMATEX*)&format);
	voices.push_back(firstVoice);
	firstVoice->Start();
}

SoundEffect::~SoundEffect() {
	delete soundBuffer.pAudioData;
	for(int i = 0; i < voices.size(); i++) {
		voices[i]->Stop();
	}
}

void SoundEffect::Play(float volume, float pitchShift) {
	SoundManager* sounds = APP->Audio();

	// find a voice that isn't playing
	IXAudio2SourceVoice* usedVoice = nullptr;
	for(int i = 0; i < voices.size(); i++) {
		XAUDIO2_VOICE_STATE state;
		voices[i]->GetState(&state);
		if(state.BuffersQueued <= 0) {
			usedVoice = voices[i];
			break;
		}
	}

	// add a new voice if all are playing
	if(usedVoice == nullptr) {
		sounds->GetAudioEngine()->CreateSourceVoice(&usedVoice, (WAVEFORMATEX*)&format);
		voices.push_back(usedVoice);
		usedVoice->Start();
	}
	
	// start the sound
	usedVoice->SetVolume(volume * baseVolume * sounds->GetEffectsVolume());
	usedVoice->SetFrequencyRatio(pitchShift >= 0 ? 1.0f + pitchShift : 1.0f / (1.0f - pitchShift));
	usedVoice->SubmitSourceBuffer(&soundBuffer);
}

void SoundEffect::StopAllInstances() {
	for(int i = 0; i < voices.size(); i++) {
		voices[i]->Stop();
		voices[i]->FlushSourceBuffers();
	}
}