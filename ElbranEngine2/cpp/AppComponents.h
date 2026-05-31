#pragma once

class GraphicsAPI;
class SoundMixer;
class InputManager;
class AssetContainer;
class MemoryArena;

struct AppComponents {
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