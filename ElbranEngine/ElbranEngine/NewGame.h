#pragma once
#include "DXGame.h"

#define GameInstance NewGame::GetInstance()
#define START_ASPECT_RATIO 16.0f / 9.0f
#define START_WINDOW_WIDTH 960

class NewGame : public DXGame
{
public:
	static NewGame* GetInstance();
	static HRESULT Initialize(HINSTANCE hInst);

	NewGame(HINSTANCE hInst);
	~NewGame();

private:
	static NewGame* instance;

	HRESULT LoadAssets() override;
	void Update(float deltaTime) override;
	void Draw() override;

	// temp asset storage
	GameObject* testObject;
	Scene* sampleScene;
};
