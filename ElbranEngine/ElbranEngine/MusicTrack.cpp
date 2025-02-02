#include "MusicTrack.h"
#include "Application.h"
#include "SoundManager.h"

MusicTrack::MusicTrack(std::wstring fileName, float baseVolume) {
	this->baseVolume = baseVolume;

	WAVEFORMATEXTENSIBLE wfx = APP->Audio()->LoadAudio(&soundBuffer, fileName);
	soundBuffer.LoopCount = XAUDIO2_LOOP_INFINITE;

	SoundManager* sounds = APP->Audio();
	XAUDIO2_VOICE_SENDS outputData = {};
	outputData.SendCount = 1;
	XAUDIO2_SEND_DESCRIPTOR descriptor = { 0, sounds->GetMusicChannel() };
	outputData.pSends = &descriptor;
	sounds->GetAudioEngine()->CreateSourceVoice(&voice, (WAVEFORMATEX*)&wfx, 0, 2.0f, nullptr, &outputData);

	voice->SubmitSourceBuffer(&soundBuffer);
	SetVolume(1.0f);
}

MusicTrack::~MusicTrack() {
	voice->Stop();
	delete soundBuffer.pAudioData;
}

void MusicTrack::Play() {
	voice->Start();
}

void MusicTrack::Pause() {
	voice->Stop();
}

void MusicTrack::Restart() {
	voice->Stop();
	voice->FlushSourceBuffers();
	voice->SubmitSourceBuffer(&soundBuffer);
	SetVolume(1.0f);
	voice->Start();
}

void MusicTrack::SetVolume(float volume) {
	voice->SetVolume(baseVolume * volume);
	currentVolume = volume;
}

float MusicTrack::GetCurrentVolume() {
	return currentVolume;
}
