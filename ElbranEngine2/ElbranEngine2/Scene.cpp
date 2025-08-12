#include "Scene.h"
#include "Application.h"
#include "AssetContainer.h"
#include "GraphicsAPI.h"
#include "ShaderConstants.h"

// these match Lighting.hlsli
#define LIGHTS_REGISTER 127
#define LIGHT_INFO_REGISTER 1

Scene::Scene(uint16_t maxEntities, float cameraWidth) :
	ambientLight{Color::White},
	backgroundColor{Color::White},
	backgroundImage{nullptr},
	entityCount{0},
	maxEntities{maxEntities},
	camera{}
{
	transforms = new Transform[maxEntities];
	localMatrices = new Matrix[maxEntities];
	worldMatrices = new Matrix[maxEntities];

	opaques.Allocate(maxEntities);
	translucents.Allocate(maxEntities);
	behaviors.Allocate(maxEntities);
	lights.Allocate(maxEntities / 4);

	camera.viewWidth = cameraWidth;
	ReserveTransform(&camera.transform, &camera.worldMatrix);

	GraphicsAPI* graphics = app->graphics;

	projectionBuffer = graphics->CreateConstantBuffer(sizeof(Matrix));
	lightInfoBuffer = graphics->CreateConstantBuffer(sizeof(LightConstants));
	lightsBuffer = graphics->CreateArrayBuffer(ShaderDataType::Structured, MAX_LIGHTS_ONSCREEN, sizeof(LightData));
}

Scene::~Scene() {
	GraphicsAPI* graphics = app->graphics;
	graphics->ReleaseConstantBuffer(&projectionBuffer);
	graphics->ReleaseConstantBuffer(&lightInfoBuffer);
	graphics->ReleaseArrayBuffer(&lightsBuffer);

	delete[] transforms;
	delete[] localMatrices;
	delete[] worldMatrices;

	opaques.Release();
	translucents.Release();
	behaviors.Release();
	lights.Release();
}

void Scene::Update(float deltaTime) {
	uint32_t numBehaviors = behaviors.GetSize();
	for(uint32_t i = 0; i < numBehaviors; i++) {
		if(behaviors[i]->enabled) {
			behaviors[i]->Update(deltaTime);
		}
	}

	uint32_t numDeleted = toBeDeleted.GetSize();
	for(uint32_t i = 0; i < numDeleted; i++) {
		behaviors.RemoveAt(toBeDeleted[i] - behaviors[0]);
	}
	toBeDeleted.Clear();
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

		*currentGlobal = *currentLocal;
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

	// create projection matrix
	Vector2 cameraPosition = *camera.worldMatrix * Vector2::Zero;
	Matrix viewProjection =
		Matrix::ProjectOrthographic(camera.viewWidth, camera.GetViewHeight(), CAMERA_DEPTH) *
		Matrix::View(cameraPosition, camera.transform->rotation);

	// set the light data for pixel shaders
	LightData lightData[MAX_LIGHTS_ONSCREEN];
	LightConstants lightConstants;
	lightConstants.ambientLight = ambientLight;
	lightConstants.lightCount = 0;

	uint32_t totalLights = lights.GetSize();
	AlignedRect screenArea = AlignedRect(-1.f, 1.f, 1.f, -1.f);
	for(uint32_t i = 0; i < totalLights; i++) {
		// omit offscreen lights
		Vector2 closestPoint = lights[i].transform->position + (cameraPosition - lights[i].transform->position).SetLength(lights[i].radius);
		closestPoint = viewProjection * closestPoint;
		if(!(Circle(lights[i].transform->position, lights[i].radius).Contains(cameraPosition) || screenArea.Contains(closestPoint))) {
			continue;
		}

		// copy this light to the gpu
		int32_t nextLight = lightConstants.lightCount;
		lightData[nextLight].worldPosition = *(lights[i].worldMatrix) * Vector2::Zero;
		Vector2 forward = *(lights[i].worldMatrix) * Vector2::Right;
		lightData[nextLight].direction = (forward - lightData[i].worldPosition).Normalize();

		lightData[nextLight].color = lights[i].color;
		lightData[nextLight].radius = lights[i].radius;
		lightData[nextLight].brightness = lights[i].brightness;

		lightData[nextLight].coneEdge = *(lights[i].worldMatrix) * Vector2::FromAngle(lights[i].coneSize * 0.5f);

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
	uint32_t numOpaques = opaques.GetSize();
	for(uint32_t i = 1; i < numOpaques; i++) {
		uint32_t index = i;
		while(index > 0 && opaques[index]->worldMatrix->values[2][3] < opaques[index - 1]->worldMatrix->values[2][3]) { // global z is in row 3 col 4
			IRenderer* swapper = opaques[index - 1];
			opaques[index - 1] = opaques[index];
			opaques[index] = swapper;
			index--;
		}
	}

	for(uint32_t i = 0; i < numOpaques; i++) {
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
	uint32_t numTranslucents = translucents.GetSize();
	for(uint32_t i = 1; i < numTranslucents; i++) {
		uint32_t index = i;
		while(index > 0 && translucents[index]->worldMatrix->values[2][3] > translucents[index - 1]->worldMatrix->values[2][3]) { // global z is in row 3 col 4
			IRenderer* swapper = translucents[index - 1];
			translucents[index - 1] = translucents[index];
			translucents[index] = swapper;
			index--;
		}
	}

	graphics->SetBlendMode(BlendState::AlphaBlend);
	for(int i = 0; i < numTranslucents; i++) {
		translucents[i]->Draw();
	}
	graphics->SetBlendMode(BlendState::None);
}

void Scene::ReserveTransform(Transform** outTransform, const Matrix** outMatrix) {
	assert(entityCount < maxEntities && "scene capacity is unable to fit another transform");
	uint16_t newSlot = entityCount;
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
	behavior->scene = this;
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
	uint16_t openIndex = transform - transforms;
	openSlots.push_back(openIndex);

	// sort into the vector so that the largest index is the smallest integer
	for(size_t i = openSlots.size() - 1; i > 0; i--) {
		if(openSlots[i] < openSlots[i-1]) {
			return;
		}

		openSlots[i] = openSlots[i-1];
		openSlots[i-1] = openIndex;
	}
}

void Scene::RemoveRenderer(IRenderer* renderer) {
	if(renderer >= opaques[0] && renderer < opaques[opaques.GetSize()]) {
		opaques.RemoveAt(renderer - opaques[0]);
	}
	else if(renderer >= translucents[0] && renderer < translucents[translucents.GetSize()]) {
		translucents.RemoveAt(renderer - translucents[0]);
	}
	ReleaseTransform(renderer->transform);
}

void Scene::RemoveBehavior(IBehavior* behavior) {
	if(toBeDeleted.IndexOf(behavior) < 0) toBeDeleted.Add(behavior);
}

void Scene::RemoveLight(LightSource* light) {
	ReleaseTransform(light->transform);
	lights.RemoveAt(light - &lights[0]);
}

float Camera::GetViewHeight() const {
	return viewWidth / app->graphics->GetViewAspectRatio();
}

Vector2 Camera::GetWorldDimensions() const {
	return Vector2(viewWidth, viewWidth / app->graphics->GetViewAspectRatio());
}
