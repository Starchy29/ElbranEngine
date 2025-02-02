#include "StretchRenderer.h"
#include "Application.h"
#include "AssetManager.h"
#include "PixelShader.h"
#include "Transform.h"

StretchRenderer::StretchRenderer(std::shared_ptr<Sprite> sprite, Vector2 baseScale, Vector2 startUV, Vector2 endUV) 
	: SpriteRenderer(sprite)
{
	this->baseScale = baseScale;
	this->startUV = startUV;
	this->endUV = endUV;

	const AssetManager* assets = APP->Assets();
	pixelShader = assets->stretchyPS;
}

void StretchRenderer::Draw(Camera* camera, const Transform& transform) {
	Vector2 stretchFactor = transform.GetScale(false) / baseScale;
	pixelShader->SetConstantVariable("stretchFactor", &stretchFactor);
	pixelShader->SetConstantVariable("startUV", &startUV);
	pixelShader->SetConstantVariable("endUV", &endUV);

	SpriteRenderer::Draw(camera, transform);
}

IBehavior* StretchRenderer::Clone() {
	return new StretchRenderer(*this);
}
