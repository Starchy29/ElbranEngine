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

	//APP->Graphics()->postProcesses.push_back(new HSVPostProcess(0.3, -1, -0.2));
	//APP->Graphics()->postProcesses.push_back(new BloomPostProcess(0.7f, 50));
	//APP->Graphics()->postProcesses.push_back(new BlurPostProcess(10));

	testObject = new GameObject(-20, Color(0.3f, 1.f, 0.5f, 0.7f), true);
	sampleScene->Add(testObject);

	//picture = new GameObject(1, assets->testImage, false);
	RepeatRenderer* repeater = new RepeatRenderer(assets->testImage, Vector2(1, 1));
	//repeater->flipX = true;
	//repeater->flipY = true;
	picture = new GameObject(0, RenderMode::Opaque, repeater);
	picture->GetTransform()->SetScale(3, 4);
	sampleScene->Add(picture);

	//GameObject* photo = new GameObject(0, std::make_shared<Sprite>(L"nature.jpg"), false);
	//sampleScene->Add(photo);
	//photo->GetTransform()->GrowWidth(9);
}

Game::~Game() {
	delete sampleScene;
}

void Game::Update(float deltaTime) {
	testObject->GetTransform()->SetPosition(APP->Input()->GetMousePosition(sampleScene->GetCamera()));
	sampleScene->Update(deltaTime);
}

void Game::Draw() {
	sampleScene->Draw();
}
