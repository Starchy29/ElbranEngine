#pragma once
#include "Math.h"
#include "FixedMap.h"

#define OBJECT_CONSTANT_REGISTER 0 // shaders with per-object constant buffers should use register 0
#define MAX_POST_PROCESS_HELPER_TEXTURES 3
#define MAX_LIGHTS_ONSCREEN 16

struct Transform {
	Vector2 position;
	Vector2 scale;
	float rotation;
	float zOrder;
	Transform* parent;
};

struct Camera {
	Transform* transform;
	const Matrix* worldMatrix;
	float viewWidth;

	inline float GetViewHeight() const { return viewWidth / ASPECT_RATIO; }
	inline Vector2 GetWorldDimensions() const { return Vector2(viewWidth, GetViewHeight()); }
	inline AlignedRect GetViewArea() const {
		ASSERT(transform->rotation == 0.f)
		return AlignedRect(transform->position, GetWorldDimensions());
	}
};

// matches buffer in Lighting.hlsli
struct LightConstants {
	Color ambientLight;
	int32_t lightCount;
};

// matches struct in Lighting.hlsli
struct LightData {
	Vector2 worldPosition;
	Vector2 direction;

	Color color;
	float radius;
	float brightness;
	Vector2 coneEdge;
};

struct LightSource {
	Transform* transform;
	Matrix const* worldMatrix;

	Color color;
	float radius;
	float brightness;
	float coneSize;
};

#ifdef WINDOWS
struct ID3D11Buffer;
struct ID3D11Texture2D;
struct ID3D11SamplerState;
struct ID3D11ShaderResourceView;
struct ID3D11RenderTargetView;
struct ID3D11UnorderedAccessView;
struct ID3D11VertexShader;
struct ID3D11GeometryShader;
struct ID3D11PixelShader;
struct ID3D11ComputeShader;

typedef ID3D11Buffer GraphicsBuffer;
typedef ID3D11SamplerState Sampler;
typedef ID3D11VertexShader PlatformVS;
typedef ID3D11GeometryShader PlatformGS;
typedef ID3D11PixelShader PlatformPS;
typedef ID3D11ComputeShader PlatformCS;

struct Texture2D {
	ID3D11Texture2D* pixels;
	ID3D11ShaderResourceView* inputView;
	uint32_t width;
	uint32_t height;
};

struct Texture2DArray {
	ID3D11Texture2D* pixels;
	ID3D11ShaderResourceView* view;
	uint32_t elementWidth;
	uint32_t elementHeight;
};

struct RenderTarget {
	Texture2D texture;
	ID3D11RenderTargetView* outputView;
};

struct ComputeTexture {
	Texture2D texture;
	ID3D11UnorderedAccessView* outputView;
};

struct ArrayBuffer {
	GraphicsBuffer* buffer;
	ID3D11ShaderResourceView* view;
};

struct EditBuffer {
	ArrayBuffer arrayBuffer;
	ID3D11UnorderedAccessView* computeView;
};

struct OutputBuffer {
	GraphicsBuffer* gpuBuffer;
	GraphicsBuffer* cpuBuffer;
	ID3D11UnorderedAccessView* view;
	size_t byteLength;
};
#endif

enum class BlendState {
	None,
	AlphaBlend,
	Additive
};

enum class RenderPrimitive {
	Point,
	Line,
	Triangle
};

enum class ShaderStage {
	Vertex,
	Geometry,
	Pixel,
	Compute
};

enum class ShaderDataType {
	Structured,
	Int, Int2, Int3, Int4,
	UInt, UInt2, UInt3, UInt4,
	Float, Float2, Float3, Float4
};

struct Vertex {
	Vector2 position;
	Vector2 uv;
};

struct Sprite {
	Texture2D texture;
	bool translucent;
	inline float AspectRatio() const { return (float)texture.width / texture.height; }
};

struct SpriteSheet {
	Texture2DArray textures;
	uint8_t rows;
	uint8_t cols;
	bool translucent;
	inline uint16_t SpriteCount() const { return (uint16_t)rows * cols; } 
	inline float ElementAspectRatio() const { return (float)textures.elementWidth / textures.elementHeight; }
};

struct Mesh {
	uint16_t indexCount;
	GraphicsBuffer* vertices;
	GraphicsBuffer* indices;
};

struct VertexShader {
	PlatformVS* byteCode;
	GraphicsBuffer* constants;
};

struct GeometryShader {
	PlatformGS* byteCode;
	GraphicsBuffer* constants;
};

struct PixelShader {
	PlatformPS* byteCode;
	GraphicsBuffer* constants;
};

struct ComputeShader {
	PlatformCS* byteCode;
	GraphicsBuffer* constants;
	uint32_t xGroupSize;
	uint32_t yGroupSize;
	uint32_t zGroupSize;
};

struct Font {
	FixedMap<uint16_t, uint16_t> charToGlyphIndex;
	ArrayBuffer glyphCurves;
	ArrayBuffer firstCurveIndices;
	Vector2* glyphDimensions;
	float* glyphBaselines; // 0-1, where 0 is the bottom and 1 is the top
};