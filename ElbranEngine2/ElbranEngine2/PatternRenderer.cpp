#include "PatternRenderer.h"
#include "Application.h"
#include "GraphicsAPI.h"
#include "AssetContainer.h"
#include "ShaderConstants.h"

PatternRenderer::PatternRenderer(Texture2D* sprite) {
	this->sprite = sprite;
	origin = Vector2::Zero;
	blockSize = Vector2(1, 1);

	tint = Color::White;
	flipX = false;
	flipY = false;
	lit = false;
}

void PatternRenderer::Draw() {
	GraphicsAPI* graphics = app->graphics;

	Vector2 mid = *worldMatrix * Vector2::Zero;
	Vector2 right = *worldMatrix * Vector2(0.5f, 0.f);
	Vector2 up = *worldMatrix * Vector2(0.f, 0.5f);
	Vector2 globalScale = Vector2(right.x - mid.x, up.y - mid.y) * 2.f; // fails if this is rotated

	CameraVSConstants vsInput;
	vsInput.worldTransform = worldMatrix->Transpose();
	vsInput.uvOffset = (mid - right - up) - origin;
	vsInput.uvScale = globalScale / blockSize * Vector2(flipX ? -1 : 1, flipY ? -1 : 1); // assumes wrap enabled on sampling
	graphics->SetVertexShader(&app->assets->cameraVS, &vsInput, sizeof(CameraVSConstants));

	TexturePSConstants psInput;
	psInput.tint = tint;
	psInput.lit = lit;
	graphics->SetTexture(ShaderStage::Pixel, sprite, 0);
	graphics->SetPixelShader(&app->assets->texturePS, &psInput, sizeof(TexturePSConstants));

	graphics->DrawMesh(&app->assets->unitSquare);
}
