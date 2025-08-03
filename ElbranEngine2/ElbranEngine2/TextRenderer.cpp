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
	float boundAspectRatio = transform->scale.x / transform->scale.y;
	Matrix unstretcher;
	if(transform->scale.x > transform->scale.y * blockAspectRatio) {
		unstretcher = Matrix::Scale(blockAspectRatio / boundAspectRatio, 1.0f);
	} else {
		unstretcher = Matrix::Scale(1.0f, boundAspectRatio / blockAspectRatio);
	}
	
	// set vertex shader
	CameraVSConstants vsInput;
	vsInput.worldTransform = (*worldMatrix * unstretcher).Transpose();
	vsInput.uvOffset = Vector2::Zero;
	vsInput.uvScale = Vector2(1.f, 1.f);
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

	// determine dimensions
	unsigned int textLength = text.size();
	int rows = 1;
	for(int i = 0; i < textLength; i++) {
		if(text[i] == '\n') {
			rows++;
		}
	}
	float* rowWidths = (float*)app->perFrameData.Reserve(sizeof(float) * rows, true);

	int currentRow = 0;
	for(int i = 0; i < textLength; i++) {
		if(text[i] == '\n') {
			currentRow++;
		} else {
			rowWidths[currentRow] += font->glyphDimensions[font->charToGlyphIndex.Get(text[i])].x;
		}
	}

	float maxWidth = rowWidths[0];
	for(int i = 1; i < rows; i++) {
		maxWidth = max(maxWidth, rowWidths[i]);
	}

	// create mesh to fit in a 1x1 square
	Vertex* vertices = (Vertex*)app->perFrameData.Reserve(sizeof(Vertex) * 4 * textLength);
	unsigned int* indices = (unsigned int*)app->perFrameData.Reserve(sizeof(unsigned int) * 6 * textLength);
	Vector2 cursor = Vector2(0.f, -1.f); // start at y=-1 so the top is at y=0
	currentRow = 0;
	for(int i = 0; i < text.size(); i++) {
		if(text[i] == '\n') {
			cursor.y -= 1.0f;
			cursor.x = 0.f;
			currentRow++;
		} else {
			int glyphIndex = font->charToGlyphIndex.Get(text[i]);

			Vector2 dimensions = font->glyphDimensions[glyphIndex];
			float baseLine = font->glyphBaselines[glyphIndex] * dimensions.y;
			AlignedRect glyphBox = AlignedRect(cursor.x, cursor.x + dimensions.x, cursor.y + dimensions.y - baseLine, cursor.y - baseLine);
			glyphBox = glyphBox.Translate(Vector2(maxWidth * -0.5f, 0.5f * rows));
			glyphBox.left /= maxWidth;
			glyphBox.right /= maxWidth;
			glyphBox.top /= rows;
			glyphBox.bottom /= rows;

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

	textMesh = app->graphics->CreateMesh(vertices, 4 * textLength, indices, 6 * textLength, false);
	blockAspectRatio = maxWidth / rows;
}