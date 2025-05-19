#include "Application.h"
#include "GraphicsAPI.h"
#include "SoundMixer.h"
#include "Game.h"
#include "AssetContainer.h"
#include "InputManager.h"
#include "Random.h"

Application* app;

void Application::InitApp(std::wstring filePath, GraphicsAPI* graphics, SoundMixer* audio, InputManager* input) {
	app = new Application(filePath, graphics, audio, input);
	app->assets = new AssetContainer(filePath, graphics, audio);
	app->game = new Game();
}

Application::Application(std::wstring filePath, GraphicsAPI* graphics, SoundMixer* audio, InputManager* input) {
	this->filePath = filePath;
	this->graphics = graphics;
	this->audio = audio;
	this->input = input;
	rng = new Random();
}

Application::~Application() {
	delete rng;
	delete game;
	delete input;
	delete assets;
	delete audio;
	delete graphics;
}

void Application::Update(float deltaTime) {
	input->Update(deltaTime);
	audio->Update(deltaTime);
	game->Update(deltaTime);
	graphics->Render(game);
}
