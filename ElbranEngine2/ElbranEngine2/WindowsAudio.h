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

    AudioTrack LoadTrack(std::wstring directory, std::wstring fileName, bool looped) override;
    void ReleaseAudioTrack(AudioTrack* track) override;

    SoundEffect LoadEffect(std::wstring directory, std::wstring fileName) override;
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

    static void LoadAudio(XAUDIO2_BUFFER* destination, WAVEFORMATEXTENSIBLE* outFormat, std::wstring fullFilePath);
    static HRESULT FindChunk(HANDLE file, DWORD fourcc, DWORD& chunkSize, DWORD& chunkDataPosition);
    static HRESULT ReadChunkData(HANDLE file, void* buffer, DWORD buffersize, DWORD bufferoffset);
};
#endif