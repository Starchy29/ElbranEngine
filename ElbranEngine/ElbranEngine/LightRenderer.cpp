#include "LightRenderer.h"

LightRenderer::LightRenderer(Color color, float radius, float brightness) {
	mesh = nullptr;
	pixelShader = nullptr;
	vertexShader = nullptr;

	this->color = color;
	this->radius = radius;
	this->brightness = brightness;
	coneSize = DirectX::XM_2PI;
}

void LightRenderer::Draw(Camera* camera, const Transform& transform) { }

IBehavior* LightRenderer::Clone() {
	return new LightRenderer(*this);
}
