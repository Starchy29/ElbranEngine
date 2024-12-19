#pragma once
#include "Scene.h"

class AssetManager;

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

	Game(const AssetManager* assets);
	~Game();
};

