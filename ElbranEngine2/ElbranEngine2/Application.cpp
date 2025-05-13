#include "Application.h"
#include "GraphicsAPI.h"
#include "SoundMixer.h"
#include "Game.h"
#include "AssetContainer.h"
#include "InputManager.h"
#include "Random.h"

Application* app;

Application::Application(std::wstring filePath, GraphicsAPI* graphics, SoundMixer* audio, InputManager* input) {
	this->graphics = graphics;
	this->audio = audio;
	this->input = input;
	assets = new AssetContainer(filePath, graphics, audio);
	rng = new Random();

	this->filePath = filePath;
}

Application::~Application() {
	delete rng;
	delete game;
	delete input;
	delete assets;
	delete audio;
	delete graphics;
}

void Application::SetupGame() {
	// called after the constructor, when all assets are loaded
	game = new Game();
}

void Application::Update(float deltaTime) {
	input->Update(deltaTime);
	audio->Update(deltaTime);
	game->Update(deltaTime);
	graphics->Render(game);
}
