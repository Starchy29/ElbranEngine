#include "NewGame.h"
#include "AssetManager.h"
#include "InputManager.h"

NewGame* NewGame::instance;

NewGame* NewGame::GetInstance() {
	return instance;
}

HRESULT NewGame::Initialize(HINSTANCE hInst) {
	instance = new NewGame(hInst);

	HRESULT hRes = instance->InitWindow();
	if (FAILED(hRes)) return hRes;

	hRes = instance->InitDirectX();
	if (FAILED(hRes)) return hRes;

	hRes = instance->LoadAssets();
	if (FAILED(hRes)) return hRes;

	Inputs->Initialize(instance->windowHandle);

	return S_OK;
}

NewGame::NewGame(HINSTANCE hInst) : DXGame(hInst) {}

NewGame::~NewGame() {
	delete sampleScene;
}

HRESULT NewGame::LoadAssets() {
	HRESULT hr = DXGame::LoadAssets();
	if(FAILED(hr)) {
		return hr;
	}

	// sample assets
	Assets->testImage = std::make_shared<Sprite>(L"temp sprite.png");
	sampleScene = new Scene(10, Color(0.1f, 0.1f, 0.1f));

	testObject = new GameObject(sampleScene, true, 0, Color(0, 0.8f, 0.5f, 0.75f));
	testObject->pixelShader = Assets->circlePS;
	
	picture = new GameObject(sampleScene, false, 1, Assets->testImage);
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

	return S_OK;
}

void NewGame::Update(float deltaTime) {
	testObject->GetTransform()->SetPosition(Inputs->GetMousePosition(sampleScene->GetCamera()));
}

void NewGame::Draw() {
	sampleScene->Draw();
}
