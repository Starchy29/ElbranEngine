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
	app->graphics->DrawMesh(&app->assets->unitSquare);
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
	
}