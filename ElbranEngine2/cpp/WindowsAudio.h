#ifdef WINDOWS
#pragma once
#include "SoundMixer.h"
#include <xaudio2.h>
#include <wrl/client.h>

class WindowsAudio :
    public SoundMixer
{
public:
    WindowsAudio();

    virtual void SetMasterVolume(float volume) override;
    virtual void SetMusicVolume(float volume) override;
    virtual void SetEffectsVolume(float volume) override;

    AudioTrack CreateTrack(AudioSample audio, bool looped) override;
    void ReleaseAudioTrack(AudioTrack* track) override;

    SoundEffect CreateEffect(AudioSample audio) override;
    void PlayEffect(SoundEffect* sfx, float volume = 1.f, float pitchShift = 0.f) override;
    void ReleaseSoundEffect(SoundEffect* effect) override;

protected:
    void RestartTrack(AudioTrack* track) override;
    void ResumeTrack(AudioTrack* track) override;
    void StopTrack(AudioTrack* track) override;
    void SetTotalVolume(AudioTrack* track, float volume) override;

private:
    Microsoft::WRL::ComPtr<IXAudio2> engine;
    IXAudio2MasteringVoice* masterChannel;
    IXAudio2SubmixVoice* musicChannel;
    IXAudio2SubmixVoice* sfxChannel;

    XAUDIO2_BUFFER ConvertBuffer(AudioSample audio);
};
#endif