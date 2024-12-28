#pragma once
#include "Scene.h"

#define GAME_TITLE "Elbran Engine"
#define MAX_FPS 60
#define ASPECT_RATIO 16.f / 9.f

#if defined(DEBUG) | defined(_DEBUG)
#define USE_DEBUG_CONSOLE true
#define SHOW_FPS true
#endif

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

