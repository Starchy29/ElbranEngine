#if defined(DEBUG) | defined(_DEBUG)
#include "DebugHelper.h"
#include "RenderGroup.h"

void DebugHelper::ClearAll() {
	for(int i = 0; i < NUM_DYNAMIC; i++) {
		dynamics[i]->hidden = true;
	}
	for(int i = 0; i < NUM_STATIC; i++) {
		statics[i]->hidden = true;
	}
	activeStatics = 0;
}

void DebugHelper::AddDot(Vector2 position, Color color, float size) {
	if(activeStatics >= NUM_STATIC) return;
	if(!statics[activeStatics]) statics[activeStatics] = debugScene->ReserveRenderer();
	statics[activeStatics]->InitShape(PrimitiveShape::Circle, color);
	statics[activeStatics]->transform->position = position;
	statics[activeStatics]->transform->scale = Vector2(size, size);
	activeStatics++;
}

void DebugHelper::PlaceDot(uint8_t index, Vector2 position, Color color, float size) {
	if(!dynamics[index]) dynamics[index] = debugScene->ReserveRenderer();
	dynamics[index]->InitShape(PrimitiveShape::Circle, color);
	dynamics[index]->transform->scale = Vector2(size, size);
	dynamics[index]->transform->position = position;
}

void DebugHelper::AddLine(Vector2 start, Vector2 end, Color color, float width) {
	if(activeStatics >= NUM_STATIC) return;
	if(!statics[activeStatics]) statics[activeStatics] = debugScene->ReserveRenderer();
	Vector2 line = end - start;
	statics[activeStatics]->InitShape(PrimitiveShape::Square, color);
	statics[activeStatics]->transform->position = Tween::Lerp(start, end, 0.5f);
	statics[activeStatics]->transform->scale = Vector2(line.Length(), width);
	statics[activeStatics]->transform->rotation = line.Angle();
	activeStatics++;
}

void DebugHelper::PlaceLine(uint8_t index, Vector2 start, Vector2 end, Color color, float width) {
	if(!dynamics[index]) dynamics[index] = debugScene->ReserveRenderer();
	Vector2 line = end - start;
	dynamics[index]->InitShape(PrimitiveShape::Square, color);
	dynamics[index]->transform->position = Tween::Lerp(start, end, 0.5f);
	dynamics[index]->transform->scale = Vector2(line.Length(), width);
	dynamics[index]->transform->rotation = line.Angle();
}

void DebugHelper::AddBox(AlignedRect area, Color color) {
	if(activeStatics >= NUM_STATIC) return;
	if(!statics[activeStatics]) statics[activeStatics] = debugScene->ReserveRenderer();
	statics[activeStatics]->InitShape(PrimitiveShape::Square, color);
	statics[activeStatics]->transform->position = area.Center();
	statics[activeStatics]->transform->scale = area.Size();
	activeStatics++;
}

void DebugHelper::PlaceBox(uint8_t index, AlignedRect area, Color color) {
	if(!dynamics[index]) dynamics[index] = debugScene->ReserveRenderer();
	dynamics[index]->InitShape(PrimitiveShape::Square, color);
	dynamics[index]->transform->position = area.Center();
	dynamics[index]->transform->scale = area.Size();
}
#endif