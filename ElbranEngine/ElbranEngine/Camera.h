#pragma once
#include "Transform.h"
#define CAMERA_Z -100.0f

class Camera {
public:
	Camera(float worldWidth, float aspectRatio);

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

