#include "Scene.h"
#include "Application.h"
#include "DXCore.h"
#include "LightRenderer.h"
#include "AssetManager.h"
#include "Sprite.h"
#include "PixelShader.h"

Scene::Scene(float cameraWidth, Color backgroundColor) {
	camera = new Camera(cameraWidth);
	ambientLight = Color(0.4f, 0.4f, 0.4f);

	backColor = backgroundColor;
	backImage = nullptr;
}

Scene::Scene(float cameraWidth, std::shared_ptr<Sprite> backgroundImage) {
	camera = new Camera(cameraWidth);
	ambientLight = Color(0.4f, 0.4f, 0.4f);

	backColor = Color::White;
	backImage = backgroundImage;
}

Scene::~Scene() {
	delete camera;
	for(GameObject* object : opaques) {
		delete object;
	}
	for(GameObject* object : translucents) {
		delete object;
	}
	for(GameObject* object : texts) {
		delete object;
	}
	for(GameObject* object : lightObjects) {
		delete object;
	}
}

Camera* Scene::GetCamera() {
	return camera;
}

void Scene::Update(float deltaTime) {
	// update all game objects
	for(GameObject* object : opaques) {
		if(object->IsActive()) {
			object->Update(deltaTime);
		}
	}
	for(GameObject* object : translucents) {
		if(object->IsActive()) {
			object->Update(deltaTime);
		}
	}
	for(GameObject* object : texts) {
		if(object->IsActive()) {
			object->Update(deltaTime);
		}
	}
	for(GameObject* object : lightObjects) {
		if(object->IsActive()) {
			object->Update(deltaTime);
		}
	}

	// remove objects that need to be deleted
	for(int i = (int)opaques.size() - 1; i >= 0; i--) {
		if(opaques[i]->toBeDeleted) {
			Remove(opaques[i]);
			delete opaques[i];
			opaques.erase(std::next(opaques.begin(), i));
		}
	}
	for(int i = (int)translucents.size() - 1; i >= 0; i--) {
		if(translucents[i]->toBeDeleted) {
			Remove(translucents[i]);
			delete translucents[i];
			translucents.erase(std::next(translucents.begin(), i));
		}
	}
	for(int i = (int)texts.size() - 1; i >= 0; i--) {
		if(texts[i]->toBeDeleted) {
			Remove(texts[i]);
			delete texts[i];
			texts.erase(std::next(texts.begin(), i));
		}
	}
	for(int i = (int)lightObjects.size() - 1; i >= 0; i--) {
		if(lightObjects[i]->toBeDeleted) {
			Remove(lightObjects[i]);
			delete lightObjects[i];
			lightObjects.erase(std::next(lightObjects.begin(), i));
		}
	}
}

void Scene::Draw() {
	DXCore* directX = APP->Graphics();

	// set all the lights
	int lightsOnScreen = 0;
	RectangleBox screenArea = camera->GetVisibleArea();
	for(int i = 0; i < lightObjects.size(); i++) {
		GameObject* lightObject = lightObjects[i];
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
	}

	directX->SetLights(lights, lightsOnScreen, ambientLight);

	// draw opaques front to back
	directX->SetBlendMode(BlendState::None);
	for(GameObject* object : opaques) {
		if(object->IsActive()) {
			object->Draw(camera);
		}
	}

	DrawBackground();

	// draw all text
	if(texts.size() > 0) {
		directX->StartTextBatch();
		for(GameObject* text : texts) {
			if(text->IsActive()) {
				text->Draw(camera);
			}
		}
		directX->FinishTextBatch();
	}

	// draw translucents back to front
	directX->SetBlendMode(BlendState::AlphaBlend);
	for(int i = (int)translucents.size() - 1; i >= 0; i--) {
		if(translucents[i]->IsActive()) {
			translucents[i]->Draw(camera);
		}
	}
}

void Scene::Add(GameObject* object) {
	if(object->scene != nullptr) {
		return;
	}

	object->scene = this;

	for(GameObject* child : object->children) {
		Add(child);
	}

	RenderMode renderMode = object->renderMode;
	if(renderMode == RenderMode::Text) {
		// text ordering is done by the spriteBatch
		texts.push_back(object);
		return;
	}
	else if(renderMode == RenderMode::Light) {
		// lights are not sorted
		lightObjects.push_back(object);
		return;
	}

	SortInto(object, renderMode == RenderMode::Translucent ? &translucents : &opaques);
}

// called by a game object when it changes its Z coordinate
void Scene::UpdateDrawOrder(GameObject* sceneMember) {
	RenderMode renderMode = sceneMember->renderMode;
	std::vector<GameObject*>* list = &opaques;
	if(renderMode == RenderMode::Translucent) {
		list = &translucents;
	}
	else if(renderMode == RenderMode::Text) {
		list = &texts;
	}
	else if(renderMode == RenderMode::Light) {
		list = &lightObjects;
	}
	list->erase(std::find(list->begin(), list->end(), sceneMember));
	SortInto(sceneMember, list);
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

void Scene::SortInto(GameObject* sceneMember, std::vector<GameObject*>* objectList) {
	if(objectList->size() == 0) {
		objectList->push_back(sceneMember);
		return;
	}

	float newZ = sceneMember->GetTransform()->GetGlobalZ();
	int insertIndex = 0;
	while(insertIndex < objectList->size() && newZ > (*objectList)[insertIndex]->GetTransform()->GetGlobalZ()) {
		insertIndex++;
	}
	objectList->insert(std::next(objectList->begin(), insertIndex), sceneMember);
}
