#pragma once
#define GAME_TITLE L"Elbran Engine"
#define MAX_FPS 240.f

class Game
{
public:
	Game() = default;
	void Initialize();
	void Release();

	void Update(float deltaTime);
	void Draw();
};

