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

Application::Application(std::wstring filePath, GraphicsAPI* graphics, SoundMixer* audio, InputManager* input) 
	: perFrameData(1024)
{
	this->filePath = filePath;
	this->graphics = graphics;
	this->audio = audio;
	this->input = input;
	rng = new Random();
}

void Application::Release() {
	delete rng;
	delete game;
	delete input;
	delete assets;
	delete audio;
	graphics->Release();
	delete graphics;
	perFrameData.Release();
}

void Application::Update(float deltaTime) {
	input->Update(deltaTime);
	audio->Update(deltaTime);
	game->Update(deltaTime);
	graphics->Render(game);
	perFrameData.Reset();
}
