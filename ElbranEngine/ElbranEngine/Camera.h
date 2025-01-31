#pragma once
#include "Transform.h"
#define CAMERA_Z -100.0f
#define CAMERA_DEPTH 200
#define CAMERA_MAX_Z CAMERA_Z + CAMERA_DEPTH // objects at this Z are behind the background

class Camera {
public:
	Camera(float worldWidth);

	void SetRotation(float radians);
	void SetPosition(Vector2 position);
	void SetWorldWidth(float worldWidth);

	void SetX(float x);
	void SetY(float y);
	void Translate(Vector2 displacement);
	void Rotate(float radians);
	void Zoom(float widthChange);

	float GetRotation() const;
	Vector2 GetPosition() const;
	Vector2 GetWorldDimensions() const;
	RectangleBox GetVisibleArea() const;

	const DirectX::XMFLOAT4X4* GetView() const;
	const DirectX::XMFLOAT4X4* GetProjection() const;
	const DirectX::XMFLOAT4X4* GetViewProjection() const;

private:
	float worldWidth;
	float rotation;
	Vector2 position;
	mutable DirectX::XMFLOAT4X4 view;
	mutable DirectX::XMFLOAT4X4 projection;
	mutable DirectX::XMFLOAT4X4 viewProjection;

	mutable bool projNeedsUpdate;
	mutable bool viewNeedsUpdate;
	mutable bool combinedNeedsUpdate;
	void UpdateViewMatrix() const;
	void UpdateProjectionMatrix() const;
};

