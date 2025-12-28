#pragma once
#include "GraphicsData.h"
#include "Common.h"

class GraphicsAPI;
class AssetContainer;
class MemoryArena;

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
	const Matrix* worldMatrix;

	union {
		struct {
			Color color;
			PrimitiveShape shape;
		} shapeData;

		struct {
			const Sprite* sprite;
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
			VerticalAlignment verticalAlignment;
			Color color;
			float padding;

			// changing these 4 requires calling UpdateTextMesh() to see the changes
			const Font* font;
			const char* text;
			float lineSpacing;
			HorizontalAlignment horizontalAlignment;

			float blockAspectRatio; // this is determined during mesh generation
		} textData;

		struct {
			EditBuffer particleBuffer;
			const SpriteSheet* sprites;
			Color tint;
			uint16_t maxParticles;
			float animationFPS;
			bool scaleWithParent;
			bool applyLights;
			bool blendAdditive;
		} particleData;

		struct {
			const Sprite* sprite;
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
	
	void Draw(GraphicsAPI*, const AssetContainer*);
	bool IsTranslucent() const;
	void Release(const GraphicsAPI*);

	void InitShape(PrimitiveShape shape, Color color);
	void InitSprite(const Sprite* sprite);
	void InitAtlas(const SpriteSheet* atlas);
	void InitPattern(const Sprite* sprite);
	void InitLight(Color color, float radius);
	void InitText(const GraphicsAPI*, MemoryArena*, const char* text, const Font* font, HorizontalAlignment horizontalAlignment = HorizontalAlignment::Center, float lineSpacing = 0.0f);
	void InitParticles(const GraphicsAPI*, uint16_t maxParticles, const SpriteSheet* animation, float animationFPS);

	void UpdateTextMesh(const GraphicsAPI*, MemoryArena*); // for text renderers
	void ClearParticles(GraphicsAPI*, const AssetContainer*); // for particle renderers
};