#include "Scene.h"

Scene::Scene(float cameraWidth) {
	paused = false;
	hidden = false;

	camera = new Camera(cameraWidth);
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
}

void Scene::AddObject(GameObject* object) {
	objects.push_back(object);
}
