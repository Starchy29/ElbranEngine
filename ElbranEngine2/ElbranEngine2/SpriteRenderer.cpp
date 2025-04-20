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

	VertexShader* vertexShader = &app->assets->cameraVS;
	CameraVSConstants vsInput;
	vsInput.worldTransform = worldMatrix->Transpose();
	vsInput.flipX = flipX;
	vsInput.flipY = flipY;
	graphics->WriteBuffer(&vsInput, sizeof(CameraVSConstants), vertexShader->constants.data);
	graphics->SetConstants(ShaderStage::Vertex, &vertexShader->constants, 0);
	graphics->SetVertexShader(vertexShader);

	PixelShader* pixelShader = &app->assets->texturePS;
	TexturePSConstants psInput;
	psInput.tint = tint;
	psInput.lit = lit;
	graphics->WriteBuffer(&psInput, sizeof(TexturePSConstants), pixelShader->constants.data);
	graphics->SetConstants(ShaderStage::Pixel, &pixelShader->constants, 0);
	graphics->SetTexture(ShaderStage::Pixel, sprite, 0);
	graphics->SetPixelShader(pixelShader);

	graphics->DrawMesh(&app->assets->unitSquare);
}
