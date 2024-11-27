#include "Game.h"
#include "Application.h"
#include "TextBox.h"

GameObject* text;
Game::Game(AssetManager* assets) {
	// load assets
	assets->arial = assets->LoadFont(L"Arial.spritefont");
	assets->testImage = std::make_shared<Sprite>(L"temp sprite.png");

	// set up game objects
	sampleScene = new Scene(10, Color(0.1f, 0.1f, 0.1f));

	testObject = new GameObject(sampleScene, RenderMode::Translucent, -1, Color(0, 0.8f, 0.5f, 0.7f));
	testObject->pixelShader = assets->circlePS;

	picture = new GameObject(sampleScene, false, 1, assets->testImage);
	picture->GetTransform()->SetWidth(2.0f);
	picture->GetTransform()->SetPosition(Vector2(0, -2));

	text = new TextBox(sampleScene, 0, "Hello is there text here? what about there?", assets->arial, Color::White);
	text->GetTransform()->SetScale(3, 0.5f);
	GameObject* textBack = new GameObject(sampleScene, RenderMode::Opaque, 1, Color::Cyan);
	textBack->SetParent(text);
}

Game::~Game() {
	delete sampleScene;
}

void Game::Update(float deltaTime) {
	//text->GetTransform()->SetPosition(APP->Input()->GetMousePosition(sampleScene->GetCamera()));
	testObject->GetTransform()->SetPosition(APP->Input()->GetMousePosition(sampleScene->GetCamera()));
}

void Game::Draw() {
	sampleScene->Draw();
}
