#include "Sound.h"
#include "Application.h"

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

Sound::Sound(std::wstring fileName) {
    WAVEFORMATEXTENSIBLE wfx = {};
    soundBuffer = {};

    // Open the file
    std::wstring fullPath = APP->ExePath() + L"Assets\\" + fileName;
    HANDLE file = CreateFile(
        fullPath.c_str(),
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        0,
        NULL );

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
    soundBuffer.AudioBytes = chunkSize;
    soundBuffer.pAudioData = pDataBuffer;
    soundBuffer.Flags = XAUDIO2_END_OF_STREAM; // tell the source voice not to expect any data after this buffer
    soundBuffer.LoopCount = XAUDIO2_NO_LOOP_REGION;

    // create a source voice
    HRESULT hResult = APP->Audio()->engine->CreateSourceVoice(&voice, (WAVEFORMATEX*)&wfx);
    assert(hResult == S_OK && "failed to create source voice for audio file");
    //hResult = voice->SubmitSourceBuffer(&soundBuffer);
    //assert(hResult == S_OK && "failed to submit source buffer for audio file");

    voice->Start();
}

Sound::~Sound() {
    delete soundBuffer.pAudioData;
}

void Sound::Play() {
    voice->SubmitSourceBuffer(&soundBuffer);
}

HRESULT Sound::FindChunk(HANDLE file, DWORD fourcc, DWORD& chunkSize, DWORD& chunkDataPosition) {
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

HRESULT Sound::ReadChunkData(HANDLE file, void* buffer, DWORD buffersize, DWORD bufferoffset) {
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