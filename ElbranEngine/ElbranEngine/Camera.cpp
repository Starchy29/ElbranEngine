#include "Camera.h"
#include "Application.h"
using namespace DirectX;
using namespace SimpleMath;

Camera::Camera(float worldWidth) {
	this->worldWidth = worldWidth;
	position = Vector2::Zero;
	rotation = 0;
	UpdateViewMatrix();
	UpdateProjectionMatrix();
}

void Camera::SetRotation(float radians) {
	viewNeedsUpdate = true;
	rotation = radians;
}

void Camera::SetPosition(Vector2 position) {
	viewNeedsUpdate = true;
	this->position = position;
}

void Camera::SetWorldWidth(float worldWidth) {
	projNeedsUpdate = true;
	this->worldWidth = worldWidth;
}

float Camera::GetRotation() const {
	return rotation;
}

Vector2 Camera::GetPosition() const {
	return position;
}

Vector2 Camera::GetWorldDimensions() const {
	return Vector2(worldWidth, worldWidth / APP->GetViewAspectRatio());
}

RectangleBox Camera::GetVisibleArea() const {
	return RectangleBox(position, GetWorldDimensions());
}

DirectX::XMFLOAT4X4 Camera::GetView() const {
	if(viewNeedsUpdate) {
		UpdateViewMatrix();
	}
	return view;
}

DirectX::XMFLOAT4X4 Camera::GetProjection() const {
	if(projNeedsUpdate) {
		UpdateProjectionMatrix();
	}
	return projection;
}

void Camera::UpdateViewMatrix() const {
	viewNeedsUpdate = false;
	XMFLOAT3 pos = XMFLOAT3(position.x, position.y, CAMERA_Z);
	XMVECTOR forward = XMVectorSet(0, 0, 1, 0);
	XMVECTOR up = XMVector3Rotate(XMVectorSet(0, 1, 0, 0), XMQuaternionRotationAxis(forward, rotation));
	XMStoreFloat4x4(&view, XMMatrixLookToLH(XMLoadFloat3(&pos), forward, up));
}

void Camera::UpdateProjectionMatrix() const {
	projNeedsUpdate = false;
	float viewAspectRatio = APP->GetViewAspectRatio();
	float worldHeight = worldWidth / viewAspectRatio;
	XMStoreFloat4x4(&projection, XMMatrixOrthographicLH(worldWidth, worldHeight, 0.0f, CAMERA_DEPTH));
}
