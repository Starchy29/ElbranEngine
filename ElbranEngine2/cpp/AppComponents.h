#pragma once

class GraphicsAPI;
class SoundMixer;
class InputManager;
class AssetContainer;
class MemoryArena;
class Game;

struct AppComponents {
	Game* game;
	const GraphicsAPI* graphics;
	SoundMixer* audio;
	InputManager* input;
	AssetContainer* assets;
	const MemoryArena* arena;
};

struct DrawComponents {
	GraphicsAPI* graphics;
	const AssetContainer* assets;
	const MemoryArena* arena;
};