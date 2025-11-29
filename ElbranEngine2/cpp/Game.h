#pragma once
#define GAME_TITLE L"Elbran Engine"
#define MAX_FPS 240.f

class GraphicsAPI;

class Game
{
public:
	Game() = default;
	void Initialize();
	void Release(GraphicsAPI*);

	void Update(float deltaTime);
	void Draw();
};

