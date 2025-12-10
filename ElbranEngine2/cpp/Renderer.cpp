#include "Renderer.h"
#include "MemoryArena.h"
#include "GraphicsAPI.h"
#include "AssetContainer.h"
#include "ShaderConstants.h"

void Renderer::Draw(GraphicsAPI* graphics, const AssetContainer* assets) {
	switch(type) {
	case Type::Shape: {
		CameraVSConstants vsInput = {};
		vsInput.uvScale = Vector2(1.f, 1.f);
		vsInput.worldTransform = worldMatrix->Transpose();
		graphics->SetVertexShader(&assets->cameraVS, &vsInput, sizeof(CameraVSConstants));
		graphics->SetPixelShader(shapeData.shape == PrimitiveShape::Circle ? &assets->circleFillPS : &assets->solidColorPS, &shapeData.color, sizeof(Color));
		graphics->DrawMesh(shapeData.shape == PrimitiveShape::Triangle ? &assets->unitTriangle : &assets->unitSquare);
	} break;

	case Type::Sprite: {
		CameraVSConstants vsInput;
		vsInput.worldTransform = worldMatrix->Transpose();
		vsInput.uvOffset = Vector2::Zero;
		vsInput.uvScale = Vector2(spriteData.flipX ? -1 : 1, spriteData.flipY ? -1 : 1); // assumes wrap enabled on sampling
		graphics->SetVertexShader(&assets->cameraVS, &vsInput, sizeof(CameraVSConstants));

		TexturePSConstants psInput;
		psInput.tint = spriteData.tint;
		psInput.lit = spriteData.lit;
		graphics->SetTexture(ShaderStage::Pixel, spriteData.sprite, 0);
		graphics->SetPixelShader(&assets->texturePS, &psInput, sizeof(TexturePSConstants));

		graphics->DrawMesh(&assets->unitSquare);
	} break;

	case Type::Atlas: {
		CameraVSConstants vsInput;
		vsInput.worldTransform = worldMatrix->Transpose();
		Vector2 tileScale = Vector2(1.0f / atlasData.atlas->cols, 1.0f / atlasData.atlas->rows);
		vsInput.uvScale = tileScale * Vector2(atlasData.flipX ? -1.0f : 1.0f, atlasData.flipY ? -1.0f : 1.0f); // assumes wrap enabled on sampling
		vsInput.uvOffset = Vector2(atlasData.col + (atlasData.flipX ? 1 : 0), atlasData.row + (atlasData.flipY ? 1 : 0)) * tileScale;
		graphics->SetVertexShader(&assets->cameraVS, &vsInput, sizeof(CameraVSConstants));

		TexturePSConstants psInput;
		psInput.tint = atlasData.tint;
		psInput.lit = atlasData.lit;
		graphics->SetTexture(ShaderStage::Pixel, &atlasData.atlas->texture, 0);
		graphics->SetPixelShader(&assets->texturePS, &psInput, sizeof(TexturePSConstants));

		graphics->DrawMesh(&assets->unitSquare);
	} break;

	case Type::Pattern: {
		Vector2 mid = *worldMatrix * Vector2::Zero;
		Vector2 right = *worldMatrix * Vector2(0.5f, 0.f);
		Vector2 up = *worldMatrix * Vector2(0.f, 0.5f);
		Vector2 globalScale = Vector2(right.x - mid.x, up.y - mid.y) * 2.f; // fails if this is rotated

		CameraVSConstants vsInput;
		vsInput.worldTransform = worldMatrix->Transpose();
		Vector2 flips = Vector2(patternData.flipX ? -1 : 1, patternData.flipY ? -1 : 1); // assumes wrap enabled on sampling
		vsInput.uvScale = globalScale / patternData.blockSize * flips;
		Vector2 worldOffset = (mid - right - up) - patternData.origin;
		worldOffset.y *= -1.f; // uvs are flipped vertically
		vsInput.uvOffset = worldOffset / patternData.blockSize + Vector2(0.f, -Math::FractionOf(globalScale.y / patternData.blockSize.y));
		vsInput.uvOffset *= flips;
		graphics->SetVertexShader(&assets->cameraVS, &vsInput, sizeof(CameraVSConstants));

		TexturePSConstants psInput;
		psInput.tint = patternData.tint;
		psInput.lit = patternData.lit;
		graphics->SetTexture(ShaderStage::Pixel, patternData.sprite, 0);
		graphics->SetPixelShader(&assets->texturePS, &psInput, sizeof(TexturePSConstants));

		graphics->DrawMesh(&assets->unitSquare);
	} break;

	case Type::Text: {
		Vector2 worldCenter = *worldMatrix * Vector2::Zero;
		Vector2 worldRight = *worldMatrix * Vector2(0.5f, 0.0f);
		Vector2 worldTop = *worldMatrix * Vector2(0.0f, 0.5f);
		Vector2 worldScale = Vector2(worldRight.x - worldCenter.x, worldTop.y - worldCenter.y);

		// scale and position within the text box
		AlignedRect fillArea = AlignedRect(transform->position, worldScale).Expand(-textData.padding);
		float boundAspectRatio = worldScale.x / worldScale.y;
		Vector2 paddingScaleDiffs = fillArea.Size() / worldScale;

		Vector2 unstretchFactor;
		Vector2 alignment = Vector2::Zero;
		if(fillArea.Width() > fillArea.Height() * textData.blockAspectRatio) {
			unstretchFactor = Vector2(textData.blockAspectRatio / boundAspectRatio, 1.0f) * paddingScaleDiffs.y;
			float openSpace = worldScale.x - textData.blockAspectRatio * worldScale.y + textData.padding * textData.blockAspectRatio;
			if(textData.horizontalAlignment == HorizontalAlignment::Left) alignment.x = -0.5f * openSpace / worldScale.x;
			else if(textData.horizontalAlignment == HorizontalAlignment::Right) alignment.x = 0.5f * openSpace / worldScale.x;
		} else {
			unstretchFactor = Vector2(1.0f, boundAspectRatio / textData.blockAspectRatio) * paddingScaleDiffs.x;
			float openSpace = worldScale.y - worldScale.x / textData.blockAspectRatio - 2.0f * textData.padding / textData.blockAspectRatio;
			if(textData.verticalAlignment == VerticalAlignment::Bottom) alignment.y = -0.5f * openSpace / worldScale.y;
			else if(textData.verticalAlignment == VerticalAlignment::Top) alignment.y = 0.5f * openSpace / worldScale.y;
		}

		// set vertex shader
		CameraVSConstants vsInput;
		vsInput.worldTransform = (*worldMatrix * Matrix::Translation(alignment.x, alignment.y) * Matrix::Scale(unstretchFactor.x, unstretchFactor.y)).Transpose();
		vsInput.uvOffset = Vector2::Zero;
		vsInput.uvScale = Vector2(1.f, 1.f);
		graphics->SetVertexShader(&assets->cameraVS, &vsInput, sizeof(CameraVSConstants));

		// set pixel shader
		graphics->SetArray(ShaderStage::Pixel, &textData.font->glyphCurves, 0);
		graphics->SetArray(ShaderStage::Pixel, &textData.font->firstCurveIndices, 1);
		graphics->SetPixelShader(&assets->textRasterizePS, &textData.color, sizeof(Color));

		// draw mesh
		graphics->DrawMesh(&textData.textMesh);
	} break;

	case Type::Particles: {
		// vertex shader
		graphics->ClearMesh();
		graphics->SetArray(ShaderStage::Vertex, &particleData.particleBuffer, 0);
		graphics->SetVertexShader(&assets->particlePassPS);

		// geometry shader
		ParticleQuadGSConstants gsInput = {};
		gsInput.z = worldMatrix->values[2][3];
		gsInput.spriteAspectRatio = particleData.sprites->SpriteAspectRatio();
		gsInput.animationFPS = particleData.animationFPS;
		gsInput.animationFrames = particleData.sprites->spriteCount;
		gsInput.atlasRows = particleData.sprites->rows;
		gsInput.atlasCols = particleData.sprites->cols;
		graphics->SetGeometryShader(&assets->particleQuadGS, &gsInput, sizeof(ParticleQuadGSConstants));

		// pixel shader
		TexturePSConstants psInput;
		psInput.lit = particleData.applyLights;
		psInput.tint = particleData.tint;
		graphics->SetTexture(ShaderStage::Pixel, &particleData.sprites->texture, 0);
		graphics->SetPixelShader(&assets->texturePS, &psInput, sizeof(psInput));

		// draw
		if(particleData.blendAdditive) {
			graphics->SetBlendMode(BlendState::Additive);
		}
		graphics->SetPrimitive(RenderPrimitive::Point);
		graphics->DrawVertices(particleData.maxParticles);

		// clean up
		if(particleData.blendAdditive) {
			graphics->SetBlendMode(BlendState::AlphaBlend);
		}
		graphics->SetPrimitive(RenderPrimitive::Triangle);
		graphics->SetGeometryShader(nullptr);
		graphics->SetArray(ShaderStage::Vertex, nullptr, 0); // unbind particles
	} break;
	}
}

bool Renderer::IsTranslucent() const {
	switch(type) {
	case Type::Shape: return shapeData.color.alpha > 0.f && shapeData.color.alpha < 1.f;
	case Type::Sprite: return spriteData.sprite->translucent || spriteData.tint.alpha > 0.f && spriteData.tint.alpha < 1.f;
	case Type::Atlas: return atlasData.atlas->texture.translucent || atlasData.tint.alpha > 0.f && atlasData.tint.alpha < 1.f;
	case Type::Pattern: return patternData.sprite->translucent || patternData.tint.alpha > 0.f && patternData.tint.alpha < 1.f;
	case Type::Text: return true; // anti-aliasing uses alpha
	case Type::Particles: return particleData.blendAdditive || particleData.sprites->texture.translucent;
	}
	return false;
}

void Renderer::Release(GraphicsAPI* graphics) {
	switch(type) {
	case Type::Text:
		graphics->ReleaseMesh(&textData.textMesh);
		break;
	case Type::Particles:
		graphics->ReleaseEditBuffer(&particleData.particleBuffer);
		break;
	}
}

void Renderer::InitShape(PrimitiveShape shape, Color color) {
	type = Type::Shape;
	hidden = false;
	shapeData.shape = shape;
	shapeData.color = color;
}

void Renderer::InitSprite(const Texture2D* sprite) {
	type = Type::Sprite;
	hidden = false;
	spriteData.sprite = sprite;
	spriteData.tint = Color::White;
	spriteData.flipX = false;
	spriteData.flipY = false;
	spriteData.lit = false;
}

void Renderer::InitAtlas(const SpriteSheet* atlas) {
	type = Type::Atlas;
	hidden = false;
	atlasData.atlas = atlas;
	atlasData.row = 0u;
	atlasData.col = 0u;
	atlasData.tint = Color::White;
	atlasData.flipX = false;
	atlasData.flipY = false;
	atlasData.lit = false;
}

void Renderer::InitPattern(const Texture2D* sprite) {
	type = Type::Pattern;
	hidden = false;
	patternData.sprite = sprite;
	patternData.origin = Vector2::Zero;
	patternData.blockSize = Vector2(1.0f, 1.0f);
	patternData.tint = Color::White;
	patternData.flipX = false;
	patternData.flipY = false;
	patternData.lit = false;
}

void Renderer::InitLight(Color color, float radius) {
	type = Type::Light;
	hidden = false;
	lightData.color = color;
	lightData.radius = radius;
	lightData.brightness = 1.0f;
	lightData.coneSize = PI * 2.0f;
}

void Renderer::InitText(GraphicsAPI* graphics, MemoryArena* arena, const char* text, const Font* font, HorizontalAlignment horizontalAlignment, float lineSpacing) {
	type = Type::Text;
	hidden = false;

	textData.text = text;
	textData.font = font;
	textData.horizontalAlignment = horizontalAlignment;
	textData.lineSpacing = lineSpacing;

	textData.verticalAlignment = VerticalAlignment::Center;
	textData.color = Color::White;

	UpdateTextMesh(graphics, arena);
}

#define PARTICLE_BYTES 32 // based on struct in ShaderStructs.hlsli
void Renderer::InitParticles(GraphicsAPI* graphics, uint16_t maxParticles, const SpriteSheet* animation, float animationFPS) {
	type = Type::Particles;
	hidden = false;

	particleData.sprites = animation;
	particleData.animationFPS = animationFPS;
	particleData.maxParticles = maxParticles;
	particleData.tint = Color::White;
	particleData.applyLights = false;
	particleData.blendAdditive = false;
	particleData.scaleWithParent = true;

	particleData.particleBuffer = graphics->CreateEditBuffer(ShaderDataType::Structured, maxParticles, PARTICLE_BYTES);
}

void Renderer::UpdateTextMesh(GraphicsAPI* graphics, MemoryArena* arena) {
	ASSERT(type == Type::Text)

	graphics->ReleaseMesh(&textData.textMesh);

	// determine dimensions
	uint16_t rows = 1;
	uint32_t charIndex = 0;
	while(textData.text[charIndex] != 0) {
		if(textData.text[charIndex] == '\n') rows++;
		charIndex++;
	}
	uint32_t textLength = charIndex; // excludes null terminator
	float* rowWidths = (float*)arena->Reserve(sizeof(float) * rows);

	uint16_t currentRow = 0;
	charIndex = 0;
	while(textData.text[charIndex] != 0) {
		if(textData.text[charIndex] == '\n') {
			currentRow++;
		} else {
			rowWidths[currentRow] += textData.font->glyphDimensions[textData.font->charToGlyphIndex.Get(textData.text[charIndex])].x;
		}
		charIndex++;
	}

	float maxWidth = rowWidths[0];
	for(uint16_t i = 1; i < rows; i++) {
		maxWidth = Math::Max(maxWidth, rowWidths[i]);
	}
	float totalHeight = rows + (rows - 1) * textData.lineSpacing;

	// create mesh to fit in a 1x1 square
	Vertex* vertices = (Vertex*)arena->Reserve(sizeof(Vertex) * 4 * textLength);
	uint32_t* indices = (uint32_t*)arena->Reserve(sizeof(uint32_t) * 6 * textLength);
	Vector2 cursor = Vector2(0.f, -1.f); // start at y=-1 so the top is at y=0
	if(textData.horizontalAlignment == HorizontalAlignment::Right) cursor.x = maxWidth - rowWidths[0];
	else if(textData.horizontalAlignment == HorizontalAlignment::Center) cursor.x = (maxWidth - rowWidths[0]) * 0.5f;
	currentRow = 0;
	for(uint32_t i = 0; i < textLength; i++) {
		if(textData.text[i] == '\n') {
			cursor.y -= (1.0f + textData.lineSpacing);
			currentRow++;
			cursor.x = 0.f;
			if(textData.horizontalAlignment == HorizontalAlignment::Right) cursor.x = maxWidth - rowWidths[currentRow];
			else if(textData.horizontalAlignment == HorizontalAlignment::Center) cursor.x = (maxWidth - rowWidths[currentRow]) * 0.5f;
		} else {
			uint16_t glyphIndex = textData.font->charToGlyphIndex.Get(textData.text[i]);

			Vector2 dimensions = textData.font->glyphDimensions[glyphIndex];
			float baseLine = textData.font->glyphBaselines[glyphIndex] * dimensions.y;
			AlignedRect glyphBox = AlignedRect(cursor.x, cursor.x + dimensions.x, cursor.y + dimensions.y - baseLine, cursor.y - baseLine);
			glyphBox = glyphBox.Translate(Vector2(maxWidth * -0.5f, 0.5f * totalHeight));
			glyphBox.left /= maxWidth;
			glyphBox.right /= maxWidth;
			glyphBox.top /= totalHeight;
			glyphBox.bottom /= totalHeight;

			vertices[4*i] = Vertex{Vector2(glyphBox.left, glyphBox.bottom), Vector2(glyphIndex + 0.0f, glyphIndex + 1.0f)};
			vertices[4*i+1] = Vertex{Vector2(glyphBox.left, glyphBox.top), Vector2(glyphIndex + 0.0f, glyphIndex + 0.0f)};
			vertices[4*i+2] = Vertex{Vector2(glyphBox.right, glyphBox.top), Vector2(glyphIndex + 1.0f, glyphIndex + 0.0f)};
			vertices[4*i+3] = Vertex{Vector2(glyphBox.right, glyphBox.bottom), Vector2(glyphIndex + 1.0f, glyphIndex + 1.0f)};

			indices[6*i] = 4*i;
			indices[6*i+1] = 4*i+1;
			indices[6*i+2] = 4*i+3;
			indices[6*i+3] = 4*i+1;
			indices[6*i+4] = 4*i+2;
			indices[6*i+5] = 4*i+3;

			cursor.x += dimensions.x;
		}
	}

	textData.textMesh = graphics->CreateMesh(vertices, 4 * textLength, indices, 6 * textLength, false);
	textData.blockAspectRatio = maxWidth / totalHeight;
}

void Renderer::ClearParticles(GraphicsAPI* graphics, const AssetContainer* assets) {
	ASSERT(type == Type::Particles)
	graphics->WriteBuffer(&particleData.maxParticles, sizeof(uint16_t), assets->particleClearCS.constants.data);
	graphics->SetConstants(ShaderStage::Compute, &assets->particleClearCS.constants, 0);
	graphics->SetEditBuffer(&particleData.particleBuffer, 0);
	graphics->RunComputeShader(&assets->particleClearCS, particleData.maxParticles, 1);
	graphics->SetEditBuffer(nullptr, 0); // unbind particles
}