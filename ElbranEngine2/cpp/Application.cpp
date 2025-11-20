#include "Application.h"
#include "GraphicsAPI.h"
#include "SoundMixer.h"
#include "InputManager.h"

Application* app;

void Application::Initialize(LoadedFile (*fileLoadFunction)(std::wstring fileName), GraphicsAPI* graphics, PlatformAudio* platformAudio, InputManager* input) {
	frameBuffer.Allocate(8192);
	quitFunction = nullptr;

	this->LoadFile = fileLoadFunction;
	this->graphics = graphics;
	audio.Initialize(platformAudio);
	this->input = input;

	rng.Initialize();
	assets.Initialize(graphics);
	game.Initialize();
}

void Application::Release() {
	rng.Release();
	game.Release();
	assets.Release();
	delete input;
	audio.Release();
	graphics->Release();
	delete graphics;
	frameBuffer.Release();
}

void Application::StepFrame(float deltaTime) {
	input->Update(deltaTime);
	audio.Update(deltaTime);
	game.Update(deltaTime);

	graphics->ResetRenderTargets();
	game.Draw();
	graphics->PresentFrame();

	frameBuffer.Clear();
}
