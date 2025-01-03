#include "LightRenderer.h"

LightRenderer::LightRenderer(Color color, float radius, float brightness) {
	mesh = nullptr;
	pixelShader = nullptr;
	vertexShader = nullptr;

	this->color = color;
	this->radius = radius;
	this->brightness = brightness;
}

void LightRenderer::Draw(Camera* camera, const Transform& transform) { }

IRenderer* LightRenderer::Clone() {
	return new LightRenderer(*this);
}
