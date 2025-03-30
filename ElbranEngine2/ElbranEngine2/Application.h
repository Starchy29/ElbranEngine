#pragma once
class GraphicsAPI;
class Game;

#define MAX_FPS 60.f
#define ASPECT_RATIO 16.f / 9.f

class Application
{
public:
	GraphicsAPI* graphics;
	// InputManager* input;
	// SoundMixer* sounds;
	// AssetContainer* assets;
	Game* game;
	// Random* rng;

	Application(GraphicsAPI* graphics);
	~Application();

	void Update(float deltaTime);

private:

};

extern Application* app; // pointer to the app singleton, created and destroyed by the platform

