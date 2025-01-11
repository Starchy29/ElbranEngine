#include "Game.h"
#include "Application.h"
#include "Enums.h"

#include "TextRenderer.h"
#include "Menu.h"
#include "AtlasRenderer.h"
#include "SpriteAnimator.h"
#include "HueSwapRenderer.h"
#include "BlurPostProcess.h"
#include "HSVPostProcess.h"
#include "BloomPostProcess.h"
#include "StretchRenderer.h"
#include "ParticleRenderer.h"

Game::Game(const AssetManager* assets) {
	sampleScene = new Scene(10, Color(0.1f, 0.1f, 0.1f));

	ParticleRenderer* spawner = new ParticleRenderer(200, 0.8f, std::make_shared<SpriteAtlas>(L"animation.png", 3, 3, 8));
	GameObject* particles = new GameObject(-1, spawner, true);
	sampleScene->Add(particles);
	spawner->spawnRadius = 1.0f;
	spawner->width = 0.4f;
	//spawner->secondsPerFrame = 0.1f;
	//spawner->spinRate = -6.3f;
	//spawner->growthRate = 0.1f;
	//spawner->applyLights = true;
	//spawner->Spawn(50, 0.5f);
	spawner->speed = 1.0f;
	spawner->moveStyle = ParticleMovement::Outward;
	spawner->faceMoveDirection = true;
	spawner->SetSpawnRate(30.f);
	spawner->fadeDuration = 0.8f;

	//APP->Graphics()->postProcesses.push_back(new HSVPostProcess(0, -1, 0));
	//APP->Graphics()->postProcesses.push_back(new BloomPostProcess(0.7f, 50));
	//APP->Graphics()->postProcesses.push_back(new BlurPostProcess(10));

	testObject = new GameObject(1.5f, 1.f, Color::White);
	sampleScene->Add(testObject);
	//testObject->GetTransform()->Scale(0.6f);

	//StretchRenderer* repeater = new StretchRenderer(std::make_shared<Sprite>(L"stretcher.png"), Vector2(2, 2), Vector2(0.5f, 0.167f), Vector2(0.833f, 0.833f));
	//picture = new GameObject(0, RenderMode::Opaque, repeater);
	//picture->GetTransform()->SetScale(3, 2);
	//sampleScene->Add(picture);
	//repeater->useLights = true;

	GameObject* photo = new GameObject(0, std::make_shared<Sprite>(L"apple.jpeg"), false);
	sampleScene->Add(photo);
	photo->GetTransform()->GrowWidth(7.3f);

	photo->GetRenderer<SpriteRenderer>()->useLights = true;
	sampleScene->ambientLight = Color::Black;
}

Game::~Game() {
	delete sampleScene;
}

void Game::Update(float deltaTime) {
	sampleScene->Update(deltaTime);
	testObject->GetTransform()->SetPosition(APP->Input()->GetMousePosition(sampleScene->GetCamera()));

	if(APP->Input()->KeyJustPressed(VK_MOUSE_LEFT)) {
		const Random* rng = APP->RNG();
		GameObject* newLight = new GameObject(1.5f, 1.f, Color(rng->GenerateFloat(0.f, 1.f), rng->GenerateFloat(0.f, 1.f), rng->GenerateFloat(0.f, 1.f)));
		sampleScene->Add(newLight);
		newLight->GetTransform()->SetPosition(APP->Input()->GetMousePosition(sampleScene->GetCamera()));
		newLight->GetTransform()->SetRotation(rng->GenerateFloat(0.f, 6.f));
	}

	if(APP->Input()->IsKeyPressed(VK_UP)) {
		picture->GetTransform()->Stretch(0, deltaTime);
	} 
	else if(APP->Input()->IsKeyPressed(VK_DOWN)) {
		picture->GetTransform()->Stretch(0, -deltaTime);
	}

	if(APP->Input()->IsKeyPressed(VK_RIGHT)) {
		picture->GetTransform()->Stretch(deltaTime, 0);
	} 
	else if(APP->Input()->IsKeyPressed(VK_LEFT)) {
		picture->GetTransform()->Stretch(-deltaTime, 0);
	}

	//HSVPostProcess* poster = (HSVPostProcess*)APP->Graphics()->postProcesses[0];
	//if(APP->Input()->IsKeyPressed(VK_UP) /* && poster->contrast < 1.0f*/) {
	//	poster->contrast += 0.5f * deltaTime;
	//} 
	//else if(APP->Input()->IsKeyPressed(VK_DOWN) /* && poster->contrast > -1.0f*/) {
	//	poster->contrast -= 0.5f * deltaTime;
	//}

	//if(APP->Input()->KeyJustPressed(VK_SPACE)) {
	//	poster->contrast = 0;
	//}
}

void Game::Draw() {
	sampleScene->Draw();
}
