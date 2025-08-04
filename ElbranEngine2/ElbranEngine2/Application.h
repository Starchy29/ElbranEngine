#pragma once
#include <string>
#include "MemoryArena.h"

class GraphicsAPI;
class SoundMixer;
class Game;
class AssetContainer;
class InputManager;
class Random;
#if defined(DEBUG) | defined(_DEBUG)
class DebugHelper;
#endif

class Application
{
public:
	GraphicsAPI* graphics;
	SoundMixer* audio;
	InputManager* input;
	AssetContainer* assets;
	Game* game;
	Random* rng;
	MemoryArena perFrameData;
#if defined(DEBUG) | defined(_DEBUG)
	DebugHelper* debugger;
#endif

	std::wstring filePath;
	void (*quitFunction)();

	static void InitApp(std::wstring filePath, GraphicsAPI* graphics, SoundMixer* audio, InputManager* input);
	void Release();

	void Update(float deltaTime);

private:
	Application(std::wstring filePath, GraphicsAPI* graphics, SoundMixer* audio, InputManager* input);
};

extern Application* app; // pointer to the app singleton, created and destroyed by the platform