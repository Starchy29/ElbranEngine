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

typedef ID3D11Buffer Buffer;
typedef ID3D11Texture2D TextureData;
typedef ID3D11SamplerState SamplerState;
typedef ID3D11ShaderResourceView ShaderResourceView;
typedef ID3D11RenderTargetView RenderTargetView;
typedef ID3D11UnorderedAccessView UnorderedAccessView;
typedef ID3D11VertexShader VSData;
typedef ID3D11GeometryShader GSData;
typedef ID3D11PixelShader PSData;
typedef ID3D11ComputeShader CSData;
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

struct Sampler {
	SamplerState* state;
};

struct Texture2D {
	TextureData* data;
	ShaderResourceView* inputView;
	uint32_t width;
	uint32_t height;
	bool translucent;
	inline float AspectRatio() const { return (float)width / height; }
};

struct SpriteSheet {
	Texture2D texture;
	uint8_t rows;
	uint8_t cols;
	uint8_t spriteCount;

	SpriteSheet() = default;
	inline SpriteSheet(Texture2D texture) : SpriteSheet(texture, 1, 1, 1) {}
	inline SpriteSheet(Texture2D texture, uint8_t rows, uint8_t cols, uint8_t spriteCount) {
		this->texture = texture;
		this->rows = rows;
		this->cols = cols;
		this->spriteCount = spriteCount;
	}
	inline float SpriteAspectRatio() const { return texture.AspectRatio() * rows / cols; }
};

struct RenderTarget : Texture2D {
	RenderTargetView* outputView;
};

struct ComputeTexture : Texture2D {
	UnorderedAccessView* outputView;
};

struct Mesh {
	uint16_t indexCount;
	Buffer* vertices;
	Buffer* indices;
};

struct ConstantBuffer {
	Buffer* data;
};

struct ArrayBuffer {
	Buffer* buffer;
	ShaderResourceView* view;
};

// a gpu-writable buffer that gets bound to the render pipeline
struct EditBuffer : ArrayBuffer {
	UnorderedAccessView* computeView;
};

// a gpu-writable buffer that the cpu can read back from
struct OutputBuffer {
	Buffer* gpuBuffer;
	Buffer* cpuBuffer;
	UnorderedAccessView* view;
	size_t byteLength;
};

struct VertexShader {
	VSData* shader;
	ConstantBuffer constants;
};

struct GeometryShader {
	GSData* shader;
	ConstantBuffer constants;
};

struct PixelShader {
	PSData* shader;
	ConstantBuffer constants;
};

struct ComputeShader {
	CSData* shader;
	ConstantBuffer constants;
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