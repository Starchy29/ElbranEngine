#include "ColorRenderer.h"
#include "Application.h"

ColorRenderer::ColorRenderer(Color color, bool circle) {
	this->color = color;

	const AssetManager* assets = APP->Assets();
	mesh = assets->unitSquare;
	vertexShader = assets->cameraVS;
	pixelShader = circle ? assets->circlePS : assets->colorPS;
}

void ColorRenderer::Draw(Camera* camera, const Transform& transform) {
	DirectX::XMFLOAT4X4 worldViewProj;
	DirectX::XMStoreFloat4x4(&worldViewProj, CreateWorldViewProjection(camera, transform));
	vertexShader->SetConstantVariable("worldViewProj", &worldViewProj);
	pixelShader->SetConstantVariable("color", &color);
	vertexShader->SetShader();
	pixelShader->SetShader();
	mesh->Draw();
}

IRenderer* ColorRenderer::Clone() {
	return new ColorRenderer(*this);
}
