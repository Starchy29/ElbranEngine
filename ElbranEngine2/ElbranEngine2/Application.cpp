#include "Application.h"
#include "GraphicsAPI.h"
#include "Game.h"
#include "AssetContainer.h"
#include "Random.h"

Application* app;

Application::Application(std::wstring filePath, GraphicsAPI* graphics) {
	this->graphics = graphics;
	// sounds = new SoundMixer();
	// InputManager* input;
	assets = new AssetContainer(filePath, graphics);
	rng = new Random();

	this->filePath = filePath;
}

Application::~Application() {
	delete rng;
	delete game;
	// delete input;
	// delete sounds;
	delete assets;
	delete graphics;
}

void Application::SetupGame() {
	// called after the constructor, when all assets are loaded
	game = new Game();
}

void Application::Update(float deltaTime) {
	//input->Update();
	//audio->Update(fDeltaTime);
	game->Update(deltaTime);
	graphics->Render(game);

	//input->mouseWheelDelta = 0.0f;
}
