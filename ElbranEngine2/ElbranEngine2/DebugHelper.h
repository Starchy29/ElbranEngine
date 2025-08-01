#if defined(DEBUG) | defined(_DEBUG)
#pragma once
#include "Math.h"
#include "ShapeRenderer.h"
#include "FixedList.h"

class Scene;
#define BULK_SHAPE_COUNT 20
#define SPECIAL_SHAPE_COUNT 5

class DebugHelper {
public:
	Scene* debugScene;

	void ClearAll();
	void AddDot(Vector2 position, Color color = Color::Red, float size = 0.1f);
	void PlaceDot(unsigned int index, Vector2 position, Color color = Color::Red, float size = 0.1f);
	void AddLine(Vector2 start, Vector2 end, Color color = Color::Red, float width = 0.1f);
	void PlaceLine(unsigned int index, Vector2 start, Vector2 end, Color color = Color::Red, float width = 0.1f);
	void AddBox(AlignedRect area, Color color = Color(1.0f, 0.0f, 0.0f, 0.5f));
	void PlaceBox(unsigned int index, AlignedRect area, Color color = Color(1.0f, 0.0f, 0.0f, 0.5f));

private:
	unsigned int numBulk = 0;
	ShapeRenderer dots[SPECIAL_SHAPE_COUNT];
	ShapeRenderer lines[SPECIAL_SHAPE_COUNT];
	ShapeRenderer boxes[SPECIAL_SHAPE_COUNT];
	ShapeRenderer shapes[BULK_SHAPE_COUNT];
};
#endif