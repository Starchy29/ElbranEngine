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
#include "ComputeShader.h"
#include "LightRenderer.h"
#include <DirectXMath.h>

#include "SoundEffect.h"
SoundEffect* sfx;

Game::Game(const AssetManager* assets) {
	sampleScene = new Scene(10, Color(0.1f, 0.1f, 0.1f));

	menu = std::make_shared<MusicTrack>(L"Menu Song.wav", 0.5f);
	//testSound->Play();

	sfx = new SoundEffect(L"water plunk.wav");

	//APP->Input()->SetVibration(0, 1.0f);
	
	battle = std::make_shared<MusicTrack>(L"Battle music.wav");
	//APP->Audio()->StartSong(menu);
	//battle->Play();

	ParticleRenderer* spawner = new ParticleRenderer(300, 0.8f, true, std::make_shared<Sprite>(L"spark.png") /*std::make_shared<SpriteAtlas>(L"animation.png", 3, 3, 8)*/);
	GameObject* particles = new GameObject(1, spawner, true);
	sampleScene->Add(particles);
	particles->GetTransform()->SetWidth(1);

	spawner->screenSpace = true;

	spawner->width = 0.5f;
	spawner->totalGrowth = -0.25f;

	spawner->speed = -1.0f;
	spawner->moveStyle = ParticleMovement::Outward;
	
	spawner->faceMoveDirection = true;
	spawner->SetSpawnRate(30.f);
	//spawner->scaleWithParent = true;
	//spawner->fadeDuration = 0.8f;

	//APP->Graphics()->postProcesses.push_back(new HSVPostProcess(0, -1, 0));
	APP->Graphics()->postProcesses.push_back(new BloomPostProcess(0.5f, 50));
	//APP->Graphics()->postProcesses.push_back(new BlurPostProcess(10));

	//testObject = new GameObject(1.5f, 1.f, Color::White);
	testObject = new GameObject(0, Color(0.2f, 0.8f, 0.5f, 0.7f), ColorRenderer::Triangle);
	sampleScene->Add(testObject);
	//testObject->GetTransform()->Scale(0.6f);

	//StretchRenderer* repeater = new StretchRenderer(std::make_shared<Sprite>(L"stretcher.png"), Vector2(2, 2), Vector2(0.5f, 0.167f), Vector2(0.833f, 0.833f));
	//picture = new GameObject(0, RenderMode::Opaque, repeater);
	//picture->GetTransform()->SetScale(3, 2);
	//sampleScene->Add(picture);
	//repeater->useLights = true;

	GameObject* photo = new GameObject(10, std::make_shared<Sprite>(L"apple.jpeg"), false);
	sampleScene->Add(photo);
	photo->GetTransform()->GrowWidth(7.3f);
	//photo->GetRenderer<IRenderer>()->screenSpace = true;

	photo->GetRenderer<SpriteRenderer>()->useLights = true;
	sampleScene->ambientLight = Color::Black;
}

Game::~Game() {
	delete sampleScene;
	delete sfx;
}

void Game::Update(float deltaTime) {
	sampleScene->Update(deltaTime);
	testObject->GetTransform()->SetPosition(APP->Input()->GetMousePosition(sampleScene->GetCamera()));

	if(APP->Input()->IsKeyPressed(VK_UP)) {
		sampleScene->GetCamera()->Translate(Vector2(0, 2.0 * deltaTime));
	}
	if(APP->Input()->IsKeyPressed(VK_DOWN)) {
		sampleScene->GetCamera()->Translate(Vector2(0, -2.0 * deltaTime));
	}
	if(APP->Input()->IsKeyPressed(VK_LEFT)) {
		sampleScene->GetCamera()->Translate(Vector2(-2.0 * deltaTime, 0));
	}
	if(APP->Input()->IsKeyPressed(VK_RIGHT)) {
		sampleScene->GetCamera()->Translate(Vector2(2.0 * deltaTime, 0));
	}

	if(APP->Input()->KeyJustPressed(VK_MOUSE_LEFT)) {
		
		//APP->Audio()->StartSong(nullptr, 0.f, 0.f);
	}
	else if(APP->Input()->KeyJustPressed(VK_MOUSE_RIGHT)) {
		//testSound->Pause();
		sfx->Play(1.f, APP->RNG()->GenerateFloat(-0.5f, 0.5f));
	}

	if(APP->Input()->KeyJustPressed(VK_MOUSE_LEFT)) {
		const Random* rng = APP->RNG();
		GameObject* newLight = new GameObject(1.5f, 1.f, Color(rng->GenerateFloat(0.f, 1.f), rng->GenerateFloat(0.f, 1.f), rng->GenerateFloat(0.f, 1.f)));
		sampleScene->Add(newLight);
		newLight->GetTransform()->SetPosition(APP->Input()->GetMousePosition(sampleScene->GetCamera()));
		//newLight->GetTransform()->SetRotation(rng->GenerateFloat(0.f, 6.f));
		//newLight->GetRenderer<LightRenderer>()->coneSize = 1.8f;
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
