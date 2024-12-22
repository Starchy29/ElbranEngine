#include "RepeatRenderer.h"
#include "Application.h"

RepeatRenderer::RepeatRenderer(std::shared_ptr<Sprite> sprite, Vector2 baseScale) {
	tint = Color::White;
	this->sprite = sprite;
	this->baseScale = baseScale;

	const AssetManager* assets = APP->Assets();
	mesh = assets->unitSquare;
	vertexShader = assets->cameraVS;
	pixelShader = assets->imagePS;

	flipX = false;
	flipY = false;
}

void RepeatRenderer::Draw(Camera* camera, const Transform& transform) {
	DirectX::XMFLOAT4X4 worldViewProj;
	DirectX::XMStoreFloat4x4(&worldViewProj, CreateWorldViewProjection(camera, transform));
	vertexShader->SetConstantVariable("worldViewProj", &worldViewProj);
	vertexShader->SetBool("flipX", flipX);
	vertexShader->SetBool("flipY", flipY);

	pixelShader->SetConstantVariable("tint", &tint);
	pixelShader->SetSampler(APP->Assets()->defaultSampler);
	pixelShader->SetTexture(sprite->GetResourceView());

	Vector2 stretchFactor = transform.GetScale() / baseScale;
	pixelShader->SetConstantVariable("stretchFactor", &stretchFactor);

	vertexShader->SetShader();
	pixelShader->SetShader();
	mesh->Draw();

	Vector2 oneVec = Vector2(1, 1);
	pixelShader->SetConstantVariable("stretchFactor", &oneVec);
}

IRenderer* RepeatRenderer::Clone() {
	return new RepeatRenderer(*this);
}
