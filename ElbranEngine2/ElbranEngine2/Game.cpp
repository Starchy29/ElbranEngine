#include "Game.h"
#include "Scene.h"
#include "Application.h"
#include "AssetContainer.h"

#include "HSVPostProcess.h"
#include "BlurPostProcess.h"
#include "BloomPostProcess.h"
#include "SpriteRenderer.h"
#include "ParticleBehavior.h"
#include "ShapeRenderer.h"
#include "InputManager.h"
#include "GraphicsAPI.h"
#include "SoundMixer.h"
#include "Random.h"
#include "Math.h"
#include "AtlasRenderer.h"
#include "AtlasAnimator.h"
#include "AnimationGroup.h"
#include "PatternRenderer.h"
#include "TextRenderer.h"

Game::Game() {
	testScene = new Scene(10, 15.0f);
	testScene->backgroundColor = Color(0.1f, 0.1f, 0.1f);

	SpriteRenderer* background = new SpriteRenderer(&app->assets->apple);
	testScene->AddRenderer(background, false);
	background->transform->zOrder = 99.f;
	background->transform->scale = testScene->camera.GetWorldDimensions();

	cursor = new ShapeRenderer(ShapeRenderer::Shape::Circle, Color(0, 1, 1, 0.6f));
	testScene->AddRenderer(cursor, true);
	cursor->transform->scale *= 0.3f;

	//SpriteRenderer* glyphAtlasShower = new SpriteRenderer(&app->assets->testFont.glyphAtlas);
	//testScene->AddRenderer(glyphAtlasShower, false);
	//glyphAtlasShower->transform->scale *= 2.f;

	TextRenderer* testTexter = new TextRenderer("when the\ntext renders:", &app->assets->testFont);
	testScene->AddRenderer(testTexter, false);
	testTexter->transform->scale.x = 3.0f;
	ShapeRenderer* textBox = new ShapeRenderer(ShapeRenderer::Shape::Square, Color::Black);
	testScene->AddRenderer(textBox, false);
	textBox->transform->parent = testTexter->transform;
	textBox->transform->zOrder = 1; 

	//AtlasRenderer* testSheet = new AtlasRenderer(&app->assets->testAtlas);
	//testScene->AddRenderer(testSheet, false);
	//testSheet->transform->scale *= 2.0f;
	//testSheet->row = 1;
	//testSheet->col = 1;

	//animator = new AtlasAnimator(testSheet, 8.f);
	//testScene->AddBehavior(animator);
	//animator->looped = true;

	//checker->lit = true;

	//ParticleRenderer* parts = new ParticleRenderer(200, app->assets->testSprite);
	//ParticleBehavior* swarm = new ParticleBehavior(parts);
	//testScene->AddRenderer(parts, true);
	//testScene->AddBehavior(swarm);
	//swarm->SetSpawnRate(10.f);
	//parts->transform->zOrder = 10.f;
	//swarm->Spawn(200);
	//parts->transform->zOrder = 0.99f;

	//HSVPostProcess* pp = new HSVPostProcess();
	//app->graphics->postProcesses.push_back(pp);
	//pp->contrast = 0.5f;
	//pp->saturation = -1.0f;

	//BlurPostProcess* blurBoy = new BlurPostProcess();
	//app->graphics->postProcesses.push_back(blurBoy);
	//blurBoy->blurRadius = 50;

	//BloomPostProcess* bloomer = new BloomPostProcess();
	//bloomer->threshold = 0.8f;
	//app->graphics->postProcesses.push_back(bloomer);
}

Game::~Game() {
	delete testScene;
}

void Game::Update(float deltaTime) {
	UInt2 viewSize = app->graphics->GetViewDimensions();

	cursor->transform->position = app->input->GetMousePosition(&testScene->camera);
	testScene->Update(deltaTime);

	if(app->input->JustPressed(InputAction::Select, 0)) {
		//app->input->Rumble(0, 0.1f, 2.1f);
		//app->audio->PlayEffect(&app->assets->testSound, 1.0f, app->rng->GenerateFloat(-1.f, 1.f));
		//app->audio->PauseTrack(&app->assets->testMusic, 0.5f);
		//app->audio->SetTrackVolume(&app->assets->testMusic, 0.3f, 1.0f);
		//animator->Restart(true);
	}
	if(app->input->JustPressed(InputAction::Down)) {
		//app->audio->PlayTrack(&app->assets->testMusic, true);
		//app->audio->SetTrackVolume(&app->assets->testMusic, 1.0f, 1.0f);
	}

	cursor->transform->rotation += 20.0f * app->input->GetMouseWheelSpin() * deltaTime;

	if (app->input->IsPressed(InputAction::Up)) {
		testScene->camera.transform->position.y += 3.f * deltaTime;
	}
	if(app->input->IsPressed(InputAction::Down)) {
		testScene->camera.transform->position.y -= 3.f * deltaTime;
	}
	if(app->input->IsPressed(InputAction::Left)) {
		testScene->camera.transform->position.x -= 3.f * deltaTime;
	}
	if(app->input->IsPressed(InputAction::Right)) {
		testScene->camera.transform->position.x += 3.f * deltaTime;
	}
}

void Game::Draw() {
	testScene->Draw();
}
