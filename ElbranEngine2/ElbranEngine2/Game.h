#pragma once
#include "Scene.h"
#include "Battle.h"

class Game
{
public:
	Battle battle;

	Game() {}
	void Initialize();
	void Release();

	void Update(float deltaTime);
	void Draw();

private:
	Scene sampleScene;
};

