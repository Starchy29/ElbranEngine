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
	ambientLight = Color::White;
	backgroundColor = Color(1.f, 1.f, 1.f);
	backgroundImage = nullptr;

	entityCount = 0;
	transforms = new Transform[maxEntities];
	localMatrices = new Matrix[maxEntities];
	worldMatrices = new Matrix[maxEntities];

	opaques = FixedList<IRenderer*>(maxEntities);
	translucents = FixedList<IRenderer*>(maxEntities);
	behaviors = FixedList<IBehavior*>(maxEntities);
	lights = FixedList<LightSource>(maxEntities / 4);

	GraphicsAPI* graphics = app->graphics;

	projectionBuffer = graphics->CreateConstantBuffer(sizeof(Matrix));
	lightInfoBuffer = graphics->CreateConstantBuffer(sizeof(LightConstants));
	lightsBuffer = graphics->CreateArrayBuffer(ShaderDataType::Structured, MAX_LIGHTS_ONSCREEN, sizeof(LightData));
}

Scene::~Scene() {
	projectionBuffer.Release();
	lightInfoBuffer.Release();
	lightsBuffer.Release();

	delete[] transforms;
	delete[] localMatrices;
	delete[] worldMatrices;
	for(unsigned int i = 0; i < opaques.GetSize(); i++) {
		delete opaques[i];
	}
	delete[] opaques.dataArray;
	for(unsigned int i = 0; i < translucents.GetSize(); i++) {
		delete translucents[i];
	}
	delete[] translucents.dataArray;
	for(unsigned int i = 0; i < behaviors.GetSize(); i++) {
		delete behaviors[i];
	}
	delete[] behaviors.dataArray;
	delete[] lights.dataArray;
}

void Scene::Update(float deltaTime) {
	for(unsigned int i = 0; i < behaviors.GetSize(); i++) {
		// behaviors may be added or deleted mid-update
		behaviors[i]->Update(deltaTime);
	}
}

void Scene::Draw() {
	GraphicsAPI* graphics = app->graphics;
	
	// convert all transforms into matrices
	Transform* lastTransform = &transforms[entityCount];
	Matrix* currentLocal = localMatrices;
	Matrix* currentGlobal = worldMatrices;
	for(Transform* transform = transforms; transform < lastTransform; transform++) {
		*currentLocal =
			Matrix::Translation(transform->position.x, transform->position.y, transform->zOrder) *
			Matrix::Rotation(transform->rotation) *
			Matrix::Scale(transform->scale.x, transform->scale.y);

		memcpy(currentGlobal, currentLocal, sizeof(Matrix));
		currentLocal++;
		currentGlobal++;
	}

	// convert from local to global matrices
	currentGlobal = worldMatrices;
	for(Transform* transform = transforms; transform < lastTransform; transform++) {
		Transform* parent = transform->parent;
		while(parent) {
			*currentGlobal = localMatrices[parent - transforms] * (*currentGlobal);
			parent = parent->parent;
		}
		currentGlobal++;
	}

	// set the light data for pixel shaders
	Matrix viewProjection =
		Matrix::ProjectOrthographic(camera.viewWidth, camera.GetViewHeight(), CAMERA_DEPTH) *
		Matrix::View(camera.position, camera.rotation);

	LightData lightData[MAX_LIGHTS_ONSCREEN];
	LightConstants lightConstants;
	lightConstants.ambientLight = ambientLight;
	lightConstants.lightCount = 0;

	int totalLights = lights.GetSize();
	AlignedRect screenArea = AlignedRect(-1.f, 1.f, 1.f, -1.f);
	for(int i = 0; i < totalLights; i++) {
		// omit offscreen lights
		Vector2 closestPoint = lights[i].transform->position + (camera.position - lights[i].transform->position).SetLength(lights[i].radius);
		closestPoint = viewProjection * closestPoint;
		if(!(Circle(lights[i].transform->position, lights[i].radius).Contains(camera.position) || screenArea.Contains(closestPoint))) {
			continue;
		}

		// copy this light to the gpu
		int nextLight = lightConstants.lightCount;
		lightData[nextLight].worldPosition = *(lights[i].worldMatrix) * Vector2::Zero;
		Vector2 forward = *(lights[i].worldMatrix) * Vector2::Right;
		lightData[nextLight].direction = (forward - lightData[i].worldPosition).Normalize();

		lightData[nextLight].color = lights[i].color;
		lightData[nextLight].radius = lights[i].radius;
		lightData[nextLight].brightness = lights[i].brightness;

		lightData[nextLight].coneEdge = *(lights[i].worldMatrix) * Vector2::FromAngle(lights[i].coneSize / 2.0f);

		lightConstants.lightCount++;
		if(lightConstants.lightCount >= MAX_LIGHTS_ONSCREEN) {
			break;
		}
	}

	graphics->WriteBuffer(&lightData, sizeof(LightData) * MAX_LIGHTS_ONSCREEN, lightsBuffer.buffer);
	graphics->SetArray(ShaderStage::Pixel, &lightsBuffer, LIGHTS_REGISTER);

	graphics->WriteBuffer(&lightConstants, sizeof(LightConstants), lightInfoBuffer.data);
	graphics->SetConstants(ShaderStage::Pixel, &lightInfoBuffer, LIGHT_INFO_REGISTER);

	// set the projection matrix for vertex shaders
	viewProjection = viewProjection.Transpose();
	graphics->WriteBuffer(&viewProjection, sizeof(Matrix), projectionBuffer.data);
	graphics->SetConstants(ShaderStage::Vertex, &projectionBuffer, 1);
	graphics->SetConstants(ShaderStage::Geometry, &projectionBuffer, 1);

	// draw opaques front to back
	int numOpaques = opaques.GetSize();
	for(int i = 0; i < numOpaques; i++) {
		opaques[i]->Draw();
	}

	// draw the background
	if(backgroundImage) {
		TexturePSConstants psInput;
		psInput.tint = backgroundColor;
		psInput.lit = false;

		graphics->SetTexture(ShaderStage::Pixel, backgroundImage, 0);
		graphics->SetPixelShader(&app->assets->texturePS, &psInput, sizeof(TexturePSConstants));
	} else {
		graphics->SetPixelShader(&app->assets->solidColorPS, &backgroundColor, sizeof(Color));
	}
	graphics->DrawFullscreen();

	// draw translucents back to front
	graphics->SetBlendMode(BlendState::AlphaBlend);
	int numTranslucents = translucents.GetSize();
	for(int i = 0; i < numTranslucents; i++) {
		translucents[i]->Draw();
	}
	graphics->SetBlendMode(BlendState::None);
}

void Scene::ReserveTransform(Transform** outTransform, const Matrix** outMatrix) {
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

void Scene::AddRenderer(IRenderer* renderer, bool translucent) {
	ReserveTransform(&renderer->transform, &renderer->worldMatrix);
	if(translucent) {
		translucents.Add(renderer);
	} else {
		opaques.Add(renderer);
	}
}

void Scene::AddBehavior(IBehavior* behavior) {
	behaviors.Add(behavior);
}

LightSource* Scene::AddLight(Color color, float radius) {
	LightSource added;
	added.color = color;
	added.radius = radius;
	added.brightness = 1.0f;
	added.coneSize = 2.0f * PI;

	ReserveTransform(&added.transform, &added.worldMatrix);
	lights.Add(added);

	return &lights[lights.GetSize() - 1];
}

void Scene::ReleaseTransform(Transform* transform) {
	int openIndex = transform - transforms;
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

void Scene::RemoveRenderer(IRenderer* renderer) {
	unsigned int index = -1;
	if(renderer >= opaques.dataArray[0] && renderer <= opaques.dataArray[opaques.GetSize() - 1]) {
		index = renderer - opaques.dataArray[0];
		opaques.RemoveAt(index);
	}
	else if(renderer >= translucents.dataArray[0] && renderer <= translucents.dataArray[translucents.GetSize() - 1]) {
		index = renderer - translucents.dataArray[0];
		translucents.RemoveAt(index);
	}
	assert(index >= 0 && "attempted to remove a renderer that was not added");
	ReleaseTransform(renderer->transform);
}

void Scene::RemoveBehavior(IBehavior* behavior) {
	behaviors.RemoveAt(behavior - behaviors.dataArray[0]);
}

void Scene::RemoveLight(LightSource* light) {
	unsigned int index = light - lights.dataArray;
	assert(index >= 0 && index < lights.GetSize() && "attempted to remove a light that was not added");
	ReleaseTransform(light->transform);
	lights.RemoveAt(index);
}

float Camera::GetViewHeight() const {
	return viewWidth / app->graphics->GetViewAspectRatio();
}

Vector2 Camera::GetWorldDimensions() const {
	return Vector2(viewWidth, viewWidth / app->graphics->GetViewAspectRatio());
}
