#include "Application.h"
#include "Random.h"

void Application::Initialize(UInt2 windowSize, PlatformGraphics* platformGraphics, PlatformAudio* platformAudio, PlatformInput* platformInput) {
	frameBuffer.Allocate(4194304);
	_rng.Initialize();
	quitFunction = nullptr;

	graphics.Initialize(platformGraphics, windowSize);
	audio.Initialize(platformAudio);
	input.Initialize(platformInput);

	assets.Initialize(&graphics, &frameBuffer);
	game.Initialize(&graphics, &assets, &frameBuffer);
	frameBuffer.Clear();
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
	game.Update(this, deltaTime);

	graphics.ResetRenderTargets();
	game.Draw(&graphics, &assets, &frameBuffer);
	graphics.PresentFrame();

	frameBuffer.Clear();
}
