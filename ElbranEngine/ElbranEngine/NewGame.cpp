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

	testObject = new GameObject(sampleScene, true, Color(0, 0.8f, 0.5f, 0.75f));
	testObject->pixelShader = Assets->circlePS;
	
	GameObject* picture = new GameObject(sampleScene, false, Assets->testImage);
	picture->colorTint = Color(1, 1, 1);
	picture->GetTransform()->SetScale(2, 2);

	picture->SetZ(1);
	picture->GetTransform()->SetPosition(DirectX::XMFLOAT2(0.5f, 0.5f));

	return S_OK;
}

void NewGame::Update(float deltaTime) {
	testObject->GetTransform()->SetPosition(Inputs->GetMousePosition(sampleScene->GetCamera()));
	testObject->colorTint.alpha += Inputs->GetMouseWheelSpin() / 20.0f;
}

void NewGame::Draw() {
	sampleScene->Draw();
}
