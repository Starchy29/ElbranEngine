#pragma once
class Game
{
public:
	Game() {}
	void Initialize();
	void Release();

	void Update(float deltaTime);
	void Draw();
};

