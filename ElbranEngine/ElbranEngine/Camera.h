#pragma once
#include "Transform.h"
#define CAMERA_Z -100.0f

class Camera {
public:
	Camera(float unitWidth, float aspectRatio);

	void SetRotation(float radians);
	void SetPosition(DirectX::XMFLOAT2 position);
	void SetUnitWidth(float width);

	float GetRotation();
	DirectX::XMFLOAT2 GetPosition();
	DirectX::XMFLOAT4X4 GetView();
	DirectX::XMFLOAT4X4 GetProjection();

private:
	float unitWidth;
	float aspectRatio;
	float rotation;
	DirectX::XMFLOAT2 position;
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 projection;

	bool projNeedsUpdate;
	bool viewNeedsUpdate;
	void UpdateViewMatrix();
	void UpdateProjectionMatrix();
};

