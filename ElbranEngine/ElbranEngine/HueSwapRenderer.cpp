#include "HueSwapRenderer.h"
#include "Application.h"

HueSwapRenderer::HueSwapRenderer(std::shared_ptr<Sprite> sprite) 
	: SpriteRenderer(sprite)
{
	pixelShader = APP->Assets()->hueSwapPS;
}

void HueSwapRenderer::Draw(Camera* camera, const Transform& transform) {
	pixelShader->SetConstantVariable("replacedColor", &oldHue);
	pixelShader->SetConstantVariable("replacementColor", &newHue);
	pixelShader->SetConstantVariable("sensitivity", &sensitivity);
	SpriteRenderer::Draw(camera, transform);
}

IBehavior* HueSwapRenderer::Clone() {
	return new HueSwapRenderer(*this);
}
