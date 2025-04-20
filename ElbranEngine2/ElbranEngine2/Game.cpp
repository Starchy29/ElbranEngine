#include "Game.h"
#include "Scene.h"
#include "Application.h"
#include "AssetContainer.h"

#include "HSVPostProcess.h"

Game::Game() {
	testScene = new Scene(5, 5.0f);
	testScene->backgroundImage = &app->assets->apple;
	SpriteRenderer* checker = testScene->AddSprite(&app->assets->testSprite);

	//HSVPostProcess* pp = new HSVPostProcess();
	//app->graphics->postProcesses.push_back(pp);
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
