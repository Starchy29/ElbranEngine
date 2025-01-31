#include "Camera.h"
#include "Application.h"
using namespace DirectX;

Camera::Camera(float worldWidth) {
	this->worldWidth = worldWidth;
	position = Vector2::Zero;
	rotation = 0;
	UpdateViewMatrix();
	UpdateProjectionMatrix();
	combinedNeedsUpdate = true;
}

void Camera::SetRotation(float radians) {
	viewNeedsUpdate = true;
	combinedNeedsUpdate = true;
	rotation = radians;
}

void Camera::SetPosition(Vector2 position) {
	viewNeedsUpdate = true;
	combinedNeedsUpdate = true;
	this->position = position;
}

void Camera::SetWorldWidth(float worldWidth) {
	projNeedsUpdate = true;
	combinedNeedsUpdate = true;
	this->worldWidth = worldWidth;
}

void Camera::SetX(float x) {
	viewNeedsUpdate = true;
	combinedNeedsUpdate = true;
	position.x = x;
}

void Camera::SetY(float y) {
	viewNeedsUpdate = true;
	combinedNeedsUpdate = true;
	position.y = y;
}

void Camera::Translate(Vector2 displacement) {
	viewNeedsUpdate = true;
	combinedNeedsUpdate = true;
	position += displacement;
}

void Camera::Rotate(float radians) {
	viewNeedsUpdate = true;
	combinedNeedsUpdate = true;
	rotation += radians;
}

void Camera::Zoom(float widthChange) {
	projNeedsUpdate = true;
	combinedNeedsUpdate = true;
	worldWidth += widthChange;
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

const DirectX::XMFLOAT4X4* Camera::GetView() const {
	if(viewNeedsUpdate) {
		UpdateViewMatrix();
	}
	return &view;
}

const DirectX::XMFLOAT4X4* Camera::GetProjection() const {
	if(projNeedsUpdate) {
		UpdateProjectionMatrix();
	}
	return &projection;
}

const DirectX::XMFLOAT4X4* Camera::GetViewProjection() const {
	if(combinedNeedsUpdate) {
		combinedNeedsUpdate = false;
		XMStoreFloat4x4(&viewProjection, XMMatrixMultiply(XMLoadFloat4x4(GetView()), XMLoadFloat4x4(GetProjection())));
	}

	return &viewProjection;
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
