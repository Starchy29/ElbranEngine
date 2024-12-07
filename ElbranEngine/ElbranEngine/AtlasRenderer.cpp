#include "AtlasRenderer.h"
#include "Application.h"

AtlasRenderer::AtlasRenderer(std::shared_ptr<SpriteAtlas> sprite)
	: SpriteRenderer(sprite)
{
	vertexShader = APP->Assets()->atlasVS;
	col = 0;
	row = 0;
}

void AtlasRenderer::Draw(Camera * camera, const Transform & transform) {
	SpriteAtlas* atlas = (SpriteAtlas*)sprite.get();
	float spriteWidth = 1.0f / atlas->NumCols();
	float spriteHeight = 1.0f / atlas->NumRows();
	vertexShader->SetConstantVariable("spriteWidth", &spriteWidth);
	vertexShader->SetConstantVariable("spriteHeight", &spriteHeight);
	vertexShader->SetConstantVariable("row", &row);
	vertexShader->SetConstantVariable("col", &col);
	SpriteRenderer::Draw(camera, transform);
}

IRenderer* AtlasRenderer::Clone() {
	return new AtlasRenderer(*this);
}
