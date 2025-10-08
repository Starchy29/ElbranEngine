#pragma once
#include "Game.h"
#include "AssetContainer.h"
#include "Random.h"
#include <string>
#include "MemoryArena.h"
#include "LoadedFile.h"

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
	LoadedFile (*LoadFile)(std::wstring fileName, bool littleEndian);
	void (*quitFunction)();

	Application() {}
	void Initialize(std::wstring filePath, LoadedFile (*fileLoadFunction)(std::wstring fileName, bool littleEndian), GraphicsAPI* graphics, SoundMixer* audio, InputManager* input);
	void Release();

	void Update(float deltaTime);
};

extern Application app;