#pragma once
#include "Scene.h"

class Game
{
public:
	Game();
	~Game();

	void Update(float deltaTime);
	void Draw();

private:
	Scene* testScene;
};

