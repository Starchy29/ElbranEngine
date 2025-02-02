#include "ColorRenderer.h"
#include "Application.h"
#include "AssetManager.h"
#include "VertexShader.h"
#include "PixelShader.h"

ColorRenderer::ColorRenderer(Color color, Shape shape = Square) {
	this->color = color;

	const AssetManager* assets = APP->Assets();
	mesh = shape == Triangle ? assets->unitTriangle : assets->unitSquare;
	vertexShader = assets->cameraVS;
	pixelShader = shape == Circle ? assets->circlePS : assets->colorPS;
}

void ColorRenderer::Draw(Camera* camera, const Transform& transform) {
	DirectX::XMFLOAT4X4 worldViewProj;
	GetScreenTransform(&worldViewProj, camera, transform);
	vertexShader->SetConstantVariable("worldTransform", &transform);
	vertexShader->SetConstantVariable("worldViewProj", &worldViewProj);
	pixelShader->SetConstantVariable("color", &color);
	vertexShader->SetShader();
	pixelShader->SetShader();
	mesh->Draw();
}

IBehavior* ColorRenderer::Clone() {
	return new ColorRenderer(*this);
}
