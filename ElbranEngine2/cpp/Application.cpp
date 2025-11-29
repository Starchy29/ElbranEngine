#include "Application.h"
#include "Random.h"

Application app;

void Application::Initialize(LoadedFile (*fileLoadFunction)(std::wstring fileName), UInt2 windowSize, PlatformGraphics* platformGraphics, PlatformAudio* platformAudio, PlatformInput* platformInput) {
	frameBuffer.Allocate(8192);
	quitFunction = nullptr;

	this->LoadFile = fileLoadFunction;
	graphics.Initialize(platformGraphics, windowSize);
	audio.Initialize(platformAudio);
	input.Initialize(platformInput);

	_rng.Initialize();
	assets.Initialize(&graphics);
	game.Initialize();
}

void Application::Release() {
	_rng.Release();
	game.Release(&graphics);
	assets.Release(&graphics);
	input.Release();
	audio.Release();
	graphics.Release();
	frameBuffer.Release();
}

void Application::StepFrame(float deltaTime) {
	input.Update(deltaTime, graphics.viewportDims, graphics.viewportOffset);
	audio.Update(deltaTime);
	game.Update(deltaTime);

	graphics.ResetRenderTargets();
	game.Draw();
	graphics.PresentFrame();

	frameBuffer.Clear();
}
