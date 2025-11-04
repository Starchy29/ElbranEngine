#include "Renderer.h"
#include "GraphicsAPI.h"
#include "AssetContainer.h"
#include "ShaderConstants.h"

void Renderer::Draw(GraphicsAPI* graphics, AssetContainer* assets) {
	switch(type) {
	case Type::Shape: {
		CameraVSConstants vsInput = {};
		vsInput.uvScale = Vector2(1.f, 1.f);
		vsInput.worldTransform = worldMatrix->Transpose();
		graphics->SetVertexShader(&assets->cameraVS, &vsInput, sizeof(CameraVSConstants));
		graphics->SetPixelShader(shapeData.shape == PrimitiveShape::Circle ? &assets->circleFillPS : &assets->solidColorPS, &shapeData.color, sizeof(Color));
		graphics->DrawMesh(shapeData.shape == PrimitiveShape::Triangle ? &assets->unitTriangle : &assets->unitSquare);
	} break;

	case Type::Sprite: {
		CameraVSConstants vsInput;
		vsInput.worldTransform = worldMatrix->Transpose();
		vsInput.uvOffset = Vector2::Zero;
		vsInput.uvScale = Vector2(spriteData.flipX ? -1 : 1, spriteData.flipY ? -1 : 1); // assumes wrap enabled on sampling
		graphics->SetVertexShader(&assets->cameraVS, &vsInput, sizeof(CameraVSConstants));

		TexturePSConstants psInput;
		psInput.tint = spriteData.tint;
		psInput.lit = spriteData.lit;
		graphics->SetTexture(ShaderStage::Pixel, spriteData.sprite, 0);
		graphics->SetPixelShader(&assets->texturePS, &psInput, sizeof(TexturePSConstants));

		graphics->DrawMesh(&assets->unitSquare);
	} break;

	case Type::Atlas: {
		CameraVSConstants vsInput;
		vsInput.worldTransform = worldMatrix->Transpose();
		Vector2 tileScale = Vector2(1.0f / atlasData.atlas->cols, 1.0f / atlasData.atlas->rows);
		vsInput.uvScale = tileScale * Vector2(atlasData.flipX ? -1.0f : 1.0f, atlasData.flipY ? -1.0f : 1.0f); // assumes wrap enabled on sampling
		vsInput.uvOffset = Vector2(atlasData.col + (atlasData.flipX ? 1 : 0), atlasData.row + (atlasData.flipY ? 1 : 0)) * tileScale;
		graphics->SetVertexShader(&assets->cameraVS, &vsInput, sizeof(CameraVSConstants));

		TexturePSConstants psInput;
		psInput.tint = atlasData.tint;
		psInput.lit = atlasData.lit;
		graphics->SetTexture(ShaderStage::Pixel, &atlasData.atlas->texture, 0);
		graphics->SetPixelShader(&assets->texturePS, &psInput, sizeof(TexturePSConstants));

		graphics->DrawMesh(&assets->unitSquare);
	} break;

	case Type::Pattern: {
		Vector2 mid = *worldMatrix * Vector2::Zero;
		Vector2 right = *worldMatrix * Vector2(0.5f, 0.f);
		Vector2 up = *worldMatrix * Vector2(0.f, 0.5f);
		Vector2 globalScale = Vector2(right.x - mid.x, up.y - mid.y) * 2.f; // fails if this is rotated

		CameraVSConstants vsInput;
		vsInput.worldTransform = worldMatrix->Transpose();
		Vector2 flips = Vector2(patternData.flipX ? -1 : 1, patternData.flipY ? -1 : 1); // assumes wrap enabled on sampling
		vsInput.uvScale = globalScale / patternData.blockSize * flips;
		Vector2 worldOffset = (mid - right - up) - patternData.origin;
		worldOffset.y *= -1.f; // uvs are flipped vertically
		vsInput.uvOffset = worldOffset / patternData.blockSize + Vector2(0.f, -Math::FractionOf(globalScale.y / patternData.blockSize.y));
		vsInput.uvOffset *= flips;
		graphics->SetVertexShader(&assets->cameraVS, &vsInput, sizeof(CameraVSConstants));

		TexturePSConstants psInput;
		psInput.tint = patternData.tint;
		psInput.lit = patternData.lit;
		graphics->SetTexture(ShaderStage::Pixel, patternData.sprite, 0);
		graphics->SetPixelShader(&assets->texturePS, &psInput, sizeof(TexturePSConstants));

		graphics->DrawMesh(&assets->unitSquare);
	} break;

	case Type::Text: {

	} break;

	case Type::Particles: {

	} break;
	}
}

void Renderer::UpdateTextMesh() {
	ASSERT(type == Type::Text);
}

void Renderer::ClearParticles() {
	ASSERT(type == Type::Particles)
}

void Renderer::InitShape(PrimitiveShape shape, Color color) {
	hidden = false;
	translucent = color.alpha < 1.0f;
	type = Type::Shape;
	shapeData.shape = shape;
	shapeData.color = color;
}

void Renderer::InitSprite(const Texture2D* sprite) {
	hidden = false;
	translucent = false;
	type = Type::Sprite;
	spriteData.sprite = sprite;
	spriteData.tint = Color::White;
	spriteData.flipX = false;
	spriteData.flipY = false;
	spriteData.lit = false;
}

void Renderer::InitAtlas(const SpriteSheet* atlas) {
	hidden = false;
	translucent = false;
	type = Type::Atlas;
	atlasData.atlas = atlas;
	atlasData.row = 0u;
	atlasData.col = 0u;
	atlasData.tint = Color::White;
	atlasData.flipX = false;
	atlasData.flipY = false;
	atlasData.lit = false;
}

void Renderer::InitPattern(const Texture2D* sprite) {
	hidden = false;
	translucent = false;
	type = Type::Pattern;
	patternData.sprite = sprite;
	patternData.origin = Vector2::Zero;
	patternData.blockSize = Vector2(1.0f, 1.0f);
	patternData.tint = Color::White;
	patternData.flipX = false;
	patternData.flipY = false;
	patternData.lit = false;
}

void Renderer::InitLight(Color color, float radius) {
	hidden = false;
	translucent = false;
	lightData.color = color;
	lightData.radius = radius;
	lightData.brightness = 1.0f;
	lightData.coneSize = PI * 2.0f;
}
