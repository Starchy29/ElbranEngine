#include "Game.h"
#include "Application.h"

#include "InputManager.h"
#include "RenderGroup.h"
#include "GraphicsAPI.h"
#include "PostProcess.h"
RenderGroup sampleScene;
Transform transforms[10];
Matrix worldMatrices[10];
Renderer renderers[10];

Renderer* spriteTest;
Renderer* cursor;
Renderer* textTest;

PostProcess ppTest[2];

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

	ppTest[0].HSV(0.f, -1.0f, 0.f);
	ppTest[1].Blur(10);
}

void Game::Release() {
	sampleScene.ReleaseRenderers();
}

void Game::Update(float deltaTime) {
	cursor->transform->position = app->input->GetMousePosition(&sampleScene.camera);

	if(app->input->IsPressed(InputAction::Up)) {
		//ppTest.bloomData.brightnessThreshold += deltaTime;
	}
	else if(app->input->IsPressed(InputAction::Down)) {
		//ppTest.bloomData.brightnessThreshold -= deltaTime;
	}
}

void Game::Draw() {
	sampleScene.Draw();
	app->graphics->ApplyPostProcesses(ppTest, 2);
}
