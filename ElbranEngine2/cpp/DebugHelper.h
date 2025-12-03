#if defined(DEBUG) | defined(_DEBUG)
#pragma once
#include "Math.h"
#include "Common.h"

struct RenderGroup;
struct Renderer;
#define SHOW_FPS true

class DebugHelper {
public:
	RenderGroup* debugScene;

	DebugHelper() = default;

	void ClearAll();
	void PlaceDot(uint8_t index, Vector2 position, Color color = Color::Red, float size = 0.1f);
	void PlaceLine(uint8_t index, Vector2 start, Vector2 end, Color color = Color::Red, float width = 0.1f);
	void PlaceBox(uint8_t index, AlignedRect area, Color color = Color(1.0f, 0.0f, 0.0f, 0.5f));
	void AddDot(Vector2 position, Color color = Color::Red, float size = 0.1f);
	void AddLine(Vector2 start, Vector2 end, Color color = Color::Red, float width = 0.1f);
	void AddBox(AlignedRect area, Color color = Color(1.0f, 0.0f, 0.0f, 0.5f));

private:
	static constexpr uint8_t NUM_DYNAMIC = 5;
	static constexpr uint8_t NUM_STATIC = 10;
	Renderer* dynamics[NUM_DYNAMIC];
	Renderer* statics[NUM_STATIC];
	uint8_t activeStatics;
};
#endif