#pragma once

#define MAX_FPS 60.f

class Application
{
public:
	// GraphicsAPI* graphics;
	// InputManager* input;
	// SoundMixer* sounds;
	// AssetContainer* assets;
	// Game* game;
	// Random* rng;

	Application();
	~Application();

	void Update(float deltaTime);

private:

};

extern Application* app; // pointer to the app singleton, created and destroyed by the platform

