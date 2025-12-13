#pragma once
#define GAME_TITLE L"Elbran Engine"
#define MAX_FPS 240.f

class GraphicsAPI;
class AssetContainer;
class InputManager;
class SoundMixer;
class MemoryArena;
class Application;

class Game {
public:
	Game() = default;
	void Initialize(Application*);
	void Release(GraphicsAPI*);

	void Update(Application*, float deltaTime);
	void Draw(GraphicsAPI*, const AssetContainer*, MemoryArena*);
};

