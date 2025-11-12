#ifdef WINDOWS
#include "WindowsAudio.h"
#include "Common.h"

#include "AssetContainer.h"

#define DEFAULT_SAMPLE_RATE 44100

WindowsAudio::WindowsAudio() {
    XAudio2Create(engine.GetAddressOf(), 0, XAUDIO2_DEFAULT_PROCESSOR);
    engine->CreateMasteringVoice(&masterChannel);
    engine->CreateSubmixVoice(&musicChannel, 1, DEFAULT_SAMPLE_RATE);
    engine->CreateSubmixVoice(&sfxChannel, 1, DEFAULT_SAMPLE_RATE);

    WAVEFORMATEX tester;
}

AudioSample WindowsAudio::InitializeAudio(uint8_t* audioBuffer, uint32_t bufferLength, WaveFormat format) const {
    AudioSample result = {};
    result.format = format;
    result.soundBuffer.AudioBytes = bufferLength;
    result.soundBuffer.pAudioData = audioBuffer;
    result.soundBuffer.Flags = XAUDIO2_END_OF_STREAM; // tell the source voice not to expect any data after this buffer
    return result;
}

void WindowsAudio::SetMasterVolume(float volume) {
    masterChannel->SetVolume(volume);
}

void WindowsAudio::SetMusicVolume(float volume) {
    musicChannel->SetVolume(volume);
}

void WindowsAudio::SetEffectsVolume(float volume) {
    sfxChannel->SetVolume(volume);
}

AudioTrack WindowsAudio::LoadTrack(std::wstring directory, std::wstring fileName, bool looped) {
    AudioTrack track = {};
    track.audio = AssetContainer::LoadWAV(fileName);
    track.baseVolume = 1.0f;
    track.mixVolume = 0.0f;
    track.audio.soundBuffer.LoopCount = looped ? XAUDIO2_LOOP_INFINITE : XAUDIO2_NO_LOOP_REGION;

    XAUDIO2_VOICE_SENDS outputData = {};
    outputData.SendCount = 1;
    XAUDIO2_SEND_DESCRIPTOR descriptor = { 0, musicChannel };
    outputData.pSends = &descriptor;
    engine->CreateSourceVoice(&track.voice, (WAVEFORMATEX*)&track.audio.format, 0, 2.0f, nullptr, &outputData);

    track.voice->SubmitSourceBuffer(&track.audio.soundBuffer);
    track.voice->SetVolume(track.baseVolume);
    return track;
}

void WindowsAudio::ReleaseAudioTrack(AudioTrack* track) {
    track->voice->Stop();
    delete[] track->audio.soundBuffer.pAudioData;
}

SoundEffect WindowsAudio::LoadEffect(std::wstring directory, std::wstring fileName) {
    SoundEffect effect = {};
    effect.audio = AssetContainer::LoadWAV(fileName);
    effect.baseVolume = 1.0f;
    effect.audio.soundBuffer.LoopCount = XAUDIO2_NO_LOOP_REGION;
	return effect;
}

void WindowsAudio::PlayEffect(SoundEffect* sfx, float volume, float pitchShift) {
    /*IXAudio2SourceVoice* usedVoice = nullptr;

    // find a voice that isn't playing
	for(int i = 0; i < sfx->voices.size(); i++) {
		XAUDIO2_VOICE_STATE state;
        sfx->voices[i]->GetState(&state);
		if(state.BuffersQueued <= 0) {
			usedVoice = sfx->voices[i];
			break;
		}
	}

	// add a new voice if all are playing
	if(usedVoice == nullptr) {
		XAUDIO2_VOICE_SENDS outputData = {};
		outputData.SendCount = 1;
		XAUDIO2_SEND_DESCRIPTOR descriptor = { 0, sfxChannel };
		outputData.pSends = &descriptor;
		engine->CreateSourceVoice(&usedVoice, (WAVEFORMATEX*)&sfx->audio.format, 0, 2.0f, nullptr, &outputData);
        sfx->voices.push_back(usedVoice);
		usedVoice->Start();
	}
	
	// start the sound
	usedVoice->SetVolume(volume * sfx->baseVolume);
	usedVoice->SetFrequencyRatio(pitchShift >= 0 ? 1.0f + pitchShift : 1.0f / (1.0f - pitchShift));
	usedVoice->SubmitSourceBuffer(&sfx->audio.soundBuffer);*/
}

void WindowsAudio::ReleaseSoundEffect(SoundEffect* effect) {
    delete[] effect->audio.soundBuffer.pAudioData;
    //for(IXAudio2SourceVoice* voice : effect->voices) {
    //    voice->Stop();
    //}
}

void WindowsAudio::RestartTrack(AudioTrack* track) {
    track->voice->Stop();
    track->voice->FlushSourceBuffers();
    track->voice->SubmitSourceBuffer(&track->audio.soundBuffer);
    track->voice->Start();
}

void WindowsAudio::ResumeTrack(AudioTrack* track) {
    track->voice->Start();
}

void WindowsAudio::StopTrack(AudioTrack* track) {
    track->voice->Stop();
}

void WindowsAudio::SetTotalVolume(AudioTrack* track, float volume) {
    track->voice->SetVolume(volume);
}
#endif