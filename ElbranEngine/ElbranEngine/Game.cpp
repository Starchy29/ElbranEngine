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
#include "RepeatRenderer.h"

Game::Game(const AssetManager* assets) {
	sampleScene = new Scene(10, Color(0.1f, 0.1f, 0.1f));

	APP->Graphics()->postProcesses.push_back(new HSVPostProcess(0, 0, 0));
	//APP->Graphics()->postProcesses.push_back(new BloomPostProcess(0.7f, 50));
	//APP->Graphics()->postProcesses.push_back(new BlurPostProcess(10));

	testObject = new GameObject(-20, Color(0.3f, 1.f, 0.5f, 0.7f), true);
	sampleScene->Add(testObject);

	//picture = new GameObject(1, assets->testImage, false);
	//RepeatRenderer* repeater = new RepeatRenderer(assets->testImage, Vector2(1, 1));
	//repeater->flipX = true;
	//repeater->flipY = true;
	//picture = new GameObject(0, RenderMode::Opaque, repeater);
	//picture->GetTransform()->SetScale(3, 4);
	//sampleScene->Add(picture);

	GameObject* photo = new GameObject(0, std::make_shared<Sprite>(L"apple.jpeg"), false);
	sampleScene->Add(photo);
	photo->GetTransform()->GrowWidth(7.3f);
}

Game::~Game() {
	delete sampleScene;
}

void Game::Update(float deltaTime) {
	//sampleScene->Update(deltaTime);
	testObject->GetTransform()->SetPosition(APP->Input()->GetMousePosition(sampleScene->GetCamera()));

	HSVPostProcess* poster = (HSVPostProcess*)APP->Graphics()->postProcesses[0];
	if(APP->Input()->IsKeyPressed(VK_UP) && poster->contrast < 1.0f) {
		poster->contrast += 0.5f * deltaTime;
	} 
	else if(APP->Input()->IsKeyPressed(VK_DOWN) && poster->contrast > -1.0f) {
		poster->contrast -= 0.5f * deltaTime;
	}

	if(APP->Input()->KeyJustPressed(VK_SPACE)) {
		poster->contrast = 0;
	}
}

void Game::Draw() {
	sampleScene->Draw();
}
