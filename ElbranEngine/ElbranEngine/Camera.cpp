#include "Camera.h"
#include "DXGame.h"
using namespace DirectX;

Camera::Camera(float worldWidth, float aspectRatio) {
	this->worldWidth = worldWidth;
	windowAspectRatio = GameInstance->GetWindowAspectRatio();
	position = DirectX::XMFLOAT2(0, 0);
	rotation = 0;
	UpdateViewMatrix();
	UpdateProjectionMatrix();
}

void Camera::SetRotation(float radians) {
	viewNeedsUpdate = true;
	rotation = radians;
}

void Camera::SetPosition(DirectX::XMFLOAT2 position) {
	viewNeedsUpdate = true;
	this->position = position;
}

void Camera::SetWorldWidth(float worldWidth) {
	projNeedsUpdate = true;
	this->worldWidth = worldWidth;
}

float Camera::GetRotation() {
	return rotation;
}

DirectX::XMFLOAT2 Camera::GetPosition() {
	return position;
}

DirectX::XMFLOAT4X4 Camera::GetView() {
	if(viewNeedsUpdate) {
		UpdateViewMatrix();
	}
	return view;
}

DirectX::XMFLOAT4X4 Camera::GetProjection() {
	float newWindowAspectRatio = GameInstance->GetWindowAspectRatio();
	if(windowAspectRatio != newWindowAspectRatio) {
		windowAspectRatio = newWindowAspectRatio;
		UpdateProjectionMatrix();
	}
	else if(projNeedsUpdate) {
		UpdateProjectionMatrix();
	}
	return projection;
}

void Camera::UpdateViewMatrix() {
	viewNeedsUpdate = false;
	XMFLOAT3 forward = XMFLOAT3(0, 0, 1);
	XMFLOAT3 truePos = XMFLOAT3(position.x, position.y, CAMERA_Z);
	XMStoreFloat4x4(&view, XMMatrixLookToLH(XMLoadFloat3(&truePos), XMLoadFloat3(&forward), XMVectorSet(0, 1, 0, 0)));
}

void Camera::UpdateProjectionMatrix() {
	projNeedsUpdate = false;
	float viewAspectRatio = GameInstance->GetViewAspectRatio();
	float worldHeight = worldWidth / viewAspectRatio;
	DirectX::XMFLOAT2 viewDims = windowAspectRatio > viewAspectRatio ? DirectX::XMFLOAT2(worldHeight * windowAspectRatio, worldHeight) : DirectX::XMFLOAT2(worldWidth, worldWidth / windowAspectRatio);
	XMStoreFloat4x4(&projection, XMMatrixOrthographicLH(viewDims.x, viewDims.y, 0.1f, 1000.0f));
}
