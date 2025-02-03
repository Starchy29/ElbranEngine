#include "Game.h"
#include "Application.h"
#include "AssetManager.h"

Game::Game(const AssetManager* assets) {
	sampleScene = new Scene(10, Color(0.1f, 0.1f, 0.1f));
	sampleScene->Add(new GameObject(0, assets->testImage, false));
}

Game::~Game() {
	delete sampleScene;
}

void Game::Update(float deltaTime) {
	sampleScene->Update(deltaTime);
}

void Game::Draw() {
	sampleScene->Draw();
}
