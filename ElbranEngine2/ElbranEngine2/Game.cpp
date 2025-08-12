#include "Game.h"
#include "Scene.h"
#include "Application.h"
#include "AssetContainer.h"

Game::Game() {
	sampleScene = Scene(2, 5.0f);
	sampleScene.backgroundColor = Color(0.1f, 0.1f, 0.1f);
}

Game::~Game() {
	sampleScene.Release();
}

void Game::Update(float deltaTime) {
	sampleScene.Update(deltaTime);
}

void Game::Draw() {
	sampleScene.Draw();
}
