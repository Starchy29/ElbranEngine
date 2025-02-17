#include "Scene.h"
#include "Application.h"
#include "DXCore.h"
#include "LightRenderer.h"
#include "AssetManager.h"
#include "Sprite.h"
#include "PixelShader.h"
#include "ObjectList.h"

Scene::Scene(float cameraWidth, Color backgroundColor) {
	camera = new Camera(cameraWidth);
	ambientLight = Color(0.4f, 0.4f, 0.4f);
	sceneMembers = new ObjectList();
	opaques = new ObjectList();
	translucents = new ObjectList();
	texts = new ObjectList();
	lightObjects = new ObjectList();

	backColor = backgroundColor;
	backImage = nullptr;
}

Scene::Scene(float cameraWidth, std::shared_ptr<Sprite> backgroundImage) {
	camera = new Camera(cameraWidth);
	ambientLight = Color(0.4f, 0.4f, 0.4f);
	sceneMembers = new ObjectList();
	opaques = new ObjectList();
	translucents = new ObjectList();
	texts = new ObjectList();
	lightObjects = new ObjectList();

	backColor = Color::White;
	backImage = backgroundImage;
}

Scene::~Scene() {
	delete camera;
	ObjectNode* node = sceneMembers->GetFirst();
	while(node != nullptr) {
		delete node->object;
		node = node->next;
	}
	delete sceneMembers;
	delete opaques;
	delete translucents;
	delete texts;
	delete lightObjects;
}

Camera* Scene::GetCamera() {
	return camera;
}

void Scene::Update(float deltaTime) {
	ObjectNode* node = sceneMembers->GetFirst();
	while(node) {
		if(node->object->IsActive()) {
			node->object->Update(deltaTime);
		}
		node = node->next;
	}

	// delay deletion in case an object wants to delete itself
	for(GameObject* deleter : toBeDeleted) {
		delete deleter;
	}
	toBeDeleted.clear();
}

void Scene::Draw() {
	DXCore* directX = APP->Graphics();

	// set all the lights
	int lightsOnScreen = 0;
	RectangleBox screenArea = camera->GetVisibleArea();
	ObjectNode* node = lightObjects->GetFirst();
	while(node) {
		GameObject* lightObject = node->object;
		LightRenderer* lightData = lightObject->GetRenderer<LightRenderer>();

		// ignore lights that are off screen
		Vector2 lightCenter = lightObject->GetTransform()->GetPosition(true);
		if(!screenArea.Expand(lightData->radius).Contains(lightCenter)) {
			continue;
		}

		lights[lightsOnScreen].worldPosition = lightCenter;
		lights[lightsOnScreen].rotation = lightObject->GetTransform()->GetRotation(true);

		lights[lightsOnScreen].color = lightData->color;
		lights[lightsOnScreen].brightness = lightData->brightness;
		lights[lightsOnScreen].radius = lightData->radius;
		lights[lightsOnScreen].coneSize = lightData->coneSize;

		lightsOnScreen++;

		if(lightsOnScreen >= MAX_LIGHTS_ONSCREEN) {
			break;
		}

		node = node->next;
	}

	directX->SetLights(lights, lightsOnScreen, ambientLight);

	// draw opaques front to back
	directX->SetBlendMode(BlendState::None);
	node = opaques->GetFirst();
	while(node) {
		if(node->object->IsActive()) {
			node->object->Draw(camera);
		}
		node = node->next;
	}

	DrawBackground();

	// draw all text
	node = texts->GetFirst();
	if(node != nullptr) {
		directX->StartTextBatch();
		while(node) {
			if(node->object->IsActive()) {
				node->object->Draw(camera);
			}
			node = node->next;
		}
		directX->FinishTextBatch();
	}

	// draw translucents back to front
	directX->SetBlendMode(BlendState::AlphaBlend);
	node = translucents->GetFirst();
	while(node) {
		if(node->object->IsActive()) {
			node->object->Draw(camera);
		}
		node = node->next;
	}
}

void Scene::Add(GameObject* object) {
	if(object->scene != nullptr) {
		return;
	}

	// add it to the scene
	object->scene = this;
	sceneMembers->AddToFront(object);

	// sort it into the correct render list
	switch(object->renderMode) {
	case RenderMode::Opaque:
		opaques->SortInto(object, true);
		break;
	case RenderMode::Translucent:
		translucents->SortInto(object, false);
		break;
	case RenderMode::Text:
		texts->AddToFront(object);
		break;
	case RenderMode::Light:
		lightObjects->AddToFront(object);
		break;
	}

	// add all of its children to the same scene
	for(GameObject* child : object->children) {
		Add(child);
	}
}

void Scene::Remove(GameObject* removed) {
	sceneMembers->Remove(removed);

	switch(removed->renderMode) {
	case RenderMode::Opaque:
		opaques->Remove(removed);
		break;
	case RenderMode::Translucent:
		translucents->Remove(removed);
		break;
	case RenderMode::Text:
		texts->Remove(removed);
		break;
	case RenderMode::Light:
		lightObjects->Remove(removed);
		break;
	}

	toBeDeleted.push_back(removed);
}

void Scene::Remove(GameObject* removed) {
	sceneMembers->Remove(removed);

	switch(removed->renderMode) {
	case RenderMode::Opaque:
		opaques->Remove(removed);
		break;
	case RenderMode::Translucent:
		translucents->Remove(removed);
		break;
	case RenderMode::Text:
		texts->Remove(removed);
		break;
	case RenderMode::Light:
		lightObjects->Remove(removed);
		break;
	}

	toBeDeleted.push_back(removed);
}

// called by a game object when it changes its Z coordinate
void Scene::UpdateDrawOrder(GameObject* sceneMember) {
	switch(sceneMember->renderMode) {
	case RenderMode::Opaque:
		opaques->Remove(sceneMember);
		opaques->SortInto(sceneMember, true);
		break;
	case RenderMode::Translucent:
		translucents->Remove(sceneMember);
		translucents->SortInto(sceneMember, false);
		break;
	}
}

void Scene::DrawBackground() {
	if(backColor.alpha <= 0) {
		return;
	}

	const AssetManager* assets = APP->Assets();

	if(backImage != nullptr) {
		assets->imagePS->SetConstantVariable("color", &backColor);
		assets->imagePS->SetTexture(backImage->GetResourceView());
		assets->imagePS->SetSampler(assets->defaultSampler);
		assets->imagePS->SetShader();
	} else {
		assets->colorPS->SetConstantVariable("color", &backColor);
		assets->colorPS->SetShader();
	}
	
	APP->Graphics()->DrawScreen();
}
