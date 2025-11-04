#pragma once
class Game
{
public:
	Game() = default;
	void Initialize();
	void Release();

	void Update(float deltaTime);
	void Draw();
};

