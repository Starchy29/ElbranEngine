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
	
	ShapeRenderer* trans = new ShapeRenderer(ShapeRenderer::Shape::Square, Color(0, 0, 1, 0.3f));
	testScene->AddRenderer(trans, true);
	trans->transform->scale *= 1.0f;
	trans->transform->zOrder = 70;
	trans = new ShapeRenderer(ShapeRenderer::Shape::Square, Color(0, 1, 0, 0.6f));
	testScene->AddRenderer(trans, true);
	trans->transform->scale *= 2.0f;
	trans->transform->zOrder = 80;
	trans = new ShapeRenderer(ShapeRenderer::Shape::Square, Color(1, 0, 0, 0.8f));
	testScene->AddRenderer(trans, true);
	trans->transform->scale *= 3.0f;
	trans->transform->zOrder = 90;

	SpriteRenderer* checker = new SpriteRenderer(&app->assets->testSprite);
	testScene->AddRenderer(checker, false);
	checker->transform->position.x -= 2.0f;
	checker->transform->parent = cursor->transform;
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

	BloomPostProcess* bloomer = new BloomPostProcess();
	bloomer->threshold = 0.8f;
	app->graphics->postProcesses.push_back(bloomer);
}

Game::~Game() {
	delete testScene;
}

void Game::Update(float deltaTime) {
	cursor->transform->position = app->input->GetMousePosition(&testScene->camera);
	testScene->Update(deltaTime);

	if(app->input->JustPressed(InputAction::Select, 0)) {
		app->input->Rumble(0, 0.1f, 2.1f);
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
