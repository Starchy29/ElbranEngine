#include "Application.h"
#include "GraphicsAPI.h"
#include "SoundMixer.h"
#include "InputManager.h"

Application* app;

void Application::Initialize(bool littleEndian, LoadedFile (*fileLoadFunction)(std::wstring fileName), GraphicsAPI* graphics, SoundMixer* audio, InputManager* input) {
	perFrameData.Allocate(8192);
	quitFunction = nullptr;
	this->littleEndian = littleEndian;

	this->LoadFile = fileLoadFunction;
	this->graphics = graphics;
	this->audio = audio;
	this->input = input;

	rng.Initialize();
	assets.Initialize(graphics, audio);
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

void Application::StepFrame(float deltaTime) {
	input->Update(deltaTime);
	audio->Update(deltaTime);
	game.Update(deltaTime);

	graphics->ResetRenderTargets();
	game.Draw();
	graphics->PresentFrame();

	perFrameData.Clear();
}
