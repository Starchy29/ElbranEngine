#ifdef WINDOWS
#include "WindowsAudio.h"
#include "Common.h"

#define DEFAULT_SAMPLE_RATE 44100

WindowsAudio::WindowsAudio() {
    XAudio2Create(engine.GetAddressOf(), 0, XAUDIO2_DEFAULT_PROCESSOR);
    engine->CreateMasteringVoice(&masterChannel);
    engine->CreateSubmixVoice(&musicChannel, 1, DEFAULT_SAMPLE_RATE);
    engine->CreateSubmixVoice(&sfxChannel, 1, DEFAULT_SAMPLE_RATE);
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
    WAVEFORMATEXTENSIBLE wfx = {};
    LoadAudio(&track.soundBuffer, &wfx, directory + L"assets\\" + fileName);
    track.baseVolume = 1.0f;
    track.mixVolume = 0.0f;
    track.soundBuffer.LoopCount = looped ? XAUDIO2_LOOP_INFINITE : XAUDIO2_NO_LOOP_REGION;

    XAUDIO2_VOICE_SENDS outputData = {};
    outputData.SendCount = 1;
    XAUDIO2_SEND_DESCRIPTOR descriptor = { 0, musicChannel };
    outputData.pSends = &descriptor;
    engine->CreateSourceVoice(&track.voice, (WAVEFORMATEX*)&wfx, 0, 2.0f, nullptr, &outputData);

    track.voice->SubmitSourceBuffer(&track.soundBuffer);
    track.voice->SetVolume(track.baseVolume);
    return track;
}

void WindowsAudio::ReleaseAudioTrack(AudioTrack* track) {
    track->voice->Stop();
    delete track->soundBuffer.pAudioData;
}

SoundEffect WindowsAudio::LoadEffect(std::wstring directory, std::wstring fileName) {
    SoundEffect effect = {};
    LoadAudio(&effect.soundBuffer, &effect.format, directory + L"assets\\" + fileName);
    effect.baseVolume = 1.0f;
    effect.soundBuffer.LoopCount = XAUDIO2_NO_LOOP_REGION;
	return effect;
}

void WindowsAudio::PlayEffect(SoundEffect* sfx, float volume, float pitchShift) {
    IXAudio2SourceVoice* usedVoice = nullptr;

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
		engine->CreateSourceVoice(&usedVoice, (WAVEFORMATEX*)&sfx->format, 0, 2.0f, nullptr, &outputData);
        sfx->voices.push_back(usedVoice);
		usedVoice->Start();
	}
	
	// start the sound
	usedVoice->SetVolume(volume * sfx->baseVolume);
	usedVoice->SetFrequencyRatio(pitchShift >= 0 ? 1.0f + pitchShift : 1.0f / (1.0f - pitchShift));
	usedVoice->SubmitSourceBuffer(&sfx->soundBuffer);
}

void WindowsAudio::ReleaseSoundEffect(SoundEffect* effect) {
    delete effect->soundBuffer.pAudioData;
    for(IXAudio2SourceVoice* voice : effect->voices) {
        voice->Stop();
    }
}

void WindowsAudio::RestartTrack(AudioTrack* track) {
    track->voice->Stop();
    track->voice->FlushSourceBuffers();
    track->voice->SubmitSourceBuffer(&track->soundBuffer);
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

// https://learn.microsoft.com/en-us/windows/win32/xaudio2/how-to--load-audio-data-files-in-xaudio2
#ifdef _XBOX //Big-Endian
#define fourccRIFF 'RIFF'
#define fourccDATA 'data'
#define fourccFMT 'fmt '
#define fourccWAVE 'WAVE'
#define fourccXWMA 'XWMA'
#define fourccDPDS 'dpds'
#endif

#ifndef _XBOX //Little-Endian
#define fourccRIFF 'FFIR'
#define fourccDATA 'atad'
#define fourccFMT ' tmf'
#define fourccWAVE 'EVAW'
#define fourccXWMA 'AMWX'
#define fourccDPDS 'sdpd'
#endif

void WindowsAudio::LoadAudio(XAUDIO2_BUFFER* destination, WAVEFORMATEXTENSIBLE* outFormat, std::wstring fullFilePath) {
    *outFormat = {};
    *destination = {};

    // Open the file
    HANDLE file = CreateFileW(
        fullFilePath.c_str(),
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        0,
        NULL);

    ASSERT(file != INVALID_HANDLE_VALUE);

    DWORD result = SetFilePointer(file, 0, NULL, FILE_BEGIN);
    ASSERT(result != INVALID_SET_FILE_POINTER);

    DWORD chunkSize;
    DWORD chunkPosition;

    //check the file type, should be fourccWAVE or 'XWMA'
    FindChunk(file, fourccRIFF, chunkSize, chunkPosition);
    DWORD filetype;
    ReadChunkData(file, &filetype, sizeof(DWORD), chunkPosition);
    ASSERT(filetype == fourccWAVE); // fails for invalid audio formats

    // load the 'fmt' chunk
    FindChunk(file, fourccFMT, chunkSize, chunkPosition);
    ReadChunkData(file, outFormat, chunkSize, chunkPosition);

    // load the 'data' chunk
    FindChunk(file, fourccDATA, chunkSize, chunkPosition);
    BYTE* pDataBuffer = new BYTE[chunkSize];
    ReadChunkData(file, pDataBuffer, chunkSize, chunkPosition);

    // populate the buffer
    destination->AudioBytes = chunkSize;
    destination->pAudioData = pDataBuffer;
    destination->Flags = XAUDIO2_END_OF_STREAM; // tell the source voice not to expect any data after this buffer
}

HRESULT WindowsAudio::FindChunk(HANDLE file, DWORD fourcc, DWORD& chunkSize, DWORD& chunkDataPosition) {
	HRESULT hr = S_OK;
    if(SetFilePointer(file, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    DWORD chunkType;
    DWORD chunkDataSize;
    DWORD riffDataSize = 0;
    DWORD fileType;
    DWORD bytesRead = 0;
    DWORD offset = 0;

    while(hr == S_OK) {
        DWORD dwRead;
        if(ReadFile(file, &chunkType, sizeof(DWORD), &dwRead, NULL) == 0) {
            hr = HRESULT_FROM_WIN32(GetLastError());
        }

        if(ReadFile(file, &chunkDataSize, sizeof(DWORD), &dwRead, NULL) == 0) {
            hr = HRESULT_FROM_WIN32(GetLastError());
        }

        switch(chunkType) {
        case fourccRIFF:
            riffDataSize = chunkDataSize;
            chunkDataSize = 4;
            if(ReadFile(file, &fileType, sizeof(DWORD), &dwRead, NULL) == 0) {
                hr = HRESULT_FROM_WIN32(GetLastError());
            }
            break;

        default:
            if(SetFilePointer(file, chunkDataSize, NULL, FILE_CURRENT) == INVALID_SET_FILE_POINTER) {
                return HRESULT_FROM_WIN32(GetLastError());
            }
            break;
        }

        offset += sizeof(DWORD) * 2;

        if(chunkType == fourcc) {
            chunkSize = chunkDataSize;
            chunkDataPosition = offset;
            return S_OK;
        }

        offset += chunkDataSize;

        if(bytesRead >= riffDataSize) {
            return S_FALSE;
        }
    }

    return S_OK;
}

HRESULT WindowsAudio::ReadChunkData(HANDLE file, void* buffer, DWORD buffersize, DWORD bufferoffset) {
	HRESULT hr = S_OK;
    if(SetFilePointer(file, bufferoffset, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    DWORD dwRead;
    if(ReadFile(file, buffer, buffersize, &dwRead, NULL) == 0) {
        hr = HRESULT_FROM_WIN32(GetLastError());
    }
    return hr;
}
#endif