#include "Application.h"
#include "GraphicsAPI.h"
#include "Game.h"
#include "AssetContainer.h"

Application* app;

Application::Application(std::wstring filePath, GraphicsAPI* graphics) {
	this->graphics = graphics;
	// sounds = new SoundMixer();
	// InputManager* input;
	assets = new AssetContainer(graphics);
	game = new Game();
	// rng = new Random();

	this->filePath = filePath;
}

Application::~Application() {
	// delete rng;
	// delete input;
	// delete sounds;
	delete game;
	delete assets;
	delete graphics;
}

void Application::Update(float deltaTime) {
	//input->Update();
	//audio->Update(fDeltaTime);
	//game->Update(fDeltaTime);
	graphics->Render(game);

	//input->mouseWheelDelta = 0.0f;
}
