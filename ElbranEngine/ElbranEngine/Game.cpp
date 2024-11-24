#include "Game.h"
#include "Application.h"

Game::Game(AssetManager* assets) {
	// load assets
	assets->testImage = std::make_shared<Sprite>(L"temp sprite.png");

	// set up game objects
	sampleScene = new Scene(10, Color(0.1f, 0.1f, 0.1f));

	testObject = new GameObject(sampleScene, true, 0, Color(0, 0.8f, 0.5f, 0.75f));
	testObject->pixelShader = assets->circlePS;

	picture = new GameObject(sampleScene, false, 1, assets->testImage);
	picture->GetTransform()->SetWidth(2.0f);
	picture->GetTransform()->SetPosition(Vector2(-1.5f, -1.5f));

	GameObject* child = new GameObject(sampleScene, false, 0, Color::Red);
	child->SetParent(picture);
	child->GetTransform()->SetPosition(Vector2(-0.5f, 0.5f));
	child->GetTransform()->Scale(0.3f);

	GameObject* grandchild = new GameObject(sampleScene, false, 0, Color::Green);
	grandchild->SetParent(child);
	grandchild->GetTransform()->SetPosition(Vector2(1.3f, 1.5f));
	grandchild->GetTransform()->Scale(1.2f);

	GameObject* copy = picture->Clone();
	copy->GetTransform()->SetPosition(Vector2(2, 0));
}

Game::~Game() {
	delete sampleScene;
}

void Game::Update(float deltaTime) {
	testObject->GetTransform()->SetPosition(APP->Input()->GetMousePosition(sampleScene->GetCamera()));
}

void Game::Draw() {
	sampleScene->Draw();
}
