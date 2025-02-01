#include "SpriteRenderer.h"
#include "Application.h"

SpriteRenderer::SpriteRenderer(std::shared_ptr<Sprite> sprite) {
	this->sprite = sprite; 
	
	tint = Color::White;
	flipX = false;
	flipY = false;
	useLights = false;

	const AssetManager* assets = APP->Assets();
	vertexShader = assets->cameraVS;
	pixelShader = assets->imagePS;
}

void SpriteRenderer::Draw(Camera* camera, const Transform& transform) {
	DirectX::XMFLOAT4X4 worldViewProj;
	GetScreenTransform(&worldViewProj, camera, transform);
	vertexShader->SetConstantVariable("worldTransform", transform.GetWorldMatrix());
	vertexShader->SetConstantVariable("worldViewProj", &worldViewProj);
	vertexShader->SetBool("flipX", flipX);
	vertexShader->SetBool("flipY", flipY);

	const AssetManager* assets = APP->Assets();
	pixelShader->SetBool("lit", useLights);
	pixelShader->SetConstantVariable("tint", &tint);
	pixelShader->SetSampler(assets->defaultSampler);
	pixelShader->SetTexture(sprite->GetResourceView());

	vertexShader->SetShader();
	pixelShader->SetShader();
	assets->unitSquare->Draw();
}

IBehavior* SpriteRenderer::Clone() {
	return new SpriteRenderer(*this);
}
