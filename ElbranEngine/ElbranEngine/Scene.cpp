#include "Scene.h"
#include "Application.h"

Scene::Scene(float cameraWidth, Color backgroundColor) {
	paused = false;
	camera = new Camera(cameraWidth);

	backColor = backgroundColor;
	backImage = nullptr;
}

Scene::Scene(float cameraWidth, std::shared_ptr<Sprite> backgroundImage) {
	paused = false;
	camera = new Camera(cameraWidth);

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
}

Camera* Scene::GetCamera() {
	return camera;
}

void Scene::Update(float deltaTime) {
	if(paused) {
		return;
	}

	// update all game objects
	for(GameObject* object : opaques) {
		if(object->active && !object->toBeDeleted) {
			object->Update(deltaTime);
		}
	}
	for(GameObject* object : translucents) {
		if(object->active && !object->toBeDeleted) {
			object->Update(deltaTime);
		}
	}
	for(GameObject* object : texts) {
		if(object->active && !object->toBeDeleted) {
			object->Update(deltaTime);
		}
	}

	// remove objects that need to be deleted
	for(int i = opaques.size() - 1; i >= 0; i--) {
		if(opaques[i]->toBeDeleted) {
			delete opaques[i];
			opaques.erase(std::next(opaques.begin(), i));
		}
	}
	for(int i = translucents.size() - 1; i >= 0; i--) {
		if(translucents[i]->toBeDeleted) {
			delete translucents[i];
			translucents.erase(std::next(translucents.begin(), i));
		}
	}
	for(int i = texts.size() - 1; i >= 0; i--) {
		if(texts[i]->toBeDeleted) {
			delete texts[i];
			texts.erase(std::next(texts.begin(), i));
		}
	}
}

void Scene::Draw() {
	DXCore* directX = APP->Graphics();

	// draw opaques front to back
	directX->SetAlphaBlend(false);
	for(GameObject* object : opaques) {
		if(object->visible) {
			object->Draw(camera);
		}
	}

	DrawBackground();

	// draw all text
	if(texts.size() > 0) {
		directX->StartTextBatch();
		for(GameObject* text : texts) {
			if(text->visible) {
				text->Draw(camera);
			}
		}
		directX->FinishTextBatch();
	}

	// draw translucents back to front
	directX->SetAlphaBlend(true);
	for(int i = translucents.size() - 1; i >= 0; i--) {
		if(translucents[i]->visible) {
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
	list->erase(std::find(list->begin(), list->end(), sceneMember));
	SortInto(sceneMember, list);
}

void Scene::DrawBackground() {
	if(backColor.alpha <= 0) {
		return;
	}

	const AssetManager* assets = APP->Assets();

	if(backImage != nullptr) {
		std::shared_ptr<PixelShader> imageShader = assets->imagePS;
		imageShader->SetConstantVariable("color", &backColor);
		imageShader->SetTexture(backImage->GetResourceView());
		imageShader->SetSampler(assets->defaultSampler);
		imageShader->SetShader();
	} else {
		std::shared_ptr<PixelShader> colorShader = assets->colorPS;
		colorShader->SetConstantVariable("color", &backColor);
		colorShader->SetShader();
	}
	assets->backgroundVS->SetShader();
	APP->Graphics()->GetContext()->Draw(3, 0);
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
