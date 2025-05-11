#pragma once
#include <string>
class GraphicsAPI;
class SoundMixer;
class Game;
class AssetContainer;
class InputManager;
class Random;

class Application
{
public:
	GraphicsAPI* graphics;
	SoundMixer* audio;
	InputManager* input;
	AssetContainer* assets;
	Game* game;
	Random* rng;

	std::wstring filePath;

	Application(std::wstring filePath, GraphicsAPI* graphics, SoundMixer* audio, InputManager* input);
	~Application();

	void SetupGame();
	void Update(float deltaTime);
};

extern Application* app; // pointer to the app singleton, created and destroyed by the platform