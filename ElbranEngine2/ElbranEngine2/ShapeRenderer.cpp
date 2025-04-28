#include "ShapeRenderer.h"
#include "Application.h"
#include "GraphicsAPI.h"
#include "AssetContainer.h"
#include "ShaderConstants.h"

ShapeRenderer::ShapeRenderer(Shape shape, Color color) {
	this->shape = shape;
	this->color = color;
}

void ShapeRenderer::Draw() {
	GraphicsAPI* graphics = app->graphics;

	CameraVSConstants vsInput = {};
	vsInput.worldTransform = worldMatrix->Transpose();
	graphics->SetVertexShader(&app->assets->cameraVS, &vsInput, sizeof(CameraVSConstants));
	graphics->SetPixelShader(shape == Shape::Circle ? &app->assets->circleFillPS : &app->assets->solidColorPS, &color, sizeof(Color));
	graphics->DrawMesh(shape == Shape::Triangle ? &app->assets->unitTriangle : &app->assets->unitSquare);
}
