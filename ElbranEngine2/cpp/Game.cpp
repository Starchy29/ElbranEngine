#include "Game.h"
#include "Application.h"

#include "RenderGroup.h"
RenderGroup sampleScene;
Transform transforms[10];
Matrix worldMatrices[10];
Renderer renderers[10];

Renderer* spriteTest;

void Game::Initialize() {
	sampleScene.Initialize(transforms, worldMatrices, renderers, 10, 10);
	sampleScene.backgroundColor = Color(0.1f, 0.1f, 0.1f);
	sampleScene.camera.viewWidth = 10.f;

	//text.Initialize("text", &app->assets.arial);
	//sampleScene.AddRenderer(&text, true);
	//sprite.Initialize(&app->assets.testSprite);
	//sampleScene.AddRenderer(&sprite, true);
	//sprite.flipX = true;

	spriteTest = sampleScene.ReserveRenderer();
	spriteTest->InitSprite(&app->assets.testSprite);
	spriteTest->spriteData.flipX = true;
	spriteTest->spriteData.tint = Color::Red;
}

void Game::Release() {

}

void Game::Update(float deltaTime) {

}

void Game::Draw() {
	sampleScene.Draw();
}
