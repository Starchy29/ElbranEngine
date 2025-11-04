#include "Game.h"
#include "Application.h"

#include "InputManager.h"
#include "RenderGroup.h"
RenderGroup sampleScene;
Transform transforms[10];
Matrix worldMatrices[10];
Renderer renderers[10];

Renderer* spriteTest;
Renderer* cursor;
Renderer* textTest;

char label[] = "here is\ntext";

void Game::Initialize() {
	sampleScene.Initialize(transforms, worldMatrices, renderers, 10, 10);
	sampleScene.backgroundColor = Color(0.1f, 0.1f, 0.1f);
	sampleScene.camera.viewWidth = 10.f;

	spriteTest = sampleScene.ReserveRenderer();
	spriteTest->InitSprite(&app->assets.testSprite);
	spriteTest->spriteData.flipX = true;
	spriteTest->spriteData.tint = Color::Red;

	spriteTest->InitPattern(&app->assets.testSprite);
	spriteTest->transform->scale *= 5.0f;

	cursor = sampleScene.ReserveRenderer();
	cursor->InitShape(PrimitiveShape::Circle, Color(0.1f, 0.8f, 0.3f, 0.5f));
	spriteTest->transform->zOrder = 10.f;

	textTest = sampleScene.ReserveRenderer();
	textTest->InitText(label, &app->assets.arial);
}

void Game::Release() {
	sampleScene.ReleaseRenderers();
}

void Game::Update(float deltaTime) {
	cursor->transform->position = app->input->GetMousePosition(&sampleScene.camera);
}

void Game::Draw() {
	sampleScene.Draw();
}
