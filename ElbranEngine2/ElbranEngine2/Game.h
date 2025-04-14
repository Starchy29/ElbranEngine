#pragma once

class Scene;

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

