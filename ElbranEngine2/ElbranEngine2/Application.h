#pragma once
#include <string>
class GraphicsAPI;
class Game;
class AssetContainer;
class InputManager;
class Random;

class Application
{
public:
	GraphicsAPI* graphics;
	InputManager* input;
	// SoundMixer* sounds;
	AssetContainer* assets;
	Game* game;
	Random* rng;

	std::wstring filePath;

	Application(std::wstring filePath, GraphicsAPI* graphics, InputManager* input);
	~Application();

	void SetupGame();
	void Update(float deltaTime);
};

extern Application* app; // pointer to the app singleton, created and destroyed by the platform