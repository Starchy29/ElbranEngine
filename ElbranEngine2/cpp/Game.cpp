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

void Game::Initialize(Application* app) {
	sampleScene.Initialize(10, 10);
	sampleScene.backgroundImage = &app->assets.apple;
	//sampleScene.backgroundColor = Color::Black; //Color(0.1f, 0.1f, 0.1f);
	sampleScene.camera.viewWidth = 10.f;
	sampleScene.ambientLight = Color::Black;

	spriteTest = sampleScene.ReserveRenderer();
	spriteTest->InitSprite(&app->assets.testSprite);
	spriteTest->spriteData.flipX = true;
	spriteTest->spriteData.tint = Color::Red;

	//spriteTest->InitPattern(&app->assets.testSprite);
	//spriteTest->transform->scale *= 5.0f;
	Renderer* back = sampleScene.ReserveRenderer();
	back->InitSprite(&app->assets.apple);
	back->transform->scale = sampleScene.camera.GetWorldDimensions();
	back->transform->zOrder = 99.f;
	back->spriteData.lit = true;

	cursor = sampleScene.ReserveRenderer();
	cursor->InitShape(PrimitiveShape::Circle, Color(0.1f, 0.8f, 0.3f));
	spriteTest->transform->zOrder = 10.f;
	cursor->InitLight(Color::Red, 2.0f);
	cursor->lightData.coneSize = 0.4f;

	textTest = sampleScene.ReserveRenderer();
	textTest->InitText(&app->graphics, &app->frameBuffer, label, &app->assets.arial);

	ppTest[0].Bloom(15, 0.5f);

	particleRend = sampleScene.ReserveRenderer();
	particleRend->InitParticles(&app->graphics, 200, &app->assets.testSheet, 1.f);
	partBeh.Initialize(&app->graphics, particleRend);
	partBeh.SetSpawnRate(10.f, 5.f);
	partBeh.lifespan = 0.5f;
	partBeh.fadeOutDuration = 0.2f;
	partBeh.moveStyle = ParticleBehavior::MoveStyle::Random;
	partBeh.spinRate = 2.0f;
	partBeh.startWidth = 0.3f;
	partBeh.speed = 2.0f;

	app->audio.StartTrack(&app->assets.testSong, true, 1.0f, 3.0f);
}

void Game::Release(GraphicsAPI* graphics) {
	sampleScene.Release(graphics);
	partBeh.Release(graphics);
}

void Game::Update(Application* app, float deltaTime) {
	partBeh.Update(&app->graphics, &app->assets, &app->frameBuffer, deltaTime);
	cursor->transform->position = app->input.GetMousePosition(&sampleScene.camera);

	if(app->input.JustPressed(InputAction::LeftCLick)) {
		particleRend->ClearParticles(&app->graphics, &app->assets);
		app->audio.SetPaused(&app->assets.testSong, true, 2.0f);
	}

	if(app->input.IsPressed(InputAction::Up)) {
		ppTest[0].bloomData.thresholdSensitivity += deltaTime;
		//cursor->shapeData.color = cursor->shapeData.color.SetHue(cursor->shapeData.color.GetHue() + deltaTime);
	}
	else if(app->input.IsPressed(InputAction::Down)) {
		ppTest[0].bloomData.thresholdSensitivity -= deltaTime;
		//Color& cursorColor = cursor->shapeData.color;
		//cursorColor = Color::FromHSV(cursorColor.GetHue(), cursorColor.GetSaturation(), cursorColor.GetBrightness() - deltaTime);
	}

	if(app->input.JustPressed(InputAction::Up) || app->input.JustPressed(InputAction::Down)) {
		//app->audio.PlayEffect(&app->assets.testSound);
	}
}

void Game::Draw(GraphicsAPI* graphics, const AssetContainer* assets, MemoryArena* frameBuffer) {
	sampleScene.Draw(graphics, assets, frameBuffer);
	//graphics->ApplyPostProcesses(assets, ppTest, 1);
}
