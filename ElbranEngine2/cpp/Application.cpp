#include "Application.h"
#include "GraphicsAPI.h"
#include "SoundMixer.h"
#include "InputManager.h"

Application app;

void Application::Initialize(bool littleEndian, std::wstring filePath, LoadedFile (*fileLoadFunction)(std::wstring fileName), GraphicsAPI* graphics, SoundMixer* audio, InputManager* input) {
	perFrameData.Allocate(8192);
	quitFunction = nullptr;
	this->littleEndian = littleEndian;

	this->LoadFile = fileLoadFunction;
	this->filePath = filePath;
	this->graphics = graphics;
	this->audio = audio;
	this->input = input;

	rng.Initialize();
	assets.Initialize(filePath, graphics, audio);
	game.Initialize();
}

void Application::Release() {
	rng.Release();
	game.Release();
	assets.Release();
	delete input;
	delete audio;
	graphics->Release();
	delete graphics;
	perFrameData.Release();
}

void Application::Update(float deltaTime) {
	input->Update(deltaTime);
	audio->Update(deltaTime);
	game.Update(deltaTime);
	graphics->Render(&game);
	perFrameData.Clear();
}
