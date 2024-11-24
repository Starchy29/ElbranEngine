#pragma once
#include "Scene.h"
#include "AssetManager.h"

class Game
{
	friend class Application;

public:
	void Update(float deltaTime);
	void Draw();

	// prevent copying
	Game(const Game&) = delete;
	void operator=(const Game&) = delete;

private:
	GameObject* testObject;
	GameObject* picture;
	Scene* sampleScene;

	Game(AssetManager* assets);
	~Game();
};

