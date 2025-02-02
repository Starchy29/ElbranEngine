#include "SoundEffect.h"
#include "Application.h"
#include "SoundManager.h"

SoundEffect::SoundEffect(std::wstring fileName) {
	baseVolume = 1.0f;

	SoundManager* sounds = APP->Audio();
	format = sounds->LoadAudio(&soundBuffer, fileName);
	soundBuffer.LoopCount = XAUDIO2_NO_LOOP_REGION;

	XAUDIO2_VOICE_SENDS outputData = {};
	outputData.SendCount = 1;
	XAUDIO2_SEND_DESCRIPTOR descriptor = { 0, sounds->GetEffectsChannel() };
	outputData.pSends = &descriptor;
	IXAudio2SourceVoice* firstVoice = nullptr;
	sounds->GetAudioEngine()->CreateSourceVoice(&firstVoice, (WAVEFORMATEX*)&format, 0, 2.0f, nullptr, &outputData);
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
		XAUDIO2_VOICE_SENDS outputData = {};
		outputData.SendCount = 1;
		XAUDIO2_SEND_DESCRIPTOR descriptor = { 0, sounds->GetEffectsChannel() };
		outputData.pSends = &descriptor;
		sounds->GetAudioEngine()->CreateSourceVoice(&usedVoice, (WAVEFORMATEX*)&format, 0, 2.0f, nullptr, &outputData);
		voices.push_back(usedVoice);
		usedVoice->Start();
	}
	
	// start the sound
	usedVoice->SetVolume(volume * baseVolume);
	usedVoice->SetFrequencyRatio(pitchShift >= 0 ? 1.0f + pitchShift : 1.0f / (1.0f - pitchShift));
	usedVoice->SubmitSourceBuffer(&soundBuffer);
}

void SoundEffect::StopAllInstances() {
	for(int i = 0; i < voices.size(); i++) {
		voices[i]->Stop();
		voices[i]->FlushSourceBuffers();
	}
}