#include "Application.h"
#include "GraphicsAPI.h"
#include "Game.h"

Application* app;

Application::Application(std::wstring filePath, GraphicsAPI* graphics) {
	// rng = new Random();
	this->graphics = graphics;
	// sounds = new SoundMixer();
	// InputManager* input;
	// assets = new AssetContainer();
	game = new Game();

	this->filePath = filePath;
}

Application::~Application() {
	delete graphics;
	// delete input;
	// delete sounds;
	// delete assets;
	delete game;
	// delete rng;
}

void Application::Update(float deltaTime) {
	//input->Update();
	//audio->Update(fDeltaTime);
	//game->Update(fDeltaTime);
	graphics->Render(game);

	//input->mouseWheelDelta = 0.0f;
}
