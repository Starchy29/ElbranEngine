#include "Game.h"
#include "Scene.h"

Game::Game() {
	testScene = new Scene(5.0f);
}

Game::~Game() {
	delete testScene;
}

void Game::Update(float deltaTime) {
	testScene->Update(deltaTime);
}

void Game::Draw() {
	testScene->Draw();
}
