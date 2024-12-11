#include "SpriteRenderer.h"
#include "Application.h"

SpriteRenderer::SpriteRenderer(std::shared_ptr<Sprite> sprite) {
	tint = Color::White;
	this->sprite = sprite;

	const AssetManager* assets = APP->Assets();
	mesh = assets->unitSquare;
	vertexShader = assets->cameraVS;
	pixelShader = assets->imagePS;
}

void SpriteRenderer::Draw(Camera* camera, const Transform& transform) {
	DirectX::XMFLOAT4X4 worldViewProj;
	DirectX::XMStoreFloat4x4(&worldViewProj, CreateWorldViewProjection(camera, transform));
	vertexShader->SetConstantVariable("worldViewProj", &worldViewProj);
	vertexShader->SetBool("flipX", flipX);
	vertexShader->SetBool("flipY", flipY);

	pixelShader->SetConstantVariable("tint", &tint);
	pixelShader->SetSampler(APP->Assets()->defaultSampler);
	pixelShader->SetTexture(sprite->GetResourceView());

	vertexShader->SetShader();
	pixelShader->SetShader();
	mesh->Draw();
}

IRenderer* SpriteRenderer::Clone() {
	return new SpriteRenderer(*this);
}
