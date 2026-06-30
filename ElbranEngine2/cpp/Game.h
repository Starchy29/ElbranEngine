#pragma once
#define GAME_TITLE L"Elbran Engine"
#define MAX_FPS 240.f

#include "AppComponents.h"
#include "PlayerControls.h"

class Game {
public:
	PlayerControls controls;

	Game() = default;
	void Initialize(AppComponents);
	void Release(const GraphicsAPI*);

	void Update(AppComponents, float deltaTime);
	void Draw(DrawComponents);
};

