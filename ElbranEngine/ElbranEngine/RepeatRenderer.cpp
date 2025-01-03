#include "RepeatRenderer.h"
#include "Application.h"

RepeatRenderer::RepeatRenderer(std::shared_ptr<Sprite> sprite, Vector2 baseScale) 
	: SpriteRenderer(sprite)
{
	this->baseScale = baseScale;
}

void RepeatRenderer::Draw(Camera* camera, const Transform& transform) {
	Vector2 stretchFactor = transform.GetScale(false) / baseScale;
	pixelShader->SetConstantVariable("stretchFactor", &stretchFactor);

	SpriteRenderer::Draw(camera, transform);

	// reset variable for deafault sprite renderers
	stretchFactor = Vector2(1, 1);
	pixelShader->SetConstantVariable("stretchFactor", &stretchFactor);
}

IRenderer* RepeatRenderer::Clone() {
	return new RepeatRenderer(*this);
}
