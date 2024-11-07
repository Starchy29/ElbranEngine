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
	LoadTexture(L"temp sprite.png", Assets->testImage.GetAddressOf());
	sampleScene = new Scene(10, Color(0.2f, 0.2f, 0.2f));

	DirectX::XMFLOAT2 dims = sampleScene->GetCamera()->GetWorldDimensions();

	testObject = new GameObject(Color(0, 1, 1, 0.5f));
	testObject->pixelShader = Assets->circlePS;

	sampleScene->AddObject(testObject);
	GameObject* other = new GameObject(Color::Yellow);
	sampleScene->AddObject(other);

	other->GetTransform()->SetZ(CAMERA_MAX_Z - 1);
	testObject->GetTransform()->SetZ(1);
	//testObject->colorTint = Color::Red;
	//testObject->GetTransform()->SetScale(dims.x + 3, dims.y + 3);
	//testObject->GetTransform()->SetZ(1);
	//testObject->GetTransform()->SetPosition(DirectX::XMFLOAT2(-10.0f, 0.0f));

	return S_OK;
}

void NewGame::Update(float deltaTime) {
	testObject->GetTransform()->SetPosition(Inputs->GetMousePosition(sampleScene->GetCamera()));
}

void NewGame::Draw() {
	sampleScene->Draw();
}
