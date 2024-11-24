#pragma once
#include "Scene.h"
#include "AssetManager.h"

class NewGame
{
	friend class Application;

public:
	void Update(float deltaTime);
	void Draw();

	// prevent copying
	NewGame(const NewGame&) = delete;
	void operator=(const NewGame&) = delete;

private:
	GameObject* testObject;
	GameObject* picture;
	Scene* sampleScene;

	NewGame(AssetManager* assets);
	~NewGame();
};

