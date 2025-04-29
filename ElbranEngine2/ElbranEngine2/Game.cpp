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

Game::Game() {
	testScene = new Scene(10, 5.0f);
	//testScene->backgroundColor = Color::Black;
	testScene->backgroundImage = &app->assets->apple;
	SpriteRenderer* checker = new SpriteRenderer(&app->assets->testSprite);
	testScene->AddRenderer(checker, false);
	//checker->lit = true;

	testScene->AddRenderer(new ShapeRenderer(ShapeRenderer::Shape::Circle, Color(0, 1, 1, 0.6f)), true);

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
	//blurBoy->blurRadius = 100;

	//BloomPostProcess* bloomer = new BloomPostProcess();
	//bloomer->threshold = 0.7f;
	//app->graphics->postProcesses.push_back(bloomer);
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
