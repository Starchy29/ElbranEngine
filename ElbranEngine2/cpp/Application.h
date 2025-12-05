#pragma once
#include "Game.h"
#include "AssetContainer.h"
#include "MemoryArena.h"
#include "SoundMixer.h"
#include "GraphicsAPI.h"
#include "InputManager.h"

#if defined(DEBUG) | defined(_DEBUG)
#include "DebugHelper.h"
#endif

class Application {
public:
	GraphicsAPI graphics;
	SoundMixer audio;
	InputManager input;
	AssetContainer assets;
	Game game;
	MemoryArena frameBuffer;

#if defined(DEBUG) | defined(_DEBUG)
	DebugHelper debugger;
#endif
	void (*quitFunction)();

	Application() = default;
	void Initialize(UInt2 windowSize, PlatformGraphics* platformGraphics, PlatformAudio* platformAudio, PlatformInput* platformInput);
	void Release();

	void StepFrame(float deltaTime);
};