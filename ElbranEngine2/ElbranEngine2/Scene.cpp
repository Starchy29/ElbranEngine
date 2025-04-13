#include "Scene.h"
#include "Application.h"
#include "GraphicsAPI.h"

Scene::Scene() {
	GraphicsAPI* graphics = app->graphics;

	projectionBuffer = graphics->CreateConstantBuffer(sizeof(Matrix));
}

Scene::~Scene() {
	projectionBuffer.Release();
}

void Scene::Update(float deltaTime) {

}

void Scene::Draw() {
	GraphicsAPI* graphics = app->graphics;

	// set the projection matrix for vertex shaders
	Matrix viewProjection = 
		Matrix::ProjectOrthographic(camera.viewWidth, camera.viewWidth * app->graphics->GetViewAspectRatio(), CAMERA_DEPTH) *
		Matrix::View(camera.position, camera.rotation);
	graphics->WriteBuffer(&viewProjection, sizeof(Matrix), projectionBuffer.data);
	graphics->SetConstants(ShaderStage::Vertex, &projectionBuffer, 1);

	// set the light data for pixel shaders

	// draw opaques front to back

	// draw the background

	// draw translucents back to front
}
