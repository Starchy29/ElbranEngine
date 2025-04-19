#include "Game.h"
#include "Scene.h"
#include "Application.h"
#include "AssetContainer.h"

Game::Game() {
	testScene = new Scene(5, 5.0f);
	SpriteRenderer* checker = testScene->AddSprite(&app->assets->testSprite);
	testScene->RemoveSprite(checker);
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
