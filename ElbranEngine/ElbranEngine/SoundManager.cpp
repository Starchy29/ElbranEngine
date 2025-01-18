#include "SoundManager.h"

SoundManager::SoundManager(HRESULT* outResult) {
	*outResult = XAudio2Create(engine.GetAddressOf(), 0, XAUDIO2_DEFAULT_PROCESSOR);
	if(FAILED(*outResult)) return;

	*outResult = engine->CreateMasteringVoice(&masteringVoice);
	if(FAILED(*outResult)) return;
}

SoundManager::~SoundManager() {

}
