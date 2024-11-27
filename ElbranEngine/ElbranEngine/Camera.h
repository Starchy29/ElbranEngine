#pragma once
#include "Transform.h"
#define CAMERA_Z -100.0f
#define CAMERA_DEPTH 200
#define CAMERA_MAX_Z CAMERA_Z + CAMERA_DEPTH // objects at this Z are behind the background

class Camera {
public:
	Camera(float worldWidth);

	void SetRotation(float radians);
	void SetPosition(DirectX::SimpleMath::Vector2 position);
	void SetWorldWidth(float worldWidth);

	float GetRotation() const;
	DirectX::SimpleMath::Vector2 GetPosition() const;
	DirectX::SimpleMath::Vector2 GetWorldDimensions() const;
	RectangleBox GetVisibleArea() const;

	DirectX::XMFLOAT4X4 GetView() const;
	DirectX::XMFLOAT4X4 GetProjection() const;

private:
	float worldWidth;
	float rotation;
	DirectX::SimpleMath::Vector2 position;
	mutable DirectX::XMFLOAT4X4 view;
	mutable DirectX::XMFLOAT4X4 projection;

	mutable bool projNeedsUpdate;
	mutable bool viewNeedsUpdate;
	void UpdateViewMatrix() const;
	void UpdateProjectionMatrix() const;
};

