#ifdef WINDOWS
#pragma once
#include "AudioData.h"

class IXAudio2;
class IXAudio2MasteringVoice;
class IXAudio2SubmixVoice;
class IXAudio2SourceVoice;
struct XAUDIO2_BUFFER;

class WindowsAudio {
public:
    WindowsAudio();
    ~WindowsAudio();

    void SetMasterVolume(float volume);
    void SetMusicVolume(float volume);
    void SetEffectsVolume(float volume);

    void PlayEffect(const AudioSample* sfx, float volume = 1.f, float pitchShift = 0.f);
    void BeginTrack(const AudioSample* track, int8_t trackIndex, bool looped);
    void PauseTrack(int8_t trackIndex);
    void ResumeTrack(int8_t trackIndex);
    void EndTrack(int8_t trackIndex);
    void SetTotalVolume(int8_t trackIndex, float volume);
    bool IsDonePlaying(int8_t trackIndex);

private:
    IXAudio2* engine;
    IXAudio2MasteringVoice* masterChannel;
    IXAudio2SubmixVoice* musicChannel;
    IXAudio2SubmixVoice* sfxChannel;
    IXAudio2SourceVoice* musicVoices[MAX_MUSIC_CHANNELS];
    IXAudio2SourceVoice* sfxVoices[MAX_SFX];
    uint8_t nextSFX;

    static XAUDIO2_BUFFER ConvertBuffer(const AudioSample* audio);
};
#endif