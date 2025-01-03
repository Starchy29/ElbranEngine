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

Game::Game(const AssetManager* assets) {
	sampleScene = new Scene(10, Color(0.1f, 0.1f, 0.1f));

	//APP->Graphics()->postProcesses.push_back(new HSVPostProcess(0, -1, 0));
	//APP->Graphics()->postProcesses.push_back(new BloomPostProcess(0.7f, 50));
	//APP->Graphics()->postProcesses.push_back(new BlurPostProcess(10));

	testObject = new GameObject(-20, 2, 2, Color::White);
	sampleScene->Add(testObject);
	//testObject->GetTransform()->Scale(0.6f);

	//StretchRenderer* repeater = new StretchRenderer(std::make_shared<Sprite>(L"stretcher.png"), Vector2(2, 2), Vector2(0.5f, 0.167f), Vector2(0.833f, 0.833f));
	//picture = new GameObject(0, RenderMode::Opaque, repeater);
	//picture->GetTransform()->SetScale(3, 2);
	//sampleScene->Add(picture);

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
	//sampleScene->Update(deltaTime);
	testObject->GetTransform()->SetPosition(APP->Input()->GetMousePosition(sampleScene->GetCamera()));

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
