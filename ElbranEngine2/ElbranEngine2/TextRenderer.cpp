#include "TextRenderer.h"

void TextRenderer::Draw() {
	// set vertex shader

	// set pixel shader

	// draw mesh
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