#pragma once
#include "Transform.h"
#include "Buffers.h"

#include "SpriteRenderer.h"

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

	Scene(float cameraWidth);
	virtual ~Scene();

	virtual void Update(float deltaTime);
	virtual void Draw();

private:
	ConstantBuffer projectionBuffer;
	Transform transforms[10];
	Matrix worldMatrices[10];

	float t = 0.f;
	SpriteRenderer* testRenderer;
};

