#include "Application.h"
#include "GraphicsAPI.h"
#include "SoundMixer.h"
#include "InputManager.h"

Application app;

void Application::Initialize(LoadedFile (*fileLoadFunction)(std::wstring fileName), UInt2 windowSize, PlatformGraphics* platformGraphics, PlatformAudio* platformAudio, PlatformInput* platformInput) {
	frameBuffer.Allocate(8192);
	quitFunction = nullptr;

	this->LoadFile = fileLoadFunction;
	graphics.Initialize(platformGraphics, windowSize);
	audio.Initialize(platformAudio);
	input.Initialize(platformInput);

	rng.Initialize();
	assets.Initialize(&graphics);
	game.Initialize();
}

void Application::Release() {
	rng.Release();
	game.Release();
	assets.Release();
	input.Release();
	audio.Release();
	graphics.Release();
	frameBuffer.Release();
}

void Application::StepFrame(float deltaTime) {
	input.Update(deltaTime);
	audio.Update(deltaTime);
	game.Update(deltaTime);

	graphics.ResetRenderTargets();
	game.Draw();
	graphics.PresentFrame();

	frameBuffer.Clear();
}
