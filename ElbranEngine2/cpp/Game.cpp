#include "Game.h"
#include "Application.h"

#include "RenderGroup.h"
RenderGroup testGroup;
Renderer* testSprite;

void Game::Initialize(Application* app) {
	testGroup.Initialize(10, 5);
	testSprite = testGroup.ReserveRenderer();
	testSprite->InitSprite(&app->assets.testSprite);
}

void Game::Release(GraphicsAPI* graphics) {
	testGroup.Release(graphics);
}

void Game::Update(Application* app, float deltaTime) {
	testSprite->transform->position = app->input.GetMousePosition(&testGroup.camera);
}

void Game::Draw(GraphicsAPI* graphics, const AssetContainer* assets, MemoryArena* frameBuffer) {
	testGroup.Draw(graphics, assets, frameBuffer);
}
