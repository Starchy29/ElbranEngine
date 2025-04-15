#include "Scene.h"
#include "Application.h"
#include "AssetContainer.h"
#include "GraphicsAPI.h"

Scene::Scene(float cameraWidth) {
	camera = {};
	camera.viewWidth = cameraWidth;

	GraphicsAPI* graphics = app->graphics;

	projectionBuffer = graphics->CreateConstantBuffer(sizeof(Matrix));

	testRenderer = new SpriteRenderer(&app->assets->testSprite);

	// TEST CODE
	Vector2 topLeft = Vector2(-0.5f, 0.5f);
	Vector2 topRight = Vector2(0.5f, 0.5f);
	Vector2 botLeft = Vector2(-0.5f, -0.5f);
	Vector2 botRight = Vector2(0.5f, -0.5f);

	topLeft = Matrix::Translate(0.2f, 0.f) * topLeft;
	topRight = Matrix::Translate(0.2f, 0.f) * topRight;
	botLeft = Matrix::Translate(0.2f, 0.f) * botLeft;
	botRight = Matrix::Translate(0.2f, 0.f) * botRight;

	topLeft = Matrix::View(camera.position, camera.rotation) * topLeft;
	topRight = Matrix::View(camera.position, camera.rotation) * topRight;
	botLeft = Matrix::View(camera.position, camera.rotation) * botLeft;
	botRight = Matrix::View(camera.position, camera.rotation) * botRight;

	topLeft = Matrix::ProjectOrthographic(camera.viewWidth, camera.viewWidth / app->graphics->GetViewAspectRatio(), CAMERA_DEPTH) * topLeft;
	topRight = Matrix::ProjectOrthographic(camera.viewWidth, camera.viewWidth / app->graphics->GetViewAspectRatio(), CAMERA_DEPTH) * topRight;
	botLeft = Matrix::ProjectOrthographic(camera.viewWidth, camera.viewWidth / app->graphics->GetViewAspectRatio(), CAMERA_DEPTH) * botLeft;
	botRight = Matrix::ProjectOrthographic(camera.viewWidth, camera.viewWidth / app->graphics->GetViewAspectRatio(), CAMERA_DEPTH) * botRight;

	Matrix fullMatrix = Matrix::ProjectOrthographic(camera.viewWidth, camera.viewWidth / app->graphics->GetViewAspectRatio(), CAMERA_DEPTH) *
		Matrix::View(camera.position, camera.rotation) *
		Matrix::Translate(0.2f, 0.f);
	// END TEST
}

Scene::~Scene() {
	projectionBuffer.Release();
	delete testRenderer;
}

void Scene::Update(float deltaTime) {
	t += deltaTime;
}

void Scene::Draw() {
	// convert all transforms into matrices

	GraphicsAPI* graphics = app->graphics;

	// set the projection matrix for vertex shaders
	Matrix viewProjection =
		(Matrix::ProjectOrthographic(camera.viewWidth, camera.viewWidth / app->graphics->GetViewAspectRatio(), CAMERA_DEPTH)
		* Matrix::View(camera.position, camera.rotation)).Transpose();
	graphics->WriteBuffer(&viewProjection, sizeof(Matrix), projectionBuffer.data);
	graphics->SetConstants(ShaderStage::Vertex, &projectionBuffer, 1);

	// set the light data for pixel shaders

	// draw opaques front to back
	testRenderer->Draw(Vector2(3.f * sinf(t), 0.f));

	// draw the background
	Color backgroundColor = Color(0.2f, 0.2f, 0.2f);
	graphics->WriteBuffer(&backgroundColor, sizeof(Color), app->assets->solidColorPS.constants.data);
	graphics->SetConstants(ShaderStage::Pixel, &app->assets->solidColorPS.constants, 0);
	graphics->SetPixelShader(&app->assets->solidColorPS);
	graphics->DrawFullscreen();

	// draw translucents back to front
}
