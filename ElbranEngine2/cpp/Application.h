#pragma once
#include "Game.h"
#include "AssetContainer.h"
#include "Random.h"
#include "MemoryArena.h"
#include "LoadedFile.h"
#include "SoundMixer.h"

class GraphicsAPI;
class InputManager;
#if defined(DEBUG) | defined(_DEBUG)
#include "DebugHelper.h"
#endif

class Application
{
public:
	GraphicsAPI* graphics;
	SoundMixer audio;
	InputManager* input;

	AssetContainer assets;
	Game game;
	Random rng;
	MemoryArena frameBuffer;
#if defined(DEBUG) | defined(_DEBUG)
	DebugHelper debugger;
#endif

	LoadedFile (*LoadFile)(std::wstring fileName);
	void (*quitFunction)();

	Application() = default;
	void Initialize(LoadedFile (*fileLoadFunction)(std::wstring fileName), GraphicsAPI* graphics, PlatformAudio* platformAudio, InputManager* input);
	void Release();

	void StepFrame(float deltaTime);
};

extern Application* app;