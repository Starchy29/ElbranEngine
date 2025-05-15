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
#include "Tween.h"
#include "AtlasRenderer.h"

float t = 0.f;
SpriteRenderer* linear;
SpriteRenderer* tweener;

Game::Game() {
	testScene = new Scene(10, 5.0f);
	testScene->backgroundColor = Color(0.1f, 0.1f, 0.1f);

	SpriteRenderer* background = new SpriteRenderer(&app->assets->apple);
	testScene->AddRenderer(background, false);
	background->transform->zOrder = 99.f;
	background->transform->scale = testScene->camera.GetWorldDimensions();

	cursor = new ShapeRenderer(ShapeRenderer::Shape::Circle, Color(0, 1, 1, 0.6f));
	testScene->AddRenderer(cursor, true);
	cursor->transform->scale *= 0.3f;

	linear = new SpriteRenderer(&app->assets->testSprite);
	testScene->AddRenderer(linear, false);
	linear->transform->position.y = 0.5f;
	linear->transform->scale *= 0.5f;

	tweener = new SpriteRenderer(&app->assets->testSprite);
	testScene->AddRenderer(tweener, false);
	tweener->transform->position.y = -0.5f;
	tweener->transform->scale *= 0.5f;

	AtlasRenderer* testSheet = new AtlasRenderer(&app->assets->testAtlas);
	testScene->AddRenderer(testSheet, false);
	testSheet->transform->scale *= 2.0f;
	testSheet->row = 1;
	testSheet->col = 1;

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
	//pp->contrast = -1.0f;
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
	t += 0.7f * deltaTime;
	if(t > 1.0f) t -= 1.0f;
	linear->transform->position.x = Tween::Lerp(-2.0f, 2.0f, t);
	tweener->transform->position.x = -2.0f + 4.0f * Tween::EaseInOut(t);

	cursor->transform->position = app->input->GetMousePosition(&testScene->camera);
	testScene->Update(deltaTime);

	if(app->input->JustPressed(InputAction::Select, 0)) {
		//app->input->Rumble(0, 0.1f, 2.1f);
		//app->audio->PlayEffect(&app->assets->testSound, 1.0f, app->rng->GenerateFloat(-1.f, 1.f));
		//app->audio->PauseTrack(&app->assets->testMusic, 0.5f);
		//app->audio->SetTrackVolume(&app->assets->testMusic, 0.3f, 1.0f);
	}
	if(app->input->JustPressed(InputAction::Down)) {
		//app->audio->PlayTrack(&app->assets->testMusic, true);
		//app->audio->SetTrackVolume(&app->assets->testMusic, 1.0f, 1.0f);
	}

	cursor->transform->rotation += 20.0f * app->input->GetMouseWheelSpin() * deltaTime;

	if(app->input->IsPressed(InputAction::Up)) {
		//((BloomPostProcess*)app->graphics->postProcesses[0])->threshold += deltaTime;
	}
	if(app->input->IsPressed(InputAction::Down)) {
		//((BloomPostProcess*)app->graphics->postProcesses[0])->threshold -= deltaTime;
	}

	/*if (app->input->IsPressed(InputAction::Up)) {
		testScene->camera.transform->position.y += deltaTime;
	}
	if(app->input->IsPressed(InputAction::Down)) {
		testScene->camera.transform->position.y -= deltaTime;
	}
	if(app->input->IsPressed(InputAction::Left)) {
		testScene->camera.transform->position.x -= deltaTime;
	}
	if(app->input->IsPressed(InputAction::Right)) {
		testScene->camera.transform->position.x += deltaTime;
	}*/
}

void Game::Draw() {
	testScene->Draw();
}
