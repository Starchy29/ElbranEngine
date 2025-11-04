#pragma once
#include "Transform.h"
#include "Buffers.h"
#include "Common.h"

class GraphicsAPI;
class AssetContainer;

enum class PrimitiveShape : uint8_t {
	Square,
	Circle,
	Triangle
};

enum class HorizontalAlignment : uint8_t {
	Left,
	Right,
	Center
};

enum class VerticalAlignment : uint8_t {
	Top,
	Bottom,
	Center
};

struct Renderer {
	enum class Type {
		None,
		Shape,
		Sprite,
		Atlas,
		Pattern,
		Text,
		Particles,
		Light
	} type;

	Transform* transform;
	const Matrix* worldMatrix; // delete this?

	union {
		struct {
			Color color;
			PrimitiveShape shape;
		} shapeData;

		struct {
			const Texture2D* sprite;
			Color tint;
			bool lit;
			bool flipX;
			bool flipY;
		} spriteData;

		struct {
			const SpriteSheet* atlas;
			Color tint;
			uint8_t row;
			uint8_t col;
			bool lit;
			bool flipX;
			bool flipY;
		} atlasData;

		struct {
			Mesh textMesh;
			const Font* font;
			const char* text;
			Color color;
			float lineSpacing;
			float padding;
			float blockAspectRatio;
			HorizontalAlignment horizontalAlignment;
			VerticalAlignment verticalAlignment;
		} textData;

		struct {

		} particleData;

		struct {
			const Texture2D* sprite;
			Color tint;
			Vector2 origin;
			Vector2 blockSize;
			bool lit;
			bool flipX;
			bool flipY;
		} patternData;

		struct {
			Color color;
			float radius;
			float brightness;
			float coneSize;
		} lightData;
	};

	bool hidden;
	bool translucent;
	
	void Draw(GraphicsAPI* graphics, AssetContainer* assets);

	void UpdateTextMesh(); // for text renderers
	void ClearParticles(); // for particle renderers

	void InitShape(PrimitiveShape shape, Color color);
	void InitSprite(const Texture2D* sprite);
	void InitAtlas(const SpriteSheet* atlas);
	void InitPattern(const Texture2D* sprite);
	void InitLight(Color color, float radius);
};