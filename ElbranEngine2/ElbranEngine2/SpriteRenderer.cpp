#include "SpriteRenderer.h"
#include "Application.h"
#include "GraphicsAPI.h"
#include "AssetContainer.h"
#include "ShaderConstants.h"

SpriteRenderer::SpriteRenderer(const Texture2D* sprite) {
	this->sprite = sprite;
	tint = Colors::White;
	flipX = false;
	flipY = false;
	lit = false;
}

void SpriteRenderer::Draw() {
	GraphicsAPI* graphics = app->graphics;

	CameraVSConstants vsInput;
	// matrix
	// matrix
	vsInput.flipX = flipX;
	vsInput.flipY = flipY;
	
	graphics->SetConstants(&app->assets->cameraVS, &vsInput);
	graphics->SetVertexShader(&app->assets->cameraVS);

	TexturePSConstants psInput;
	psInput.tint = tint;
	psInput.lit = lit;
	graphics->SetConstants(&app->assets->texturePS, &psInput);
	graphics->SetTexture(ShaderStage::Pixel, sprite, 0);
	graphics->SetPixelShader(&app->assets->texturePS);

	graphics->DrawSquare();
}
