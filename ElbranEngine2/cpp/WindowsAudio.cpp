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

void WindowsAudio::SetMasterVolume(float volume) {
    masterChannel->SetVolume(volume);
}

void WindowsAudio::SetMusicVolume(float volume) {
    musicChannel->SetVolume(volume);
}

void WindowsAudio::SetEffectsVolume(float volume) {
    sfxChannel->SetVolume(volume);
}

AudioTrack WindowsAudio::CreateTrack(AudioSample audio, bool looped) {
    AudioTrack track = {};
    track.audio = audio;
    track.baseVolume = 1.0f;
    track.mixVolume = 0.0f;

    XAUDIO2_BUFFER soundBuffer = ConvertBuffer(audio);
    soundBuffer.LoopCount = looped ? XAUDIO2_LOOP_INFINITE : XAUDIO2_NO_LOOP_REGION;

    XAUDIO2_VOICE_SENDS outputData = {};
    outputData.SendCount = 1;
    XAUDIO2_SEND_DESCRIPTOR descriptor = { 0, musicChannel };
    outputData.pSends = &descriptor;
    engine->CreateSourceVoice(&track.voice, (WAVEFORMATEX*)&track.audio.format, 0, 2.0f, nullptr, &outputData);

    track.voice->SubmitSourceBuffer(&soundBuffer);
    track.voice->SetVolume(track.baseVolume);
    return track;
}

void WindowsAudio::ReleaseAudioTrack(AudioTrack* track) {
    track->voice->Stop();
    track->audio.Release();
}

SoundEffect WindowsAudio::CreateEffect(AudioSample audio) {
    SoundEffect effect = {};
    effect.audio = audio;
    effect.baseVolume = 1.0f;

    XAUDIO2_VOICE_SENDS outputData = {};
    outputData.SendCount = 1;
    XAUDIO2_SEND_DESCRIPTOR descriptor = { 0, sfxChannel };
    outputData.pSends = &descriptor;
    for(uint8_t i = 0; i < MAX_SFX_VOICES; i++) {
        engine->CreateSourceVoice(&effect.voices[i], (WAVEFORMATEX*)&audio.format, 0, 2.0f, nullptr, &outputData);
        effect.voices[i]->Start();
    }
	return effect;
}

void WindowsAudio::PlayEffect(SoundEffect* sfx, float volume, float pitchShift) {
    // find a voice that isn't playing
    int8_t unusedIndex = -1;
	for(uint8_t i = 0; i < MAX_SFX_VOICES; i++) {
		XAUDIO2_VOICE_STATE state;
        sfx->voices[i]->GetState(&state);
		if(state.BuffersQueued <= 0) {
            unusedIndex = i;
			break;
		}
	}

    if(unusedIndex < 0) {
        sfx->voices[0]->FlushSourceBuffers();
        unusedIndex = 0;
    }
	
	// start the sound
	sfx->voices[unusedIndex]->SetVolume(volume * sfx->baseVolume);
    sfx->voices[unusedIndex]->SetFrequencyRatio(pitchShift >= 0 ? 1.0f + pitchShift : 1.0f / (1.0f - pitchShift));
    XAUDIO2_BUFFER soundBuffer = ConvertBuffer(sfx->audio);
    sfx->voices[unusedIndex]->SubmitSourceBuffer(&soundBuffer);
}

void WindowsAudio::ReleaseSoundEffect(SoundEffect* effect) {
    for(uint8_t i = 0; i < MAX_SFX_VOICES; i++) {
        effect->voices[i]->Stop();
    }
    effect->audio.Release();
}

void WindowsAudio::RestartTrack(AudioTrack* track) {
    track->voice->Stop();
    track->voice->FlushSourceBuffers();
    XAUDIO2_BUFFER soundBuffer = ConvertBuffer(track->audio);
    track->voice->SubmitSourceBuffer(&soundBuffer);
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

XAUDIO2_BUFFER WindowsAudio::ConvertBuffer(AudioSample audio) {
    XAUDIO2_BUFFER result = {};
    result.AudioBytes = audio.bufferLength;
    result.pAudioData = audio.audioBuffer;
    result.Flags = XAUDIO2_END_OF_STREAM;
    return result;
}
#endif