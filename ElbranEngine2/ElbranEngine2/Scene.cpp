#include "Scene.h"
#include "Application.h"
#include "AssetContainer.h"
#include "GraphicsAPI.h"
#include "ShaderConstants.h"

// these match Lighting.hlsli
#define LIGHTS_REGISTER 127
#define LIGHT_INFO_REGISTER 1

Scene::Scene(unsigned int maxEntities, float cameraWidth)  {
	camera = {};
	camera.viewWidth = cameraWidth;
	ambientLight = Colors::White;
	backgroundColor = Color(0.1f, 0.1f, 0.1f);
	backgroundImage = nullptr;

	entityCount = 0;
	transforms = new Transform[maxEntities];
	worldMatrices = new Matrix[maxEntities];
	sprites = FixedList<SpriteRenderer*>(maxEntities);
	behaviors = FixedList<IBehavior*>(maxEntities);
	lights = FixedList<LightSource>(maxEntities);

	GraphicsAPI* graphics = app->graphics;

	projectionBuffer = graphics->CreateConstantBuffer(sizeof(Matrix));
	lightInfoBuffer = graphics->CreateConstantBuffer(sizeof(LightConstants));
	lightsBuffer = graphics->CreateArrayBuffer(MAX_LIGHTS_ONSCREEN, sizeof(LightData), true);
}

Scene::~Scene() {
	projectionBuffer.Release();
	lightInfoBuffer.Release();
	lightsBuffer.Release();

	delete[] transforms;
	delete[] worldMatrices;
	for(int i = 0; i < sprites.GetSize(); i++) {
		delete sprites[i];
	}
	delete[] sprites.dataArray;
	for(int i = 0; i < behaviors.GetSize(); i++) {
		delete behaviors[i];
	}
	delete[] behaviors.dataArray;
	delete[] lights.dataArray;
}

void Scene::Update(float deltaTime) {
	t += deltaTime;
	transforms[0].position.x = sinf(t);

	for(int i = 0; i < behaviors.GetSize(); i++) {
		// behaviors may be added or deleted mid-update
		behaviors[i]->Update(deltaTime);
	}
}

void Scene::Draw() {
	// convert all transforms into matrices
	for(int i = 0; i < entityCount; i++) {
		Transform* transform = &transforms[i];
		worldMatrices[i] = 
			(Matrix::Translate(transform->position.x, transform->position.y, transform->zOrder) *
			Matrix::Rotation(transform->rotation) *
			Matrix::Scale(transform->scale.x, transform->scale.y))
			.Transpose();
	}

	GraphicsAPI* graphics = app->graphics;

	// set the projection matrix for vertex shaders
	Matrix viewProjection =
		(Matrix::ProjectOrthographic(camera.viewWidth, camera.viewWidth / app->graphics->GetViewAspectRatio(), CAMERA_DEPTH)
		* Matrix::View(camera.position, camera.rotation)).Transpose();
	graphics->WriteBuffer(&viewProjection, sizeof(Matrix), projectionBuffer.data);
	graphics->SetConstants(ShaderStage::Vertex, &projectionBuffer, 1);

	// set the light data for pixel shaders
	LightData lightData[MAX_LIGHTS_ONSCREEN];
	LightConstants lightConstants;
	lightConstants.ambientLight = ambientLight;
	lightConstants.lightCount = 0;

	int totalLights = lights.GetSize();
	for(int i = 0; i < totalLights; i++) {
		// TODO: copy light data

		if(lightConstants.lightCount >= MAX_LIGHTS_ONSCREEN) {
			break;
		}
	}

	graphics->WriteBuffer(&lightData, sizeof(LightData) * MAX_LIGHTS_ONSCREEN, lightsBuffer.buffer);
	graphics->SetArray(ShaderStage::Pixel, &lightsBuffer, LIGHTS_REGISTER);

	graphics->WriteBuffer(&lightConstants, sizeof(LightConstants), lightInfoBuffer.data);
	graphics->SetConstants(ShaderStage::Pixel, &lightInfoBuffer, LIGHT_INFO_REGISTER);

	// draw opaques front to back
	int numOpaques = sprites.GetSize();
	for(int i = 0; i < numOpaques; i++) {
		sprites[i]->Draw();
	}

	// draw the background
	if(backgroundImage) {
		TexturePSConstants psInput;
		psInput.tint = backgroundColor;
		psInput.lit = false;

		graphics->WriteBuffer(&psInput, sizeof(TexturePSConstants), app->assets->texturePS.constants.data);
		graphics->SetConstants(ShaderStage::Pixel, &app->assets->texturePS.constants, 0);
		graphics->SetTexture(ShaderStage::Pixel, backgroundImage, 0);
		graphics->SetPixelShader(&app->assets->texturePS);
	} else {
		graphics->WriteBuffer(&backgroundColor, sizeof(Color), app->assets->solidColorPS.constants.data);
		graphics->SetConstants(ShaderStage::Pixel, &app->assets->solidColorPS.constants, 0);
		graphics->SetPixelShader(&app->assets->solidColorPS);
	}
	graphics->DrawFullscreen();

	// draw translucents back to front
}

void Scene::CreateTransform(Transform** outTransform, const Matrix** outMatrix) {
	int newSlot = entityCount;
	if(openSlots.size() > 0) {
		newSlot = openSlots[openSlots.size() - 1];
		openSlots.pop_back();
	}

	entityCount++;

	transforms[newSlot] = {};
	transforms[newSlot].scale = Vector2(1.f, 1.f);
	worldMatrices[newSlot] = Matrix::Identity;

	if(outTransform) {
		*outTransform = &transforms[newSlot];
	}
	if(outMatrix) {
		*outMatrix = &worldMatrices[newSlot];
	}
}

SpriteRenderer* Scene::AddSprite(Texture2D* sprite) {
	SpriteRenderer* added = new SpriteRenderer(sprite);
	sprites.Add(added);
	CreateTransform(&added->transform, &added->worldMatrix);
	return added;
}

void Scene::ReleaseTransform(Transform* transform) {
	int openIndex = transform - transforms; // pointer difference is the index
	openSlots.push_back(openIndex);

	// sort into the vector so that the largest index is the smallest integer
	for(int i = openSlots.size() - 1; i > 0; i--) {
		if(openSlots[i] < openSlots[i-1]) {
			return;
		}

		openSlots[i] = openSlots[i-1];
		openSlots[i-1] = openIndex;
	}
}

void Scene::RemoveSprite(SpriteRenderer* sprite) {
	int index = sprite - sprites.dataArray[0];
	assert(index >= 0 && index < sprites.GetSize() && "attempted to remove a sprite that was not added");
	ReleaseTransform(sprite->transform);
	sprites.RemoveAt(index);
	delete sprite;
}
