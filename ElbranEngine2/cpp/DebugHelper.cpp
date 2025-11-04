#if defined(DEBUG) | defined(_DEBUG)
/*#include "DebugHelper.h"
#include "Scene.h"

void DebugHelper::ClearAll() {
	for(int i = 0; i < SPECIAL_SHAPE_COUNT; i++) {
		dots[i].color.alpha = 0.f;
		lines[i].color.alpha = 0.f;
		boxes[i].color.alpha = 0.f;
	}
	for(int i = 0; i < BULK_SHAPE_COUNT; i++) {
		shapes[i].color.alpha = 0.f;
	}
}

void DebugHelper::AddDot(Vector2 position, Color color, float size) {
	if(shapes[numBulk].transform == nullptr) {
		debugScene->AddRenderer(&shapes[numBulk], true);
		shapes[numBulk].shape = ShapeRenderer::Shape::Circle;
	}
	shapes[numBulk].transform->position = position;
	shapes[numBulk].color = color;
	shapes[numBulk].transform->scale = Vector2(size, size);
	numBulk++;
}

void DebugHelper::PlaceDot(uint8_t index, Vector2 position, Color color, float size) {
	if(dots[index].transform == nullptr) {
		debugScene->AddRenderer(&dots[index], true);
		dots[index].shape = ShapeRenderer::Shape::Circle;
	}
	dots[index].transform->position = position;
	dots[index].color = color;
	dots[index].transform->scale = Vector2(size, size);
}

void DebugHelper::AddLine(Vector2 start, Vector2 end, Color color, float width) {
	if(shapes[numBulk].transform == nullptr) {
		debugScene->AddRenderer(&shapes[numBulk], true);
		shapes[numBulk].shape = ShapeRenderer::Shape::Square;
	}
	Vector2 line = end - start;
	shapes[numBulk].transform->position = Tween::Lerp(start, end, 0.5f);
	shapes[numBulk].color = color;
	shapes[numBulk].transform->scale = Vector2(line.Length(), width);
	shapes[numBulk].transform->rotation = line.Angle();
	numBulk++;
}

void DebugHelper::PlaceLine(uint8_t index, Vector2 start, Vector2 end, Color color, float width) {
	if(lines[index].transform == nullptr) {
		debugScene->AddRenderer(&lines[index], true);
		lines[index].shape = ShapeRenderer::Shape::Square;
	}
	Vector2 line = end - start;
	lines[index].transform->position = Tween::Lerp(start, end, 0.5f);
	lines[index].color = color;
	lines[index].transform->scale = Vector2(line.Length(), width);
	lines[index].transform->rotation = line.Angle();
}

void DebugHelper::AddBox(AlignedRect area, Color color) {
	if(shapes[numBulk].transform == nullptr) {
		debugScene->AddRenderer(&shapes[numBulk], true);
		shapes[numBulk].shape = ShapeRenderer::Shape::Square;
	}
	shapes[numBulk].transform->position = area.Center();
	shapes[numBulk].color = color;
	shapes[numBulk].transform->scale = area.Size();
	numBulk++;
}

void DebugHelper::PlaceBox(uint8_t index, AlignedRect area, Color color) {
	if(boxes[index].transform == nullptr) {
		debugScene->AddRenderer(&boxes[index], true);
		boxes[index].shape = ShapeRenderer::Shape::Square;
	}
	boxes[index].transform->position = area.Center();
	boxes[index].color = color;
	boxes[index].transform->scale = area.Size();
}*/
#endif