#include "AtlasRenderer.h"
#include "Application.h"
#include "GraphicsAPI.h"
#include "AssetContainer.h"
#include "ShaderConstants.h"

AtlasRenderer::AtlasRenderer(SpriteSheet* atlas) {
	transform = nullptr;
	worldMatrix = nullptr;

	this->atlas = atlas;
	row = 0;
	col = 0;
	tint = Color::White;
	flipX = false;
	flipY = false;
	lit = false;
}

void AtlasRenderer::Draw() {
	GraphicsAPI* graphics = app->graphics;

	CameraVSConstants vsInput;
	vsInput.worldTransform = worldMatrix->Transpose();
	vsInput.uvScale = Vector2(1.0f / atlas->cols * (flipX ? -1.0f : 1.0f), 1.0f / atlas->rows * (flipY ? -1.0f : 1.0f)); // assumes wrap enabled on sampling
	vsInput.uvOffset = Vector2(col, row) * vsInput.uvScale;
	graphics->SetVertexShader(&app->assets->cameraVS, &vsInput, sizeof(CameraVSConstants));

	TexturePSConstants psInput;
	psInput.tint = tint;
	psInput.lit = lit;
	graphics->SetTexture(ShaderStage::Pixel, &atlas->texture, 0);
	graphics->SetPixelShader(&app->assets->texturePS, &psInput, sizeof(TexturePSConstants));

	graphics->DrawMesh(&app->assets->unitSquare);
}
