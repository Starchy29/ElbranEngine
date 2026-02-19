#pragma once
#include "Game.h"
#include "AssetContainer.h"
#include "MemoryArena.h"
#include "SoundMixer.h"
#include "GraphicsAPI.h"
#include "InputManager.h"

class Application {
public:
	GraphicsAPI graphics;
	SoundMixer audio;
	InputManager input;
	AssetContainer assets;
	Game game;
	MemoryArena frameBuffer;

	void (*quitFunction)();

	Application() = default;
	void Initialize(void (*quitFunction)(), UInt2 windowSize, PlatformGraphics* platformGraphics, PlatformAudio* platformAudio, PlatformInput* platformInput);
	void Release();

	void StepFrame(float deltaTime);
};