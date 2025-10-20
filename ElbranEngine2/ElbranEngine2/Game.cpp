#include "Game.h"
#include "Scene.h"
#include "Application.h"

#include "TextRenderer.h"
TextRenderer text;

#include "SpriteRenderer.h"
SpriteRenderer sprite;

void Game::Initialize() {
	sampleScene.Initialize(10, 10, 10, 5.0f);
	sampleScene.backgroundColor = Color(0.1f, 0.1f, 0.1f);

	LoadedFile load = app.LoadFile(app.filePath + L"Assets\\elbran.png", true);
	load.Release();

	text.Initialize("text", &app.assets.arial);
	//sampleScene.AddRenderer(&text, true);
	sprite.Initialize(&app.assets.testBMP);
	sampleScene.AddRenderer(&sprite, true);
}

void Game::Release() {
	sampleScene.Release();
	text.Release();
}

void Game::Update(float deltaTime) {

}

void Game::Draw() {
	sampleScene.Draw();
}
