#ifdef WINDOWS
#pragma once
#include "SoundMixer.h"
#include <wrl/client.h>

class IXAudio2;
class IXAudio2MasteringVoice;
class IXAudio2SubmixVoice;
class IXAudio2SourceVoice;
struct XAUDIO2_BUFFER;

class WindowsAudio :
    public SoundMixer
{
public:
    WindowsAudio();
    ~WindowsAudio();

    virtual void SetMasterVolume(float volume) override;
    virtual void SetMusicVolume(float volume) override;
    virtual void SetEffectsVolume(float volume) override;

    void PlayEffect(AudioSample* sfx, float volume = 1.f, float pitchShift = 0.f) override;

protected:
    void BeginTrack(AudioSample* track, int8_t trackIndex, bool looped) override;
    void PauseTrack(int8_t trackIndex) override;
    void ResumeTrack(int8_t trackIndex) override;
    void EndTrack(int8_t trackIndex) override;
    void SetTotalVolume(int8_t trackIndex, float volume) override;
    bool IsDonePlaying(int8_t trackIndex) override;

private:
    Microsoft::WRL::ComPtr<IXAudio2> engine;
    IXAudio2MasteringVoice* masterChannel;
    IXAudio2SubmixVoice* musicChannel;
    IXAudio2SubmixVoice* sfxChannel;
    IXAudio2SourceVoice* musicVoices[MAX_MUSIC_CHANNELS];
    IXAudio2SourceVoice* sfxVoices[MAX_SFX];
    uint8_t nextSFX;

    static XAUDIO2_BUFFER ConvertBuffer(const AudioSample* audio);
};
#endif