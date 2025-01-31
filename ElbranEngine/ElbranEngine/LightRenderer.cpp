#include "LightRenderer.h"

LightRenderer::LightRenderer(Color color, float radius, float brightness) {
	pixelShader = nullptr;
	vertexShader = nullptr;

	this->color = color;
	this->radius = radius;
	this->brightness = brightness;
	coneSize = DirectX::XM_2PI;
}

// lights cannot be camera relative
void LightRenderer::Draw(Camera* camera, const Transform& transform) { }

IBehavior* LightRenderer::Clone() {
	return new LightRenderer(*this);
}
