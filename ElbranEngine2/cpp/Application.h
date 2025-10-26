#pragma once
#include "Game.h"
#include "AssetContainer.h"
#include "Random.h"
#include <string>
#include "MemoryArena.h"

class GraphicsAPI;
class SoundMixer;
class InputManager;
#if defined(DEBUG) | defined(_DEBUG)
#include "DebugHelper.h"
#endif

class Application
{
public:
	GraphicsAPI* graphics;
	SoundMixer* audio;
	InputManager* input;

	AssetContainer assets;
	Game game;
	Random rng;
	MemoryArena perFrameData;
#if defined(DEBUG) | defined(_DEBUG)
	DebugHelper debugger;
#endif

	std::wstring filePath;
	void (*quitFunction)();

	Application() {}
	void Initialize(std::wstring filePath, GraphicsAPI* graphics, SoundMixer* audio, InputManager* input);
	void Release();

	void Update(float deltaTime);
};

extern Application app;