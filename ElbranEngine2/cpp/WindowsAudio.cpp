#ifdef WINDOWS
#include "WindowsAudio.h"
#include "Common.h"
#include <xaudio2.h>

#define DEFAULT_SAMPLE_RATE 44100

WindowsAudio::WindowsAudio() {
    XAudio2Create(engine.GetAddressOf(), 0, XAUDIO2_DEFAULT_PROCESSOR);
    engine->CreateMasteringVoice(&masterChannel);
    engine->CreateSubmixVoice(&musicChannel, 1, DEFAULT_SAMPLE_RATE);
    engine->CreateSubmixVoice(&sfxChannel, 1, DEFAULT_SAMPLE_RATE);
    nextSFX = 0;

    for(uint8_t i = 0; i < MAX_MUSIC_CHANNELS; i++) {
        musicVoices[i] = 0; // this language is dumb
    }
    for(uint8_t i = 0; i < MAX_SFX; i++) {
        sfxVoices[i] = 0;
    }
}

WindowsAudio::~WindowsAudio() {
    for(uint8_t i = 0; i < MAX_MUSIC_CHANNELS; i++) {
        if(!musicVoices[i]) break;
        musicVoices[i]->Stop();
    }
    for(uint8_t i = 0; i < MAX_SFX; i++) {
        if(!sfxVoices[i]) break;
        sfxVoices[i]->Stop();
    }
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

void WindowsAudio::PlayEffect(AudioSample* sfx, float volume, float pitchShift) {
    if(!sfxVoices[nextSFX]) {
        XAUDIO2_VOICE_SENDS outputData = {};
        outputData.SendCount = 1;
        XAUDIO2_SEND_DESCRIPTOR descriptor = { 0, sfxChannel };
        outputData.pSends = &descriptor;
        engine->CreateSourceVoice(&sfxVoices[nextSFX], (WAVEFORMATEX*)&sfx->format, 0, 2.0f, nullptr, &outputData);
        sfxVoices[nextSFX]->Start();
    } else {
        XAUDIO2_VOICE_STATE state;
        sfxVoices[nextSFX]->GetState(&state);
        if(state.BuffersQueued <= 0) {
            sfxVoices[nextSFX]->FlushSourceBuffers();
        }

        sfxVoices[nextSFX]->SetSourceSampleRate(sfx->format.samplesPerSec);
    }
	
	// start the sound
	sfxVoices[nextSFX]->SetVolume(volume * sfx->baseVolume);
    sfxVoices[nextSFX]->SetFrequencyRatio(pitchShift >= 0 ? 1.0f + pitchShift : 1.0f / (1.0f - pitchShift));
    XAUDIO2_BUFFER soundBuffer = ConvertBuffer(sfx);
    sfxVoices[nextSFX]->SubmitSourceBuffer(&soundBuffer);

    nextSFX = (nextSFX + 1) % MAX_SFX;
}

void WindowsAudio::BeginTrack(AudioSample* track, int8_t trackIndex, bool looped) {
    if(musicVoices[trackIndex] == nullptr) {
        XAUDIO2_VOICE_SENDS outputData = {};
        outputData.SendCount = 1;
        XAUDIO2_SEND_DESCRIPTOR descriptor = { 0, musicChannel };
        outputData.pSends = &descriptor;
        engine->CreateSourceVoice(&musicVoices[trackIndex], (WAVEFORMATEX*)&track->format, 0, 2.0f, nullptr, &outputData);
    }

    XAUDIO2_BUFFER soundBuffer = ConvertBuffer(track);
    musicVoices[trackIndex]->SubmitSourceBuffer(&soundBuffer);
    musicVoices[trackIndex]->Start();
}

void WindowsAudio::PauseTrack(int8_t trackIndex) {
    musicVoices[trackIndex]->Stop();
}

void WindowsAudio::ResumeTrack(int8_t trackIndex) {
    musicVoices[trackIndex]->Start();
}

void WindowsAudio::EndTrack(int8_t trackIndex) {
    musicVoices[trackIndex]->Stop();
    musicVoices[trackIndex]->FlushSourceBuffers();
}

void WindowsAudio::SetTotalVolume(int8_t trackIndex, float volume) {
    musicVoices[trackIndex]->SetVolume(volume);
}

bool WindowsAudio::IsDonePlaying(int8_t trackIndex) {
    XAUDIO2_VOICE_STATE state;
    musicVoices[trackIndex]->GetState(&state);
    return state.BuffersQueued == 0;
}

XAUDIO2_BUFFER WindowsAudio::ConvertBuffer(const AudioSample* audio) {
    XAUDIO2_BUFFER result = {};
    result.AudioBytes = audio->bufferLength;
    result.pAudioData = audio->audioBuffer;
    result.Flags = XAUDIO2_END_OF_STREAM;
    return result;
}
#endif