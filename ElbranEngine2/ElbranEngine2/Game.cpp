#include "Game.h"
#include "Scene.h"
#include "Application.h"

void Game::Initialize() {
	sampleScene.Initialize(10, 10, 10, 10, 5.0f);
	sampleScene.backgroundColor = Color(0.1f, 0.1f, 0.1f);
}

void Game::Release() {
	sampleScene.Release();
}

void Game::Update(float deltaTime) {
	sampleScene.Update(deltaTime);
}

void Game::Draw() {
	sampleScene.Draw();
}
