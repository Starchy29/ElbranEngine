#pragma once
#include "Transform.h"

#define CAMERA_Z 0
#define CAMERA_DEPTH 100

struct Camera {
	Vector2 position;
	float viewWidth;
	float rotation;
};

class Scene
{
public:
	Camera camera;

	Scene();
	virtual ~Scene();

	virtual void Update(float deltaTime);
	virtual void Draw();

private:
	Transform transforms[10];
};

