#include "MusicTrack.h"
#include "Application.h"

MusicTrack::MusicTrack(std::wstring fileName, float baseVolume) {
	this->baseVolume = baseVolume;

	WAVEFORMATEXTENSIBLE wfx = APP->Audio()->LoadAudio(&soundBuffer, fileName);
	soundBuffer.LoopCount = XAUDIO2_LOOP_INFINITE;

	APP->Audio()->GetAudioEngine()->CreateSourceVoice(&voice, (WAVEFORMATEX*)&wfx);
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
	voice->SetVolume(baseVolume * volume * APP->Audio()->GetMusicVolume());
	currentVolume = volume;
}

float MusicTrack::GetCurrentVolume() {
	return currentVolume;
}
