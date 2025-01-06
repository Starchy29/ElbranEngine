#include "SpriteRenderer.h"
#include "Application.h"

SpriteRenderer::SpriteRenderer(std::shared_ptr<Sprite> sprite) {
	this->sprite = sprite; 
	
	tint = Color::White;
	flipX = false;
	flipY = false;
	useLights = false;

	const AssetManager* assets = APP->Assets();
	mesh = assets->unitSquare;
	vertexShader = assets->cameraVS;
	pixelShader = assets->imagePS;
}

void SpriteRenderer::Draw(Camera* camera, const Transform& transform) {
	DirectX::XMFLOAT4X4 worldViewProj;
	DirectX::XMStoreFloat4x4(&worldViewProj, CreateWorldViewProjection(camera, transform));
	DirectX::XMFLOAT4X4 worldTransform = transform.GetWorldMatrix();
	vertexShader->SetConstantVariable("worldTransform", &worldTransform);
	vertexShader->SetConstantVariable("worldViewProj", &worldViewProj);
	vertexShader->SetBool("flipX", flipX);
	vertexShader->SetBool("flipY", flipY);

	pixelShader->SetBool("lit", useLights);
	pixelShader->SetConstantVariable("tint", &tint);
	pixelShader->SetSampler(APP->Assets()->defaultSampler);
	pixelShader->SetTexture(sprite->GetResourceView());

	vertexShader->SetShader();
	pixelShader->SetShader();
	mesh->Draw();
}

IBehavior* SpriteRenderer::Clone() {
	return new SpriteRenderer(*this);
}
