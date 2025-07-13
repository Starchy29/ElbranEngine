#include "TextRenderer.h"
#include "Application.h"
#include "GraphicsAPI.h"
#include "AssetContainer.h"
#include "ShaderConstants.h"

TextRenderer::TextRenderer(std::string text, const Font* font) :
	text{text},
	font{font}
{
	color = Color::White;
	GenerateMesh();
}

TextRenderer::~TextRenderer() {
	app->graphics->ReleaseMesh(&textMesh);
}

void TextRenderer::Draw() {
	GraphicsAPI* graphics = app->graphics;

	// calculate scalar
	
	// set vertex shader
	CameraVSConstants vsInput;
	vsInput.worldTransform = worldMatrix->Transpose();
	vsInput.uvOffset = Vector2::Zero;
	vsInput.uvScale = Vector2(1.f, 1.f); // assumes wrap enabled on sampling
	graphics->SetVertexShader(&app->assets->cameraVS, &vsInput, sizeof(CameraVSConstants));

	// set pixel shader
	graphics->SetArray(ShaderStage::Pixel, &font->glyphCurves, 0);
	graphics->SetArray(ShaderStage::Pixel, &font->firstCurveIndices, 1);
	graphics->SetPixelShader(&app->assets->textRasterizePS, &color, sizeof(Color));

	// draw mesh
	app->graphics->DrawMesh(&textMesh);
}

void TextRenderer::SetText(std::string text) {
	this->text = text;
	GenerateMesh();
}

void TextRenderer::SetFont(const Font* font) {
	this->font = font;
	GenerateMesh();
}

void TextRenderer::GenerateMesh() {
	app->graphics->ReleaseMesh(&textMesh);

	int glyphIndex = font->charToGlyphIndex.Get('}');
	Vector2 dimensions = font->glyphDimensions[glyphIndex];
	AlignedRect glyphBox = AlignedRect(Vector2::Zero, dimensions);

	Vertex vertices[4] = {
		{ Vector2(glyphBox.left, glyphBox.bottom), Vector2(glyphIndex + 0.0f, glyphIndex + 1.0f) },
		{ Vector2(glyphBox.left, glyphBox.top), Vector2(glyphIndex + 0.0f, glyphIndex + 0.0f) },
		{ Vector2(glyphBox.right, glyphBox.top), Vector2(glyphIndex + 1.0f, glyphIndex + 0.0f) },
		{ Vector2(glyphBox.right, glyphBox.bottom), Vector2(glyphIndex + 1.0f, glyphIndex + 1.0f) }
	};

	unsigned int indices[6] = {
		0, 1, 3, // clockwise winding order
		1, 2, 3
	};

	textMesh = app->graphics->CreateMesh(vertices, 4, indices, 6, false);
}