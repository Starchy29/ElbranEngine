#include "Scene.h"
#include "AssetManager.h"

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

Scene::Scene(float cameraWidth, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> backgroundImage) {
	paused = false;
	hidden = false;
	camera = new Camera(cameraWidth);

	hasBackground = true;
	backColor = Color::White;
	backImage = backgroundImage;
}

Scene::~Scene() {
	delete camera;

	for(GameObject* object : objects) {
		delete object;
	}
}

Camera* Scene::GetCamera() {
	return camera;
}

void Scene::Update(float deltaTime) {
	// update all game objects
	for(GameObject* object : objects) {
		if(object->active) {
			object->Update(deltaTime);
		}
	}

	// remove objects that need to be deleted
	for(int i = objects.size() - 1; i >= 0; i--) {
		if(objects[i]->toBeDeleted) {
			delete objects[i];
			objects.erase(std::next(objects.begin(), i));
		}
	}
}

void Scene::Draw() {
	for(GameObject* object : objects) {
		if(object->visible) {
			object->Draw(camera);
		}
	}

	// draw the background
	if(!hasBackground) {
		return;
	}

	AssetManager* assets = AssetManager::GetInstance();

	if(backImage != nullptr) {
		std::shared_ptr<PixelShader> imageShader = assets->imagePS;
		imageShader->SetConstantVariable("color", &backColor);
		imageShader->SetTexture(backImage);
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

void Scene::AddObject(GameObject* object) {
	objects.push_back(object);
}
