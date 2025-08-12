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
#include "UserInterface.h"
#include "Button.h"
#include "SliderUI.h"
#include "DebugHelper.h"

SpriteRenderer* background;
UserInterface* testUI;
Button* butt1;
Button* butt2;
Button* butt3;
Button* butt4;
TextRenderer* testTexter;
ShapeRenderer* textBox;
SliderUI* slider;
ParticleRenderer* particles;
ParticleBehavior* partUpdater;

void RandomizeColor(Button* clicked) {
	clicked->box.color = Color(app->rng->GenerateFloat(0, 1), app->rng->GenerateFloat(0, 1), app->rng->GenerateFloat(0, 1));
}

Game::Game() {
	testScene = new Scene(20, 12.0f);
	testScene->backgroundColor = Color(0.1f, 0.1f, 0.1f);

	background = new SpriteRenderer(&app->assets->apple);
	testScene->AddRenderer(background, false);
	background->transform->zOrder = 99.f;
	background->transform->scale = testScene->camera.GetWorldDimensions();

	//SpriteRenderer* glyphAtlasShower = new SpriteRenderer(&app->assets->testFont.glyphAtlas);
	//testScene->AddRenderer(glyphAtlasShower, false);
	//glyphAtlasShower->transform->scale *= 2.f;

	testTexter = new TextRenderer("abcdefghijklmnop\nqrstuvwxyz\nABCDEFGHIJKLMNOP\nQRSTUVWXYZ", &app->assets->testFont, HorizontalAlignment::Right, 0.3f);
	testTexter->verticalAlignment = VerticalAlignment::Top;
	testTexter->padding = 0.3f;
	//testTexter->maxSize = 3.0f;
	//testTexter->SetText("f");
	testScene->AddRenderer(testTexter, true);
	testTexter->transform->scale = Vector2(5.0f, 4.0f);
	testTexter->transform->position.x += 0.5f;
	testTexter->transform->zOrder = 50;

	textBox = new ShapeRenderer(ShapeRenderer::Shape::Square, Color::Black);
	testScene->AddRenderer(textBox, false);
	textBox->transform->parent = testTexter->transform;
	textBox->transform->zOrder = 1;


	//testTexter->color = Color::Blue;
	//testTexter->transform->position.x += 0.5f;
	//testTexter->transform->rotation += 0.1f;

	particles = new ParticleRenderer(200, app->assets->testParticle);
	testScene->AddRenderer(particles, true);
	particles->transform->scale *= 3.0f;
	particles->blendAdditive = true;
	particles->lifespan = 0.5f;
	particles->startWidth = 0.5f;
	particles->transform->scale *= 0.5f;
	
	partUpdater = new ParticleBehavior(particles);
	testScene->AddBehavior(partUpdater);
	partUpdater->SetSpawnRate(100);
	partUpdater->moveStyle = ParticleBehavior::MoveStyle::Outward;
	partUpdater->speed = -2.0f;
	partUpdater->spinRate = 2.0f;
	partUpdater->fadeOutDuration = 0.2f;

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
	delete background;

	testTexter->Release();
	delete testTexter;
	delete textBox;

	delete testUI;
	delete butt1;
	delete butt2;
	delete butt3;
	delete butt4;
	delete slider;
	particles->Release();
	delete particles;
	partUpdater->Release();
	delete partUpdater;
}

void Game::Update(float deltaTime) {
	UInt2 viewSize = app->graphics->GetViewDimensions();

	particles->transform->position = app->input->GetMousePosition(&testScene->camera);
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

	if(app->input->IsPressed(InputAction::Up)) {
		testTexter->transform->scale.y += 5.0f * deltaTime;
	}
	if(app->input->IsPressed(InputAction::Down)) {
		testTexter->transform->scale.y -= 5.0f * deltaTime;
	}
	if(app->input->IsPressed(InputAction::Left)) {
		testTexter->transform->scale.x -= 5.0f * deltaTime;
	}
	if(app->input->IsPressed(InputAction::Right)) {
		testTexter->transform->scale.x += 5.0f * deltaTime;
	}
}

void Game::Draw() {
	testScene->Draw();
}
