#include "AtlasRenderer.h"
#include "Application.h"
#include "GraphicsAPI.h"
#include "ShaderConstants.h"

void AtlasRenderer::Initialize(const SpriteSheet* atlas) {
	this->atlas = atlas;
	row = 0u;
	col = 0u;
	tint = Color::White;
	flipX = false;
	flipY = false;
	lit = false;
	transform = nullptr;
	worldMatrix = nullptr;
}

void AtlasRenderer::Draw() {
	GraphicsAPI* graphics = app->graphics;

	CameraVSConstants vsInput;
	vsInput.worldTransform = worldMatrix->Transpose();
	Vector2 tileScale = Vector2(1.0f / atlas->cols, 1.0f / atlas->rows);
	vsInput.uvScale = tileScale * Vector2(flipX ? -1.0f : 1.0f, flipY ? -1.0f : 1.0f); // assumes wrap enabled on sampling
	vsInput.uvOffset = Vector2(col + (flipX ? 1 : 0), row + (flipY ? 1 : 0)) * tileScale;
	graphics->SetVertexShader(&app->assets.cameraVS, &vsInput, sizeof(CameraVSConstants));

	TexturePSConstants psInput;
	psInput.tint = tint;
	psInput.lit = lit;
	graphics->SetTexture(ShaderStage::Pixel, &atlas->texture, 0);
	graphics->SetPixelShader(&app->assets.texturePS, &psInput, sizeof(TexturePSConstants));

	graphics->DrawMesh(&app->assets.unitSquare);
}
