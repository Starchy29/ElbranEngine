#include "SpriteRenderer.h"
#include "Application.h"
#include "GraphicsAPI.h"
#include "AssetContainer.h"
#include "ShaderConstants.h"

SpriteRenderer::SpriteRenderer(Texture2D* sprite) {
	transform = nullptr;
	worldMatrix = nullptr;

	this->sprite = sprite;
	tint = Color::White;
	flipX = false;
	flipY = false;
	lit = false;
}

void SpriteRenderer::Draw() {
	GraphicsAPI* graphics = app->graphics;
	worldMatrix->values[0];

	CameraVSConstants vsInput;
	vsInput.worldTransform = worldMatrix->Transpose();
	vsInput.flipX = flipX;
	vsInput.flipY = flipY;
	graphics->SetVertexShader(&app->assets->cameraVS, &vsInput, sizeof(CameraVSConstants));

	TexturePSConstants psInput;
	psInput.tint = tint;
	psInput.lit = lit;
	graphics->SetTexture(ShaderStage::Pixel, sprite, 0);
	graphics->SetPixelShader(&app->assets->texturePS, &psInput, sizeof(TexturePSConstants));

	graphics->DrawMesh(&app->assets->unitSquare);
}
