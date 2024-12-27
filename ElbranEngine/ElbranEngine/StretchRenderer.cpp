#include "StretchRenderer.h"
#include "Application.h"

StretchRenderer::StretchRenderer(std::shared_ptr<Sprite> sprite, Vector2 baseScale, Vector2 startUV, Vector2 endUV) {
	this->sprite = sprite;
	this->baseScale = baseScale;
	this->startUV = startUV;
	this->endUV = endUV;

	tint = Color::White;
	flipX = false;
	flipY = false;

	const AssetManager* assets = APP->Assets();
	mesh = assets->unitSquare;
	vertexShader = assets->cameraVS;
	pixelShader = assets->stretchyPS;
}

void StretchRenderer::Draw(Camera* camera, const Transform& transform) {
	DirectX::XMFLOAT4X4 worldViewProj;
	DirectX::XMStoreFloat4x4(&worldViewProj, CreateWorldViewProjection(camera, transform));
	vertexShader->SetConstantVariable("worldViewProj", &worldViewProj);
	vertexShader->SetBool("flipX", flipX);
	vertexShader->SetBool("flipY", flipY);

	pixelShader->SetConstantVariable("tint", &tint);
	pixelShader->SetSampler(APP->Assets()->defaultSampler);
	pixelShader->SetTexture(sprite->GetResourceView());

	Vector2 stretchFactor = transform.GetScale(false) / baseScale;
	pixelShader->SetConstantVariable("stretchFactor", &stretchFactor);
	pixelShader->SetConstantVariable("startUV", &startUV);
	pixelShader->SetConstantVariable("endUV", &endUV);

	vertexShader->SetShader();
	pixelShader->SetShader();
	mesh->Draw();
}

IRenderer* StretchRenderer::Clone() {
	return new StretchRenderer(*this);
}
