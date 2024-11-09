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
	sampleScene = new Scene(10, Color(0.2f, 0.2f, 0.2f));

	testObject = new GameObject(Color(0, 1, 1, 0.75f), true);
	testObject->pixelShader = Assets->circlePS;
	
	GameObject* other = new GameObject(Assets->testImage, true);
	other->colorTint = Color(1, 1, 1, 0.25);

	other->SetZ(2);
	testObject->SetZ(1);

	sampleScene->AddObject(testObject);
	sampleScene->AddObject(other);

	return S_OK;
}

void NewGame::Update(float deltaTime) {
	testObject->GetTransform()->SetPosition(Inputs->GetMousePosition(sampleScene->GetCamera()));
}

void NewGame::Draw() {
	sampleScene->Draw();
}
