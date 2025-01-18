#pragma once
#include "xaudio2.h"
#include <string>

class Sound
{
public:
	Sound(std::wstring fileName);
	~Sound();

	void Play();

private:
	XAUDIO2_BUFFER soundBuffer; // delete this and just keep the voice?
	IXAudio2SourceVoice* voice;

	static HRESULT FindChunk(HANDLE file, DWORD fourcc, DWORD& chunkSize, DWORD& chunkDataPosition);
	static HRESULT ReadChunkData(HANDLE file, void* buffer, DWORD buffersize, DWORD bufferoffset);
};

