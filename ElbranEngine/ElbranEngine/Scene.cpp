#include "Scene.h"
#include "AssetManager.h"
#include "NewGame.h"

Scene::Scene(float cameraWidth) {
	paused = false;
	hidden = false;
	camera = new Camera(cameraWidth);

	hasBackground = false;
}

Scene::Scene(float cameraWidth, Color backgroundColor) {
	paused = false;
	hidden = false;
	camera = new Camera(cameraWidth);

	hasBackground = true;
	backColor = backgroundColor;
	backImage = nullptr;
}

Scene::Scene(float cameraWidth, std::shared_ptr<Sprite> backgroundImage) {
	paused = false;
	hidden = false;
	camera = new Camera(cameraWidth);

	hasBackground = true;
	backColor = WHITE;
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
}

Camera* Scene::GetCamera() {
	return camera;
}

void Scene::Update(float deltaTime) {
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
}

void Scene::Draw() {
	// draw opaques front to back
	GameInstance->DisableAlpha();
	for(GameObject* object : opaques) {
		if(object->visible) {
			object->Draw(camera);
		}
	}
	
	DrawBackground();

	// draw translucents back to front
	GameInstance->EnableAlpha();
	for(int i = translucents.size() - 1; i >= 0; i--) {
		if(translucents[i]->visible) {
			translucents[i]->Draw(camera);
		}
	}
}

void Scene::Join(GameObject* object) {
	SortInto(object, object->IsTranslucent() ? translucents : opaques);
}

// called by a game object when it changes its Z coordinate
void Scene::UpdateDrawOrder(GameObject* sceneMember) {
	std::vector<GameObject*> & list = sceneMember->IsTranslucent() ? translucents : opaques;
	list.erase(std::find(list.begin(), list.end(), sceneMember));
	SortInto(sceneMember, list);
}

inline void Scene::DrawBackground() {
	if(!hasBackground) {
		return;
	}

	AssetManager* assets = AssetManager::GetInstance();

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
	assets->unitSquare->Draw();
}

void Scene::SortInto(GameObject* sceneMember, std::vector<GameObject*> & objectList) {
	if(objectList.size() == 0) {
		objectList.push_back(sceneMember);
		return;
	}

	float newZ = sceneMember->GetTransform()->GetGlobalZ();
	int insertIndex = 0;
	while(insertIndex < objectList.size() && newZ > objectList[insertIndex]->GetTransform()->GetGlobalZ()) {
		insertIndex++;
	}
	objectList.insert(std::next(objectList.begin(), insertIndex), sceneMember);
}
