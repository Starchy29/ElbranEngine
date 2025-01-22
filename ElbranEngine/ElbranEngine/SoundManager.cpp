#include "SoundManager.h"
#include "Application.h"

SoundManager::SoundManager(HRESULT* outResult) {
    masterVolume = 1.0f;
    musicVolume = 1.0f;
    sfxVolume = 1.0f;

    fadeOutTime = 0.f;
    fadeInTime = 0.f;
    state = FadeState::None;

	*outResult = XAudio2Create(engine.GetAddressOf(), 0, XAUDIO2_DEFAULT_PROCESSOR);
	if(FAILED(*outResult)) return;

	*outResult = engine->CreateMasteringVoice(&masteringVoice);
	if(FAILED(*outResult)) return;
}

SoundManager::~SoundManager() {
    
}

void SoundManager::Update(float deltaTime) {
    switch(state) {
    case FadeState::Out:
        timer -= deltaTime;

        if(timer <= 0) {
            // end fade out
            currentSong->Pause();
            state = nextSong != nullptr && fadeInTime > 0 ? FadeState::In : FadeState::None;
            if(state == FadeState::In) {
                timer = fadeInTime;
            }

            if(nextSong != nullptr) {
                nextSong->Restart();
                nextSong->SetVolume(state == FadeState::In ? 0.0f : 1.0f);
            }

            currentSong = nextSong;
            nextSong = nullptr;
        } else {
            currentSong->SetVolume(timer / fadeOutTime);
        }
        break;
    case FadeState::In:
        timer -= deltaTime;

        if(timer <= 0) {
            // end fade in
            state = FadeState::None;
        } else {
            currentSong->SetVolume(1.0f - timer / fadeInTime);
        }
        break;
    }
}

// if song is nullptr, stops playing the current song.
void SoundManager::StartSong(std::shared_ptr<MusicTrack> song, float fadeOutTime, float fadeInTime) {
    this->fadeOutTime = fadeOutTime;
    this->fadeInTime = fadeInTime;

    if(currentSong != nullptr) {
        if(fadeOutTime > 0) {
            // fade out
            state = FadeState::Out;
            nextSong = song;
            timer = fadeOutTime;
            return;
        } else {
            // stop playing immediately
            currentSong->Pause();
        }
    }

    currentSong = song;
    state = song != nullptr && fadeInTime > 0 ? FadeState::In : FadeState::None;
    if(state == FadeState::In) {
        timer = fadeInTime;
    }

    if(song != nullptr) {
        song->Restart();
        song->SetVolume(state == FadeState::In ? 0.0f : 1.0f);
    }
}

void SoundManager::SetMasterVolume(float volume) {
    masterVolume = max(0, volume);
    if(currentSong != nullptr) {
        currentSong->SetVolume(currentSong->GetCurrentVolume());
    }
}

void SoundManager::SetMusicVolume(float volume) {
    musicVolume = max(0, volume);
    if(currentSong != nullptr) {
        currentSong->SetVolume(currentSong->GetCurrentVolume());
    }
}

void SoundManager::SetEffectsVolume(float volume) {
    sfxVolume = max(0, volume);
}

std::shared_ptr<MusicTrack> SoundManager::GetCurrentSong() const {
    return currentSong;
}

Microsoft::WRL::ComPtr<IXAudio2> SoundManager::GetAudioEngine() const {
    return engine;
}

float SoundManager::GetMasterVolume() const {
    return masterVolume;
}

float SoundManager::GetMusicVolume() const {
    return musicVolume;
}

float SoundManager::GetEffectsVolume() const {
    return sfxVolume;
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

WAVEFORMATEXTENSIBLE SoundManager::LoadAudio(XAUDIO2_BUFFER* destination, std::wstring fileName) {
    WAVEFORMATEXTENSIBLE wfx = {};
    *destination = {};

    // Open the file
    std::wstring fullPath = APP->ExePath() + L"Assets\\" + fileName;
    HANDLE file = CreateFile(
        fullPath.c_str(),
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        0,
        NULL);

    assert(file != INVALID_HANDLE_VALUE && "sound file not found");

    DWORD result = SetFilePointer(file, 0, NULL, FILE_BEGIN);
    assert(result != INVALID_SET_FILE_POINTER && "failed to load sound file");

    DWORD chunkSize;
    DWORD chunkPosition;

    //check the file type, should be fourccWAVE or 'XWMA'
    FindChunk(file, fourccRIFF, chunkSize, chunkPosition);
    DWORD filetype;
    ReadChunkData(file, &filetype, sizeof(DWORD), chunkPosition);
    assert(filetype == fourccWAVE && "invalid audio file format");

    // load the 'fmt' chunk
    FindChunk(file, fourccFMT, chunkSize, chunkPosition);
    ReadChunkData(file, &wfx, chunkSize, chunkPosition);

    // load the 'data' chunk
    FindChunk(file, fourccDATA, chunkSize, chunkPosition);
    BYTE* pDataBuffer = new BYTE[chunkSize];
    ReadChunkData(file, pDataBuffer, chunkSize, chunkPosition);

    // populate the buffer
    destination->AudioBytes = chunkSize;
    destination->pAudioData = pDataBuffer;
    destination->Flags = XAUDIO2_END_OF_STREAM; // tell the source voice not to expect any data after this buffer

    return wfx;
}

HRESULT SoundManager::FindChunk(HANDLE file, DWORD fourcc, DWORD& chunkSize, DWORD& chunkDataPosition) {
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

HRESULT SoundManager::ReadChunkData(HANDLE file, void* buffer, DWORD buffersize, DWORD bufferoffset) {
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