#pragma once
#include "Transform.h"
#define CAMERA_Z -100.0f
#define CAMERA_DEPTH 200
#define CAMERA_MAX_Z CAMERA_Z + CAMERA_DEPTH // objects at this Z are behind the background

class Camera {
public:
	Camera(float worldWidth);

	void SetRotation(float radians);
	void SetPosition(DirectX::XMFLOAT2 position);
	void SetWorldWidth(float worldWidth);

	float GetRotation();
	DirectX::XMFLOAT2 GetPosition();
	DirectX::XMFLOAT2 GetWorldDimensions();

	DirectX::XMFLOAT4X4 GetView();
	DirectX::XMFLOAT4X4 GetProjection();

private:
	float worldWidth;
	float rotation;
	DirectX::XMFLOAT2 position;
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 projection;

	bool projNeedsUpdate;
	bool viewNeedsUpdate;
	void UpdateViewMatrix();
	void UpdateProjectionMatrix();
};

