#pragma once
#include "Scene.h"
#include "AppPointer.h"

class Game
{
public:
	Game() {}
	void Initialize();
	void Release();

	void Update(float deltaTime);
	void Draw();

private:
	Scene sampleScene;
};

