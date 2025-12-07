#include "Game.h"
#include "Application.h"

#include "RenderGroup.h"
#include "ParticleBehavior.h"
#include "FileIO.h"
RenderGroup sampleScene;

Renderer* spriteTest;
Renderer* cursor;
Renderer* textTest;
Renderer* particleRend;
ParticleBehavior partBeh;

PostProcess ppTest[2];

char label[] = "here is\ntext";

void Game::Initialize(GraphicsAPI* graphics, const AssetContainer* assets, MemoryArena* frameBuffer) {
	sampleScene.Initialize(10, 10);
	sampleScene.backgroundColor = Color(0.1f, 0.1f, 0.1f);
	sampleScene.camera.viewWidth = 10.f;

	spriteTest = sampleScene.ReserveRenderer();
	spriteTest->InitSprite(&assets->testSprite);
	spriteTest->spriteData.flipX = true;
	spriteTest->spriteData.tint = Color::Red;

	//spriteTest->InitPattern(&app->assets.testSprite);
	//spriteTest->transform->scale *= 5.0f;

	cursor = sampleScene.ReserveRenderer();
	cursor->InitShape(PrimitiveShape::Circle, Color(0.1f, 0.8f, 0.3f, 0.5f));
	spriteTest->transform->zOrder = 10.f;

	textTest = sampleScene.ReserveRenderer();
	textTest->InitText(graphics, frameBuffer, label, &assets->arial);

	ppTest[0].HSV(0.f, -1.0f, 0.f);
	ppTest[1].Blur(10);

	particleRend = sampleScene.ReserveRenderer();
	particleRend->InitParticles(graphics, 200, &assets->testSheet, 1.f);
	partBeh.Initialize(graphics, particleRend);
	partBeh.SetSpawnRate(10.f, 5.f);
	partBeh.lifespan = 0.5f;
	partBeh.fadeOutDuration = 0.2f;
	partBeh.moveStyle = ParticleBehavior::MoveStyle::Random;
	partBeh.spinRate = 2.0f;
	partBeh.startWidth = 0.3f;
	partBeh.speed = 2.0f;
}

void Game::Release(GraphicsAPI* graphics) {
	sampleScene.Release(graphics);
	partBeh.Release(graphics);
}

void Game::Update(Application* app, float deltaTime) {
	partBeh.Update(&app->graphics, &app->assets, &app->frameBuffer, deltaTime);
	cursor->transform->position = app->input.GetMousePosition(&sampleScene.camera);

	if(app->input.JustPressed(InputAction::LeftCLick)) {
		app->debugger.debugScene = &sampleScene;
		app->debugger.AddDot(app->input.GetMousePosition(&sampleScene.camera));
	}

	if(app->input.IsPressed(InputAction::Up)) {
		//ppTest.bloomData.brightnessThreshold += deltaTime;
	}
	else if(app->input.IsPressed(InputAction::Down)) {
		//ppTest.bloomData.brightnessThreshold -= deltaTime;
	}

	if(app->input.JustPressed(InputAction::Up) || app->input.JustPressed(InputAction::Down)) {
		app->audio.PlayEffect(&app->assets.testSound);
	}
}

void Game::Draw(GraphicsAPI* graphics, const AssetContainer* assets, MemoryArena* frameBuffer) {
	sampleScene.Draw(graphics, assets, frameBuffer);
	//app->graphics->ApplyPostProcesses(ppTest, 2);
}
