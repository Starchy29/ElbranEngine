#include "Transform.h"
using namespace DirectX;

Transform::Transform() {
	position = XMFLOAT2(0, 0);
	z = 0;
	scale = XMFLOAT2(1, 1);
	rotation = 0;

	XMStoreFloat4x4(&worldMatrix, XMMatrixIdentity());
	needsUpdate = false;
}

void Transform::SetPosition(DirectX::XMFLOAT2 position) {
	needsUpdate = true;
	this->position = position;
}

void Transform::SetX(float x) {
	needsUpdate = true;
	position.x = x;
}

void Transform::SetY(float y) {
	needsUpdate = true;
	position.y = y;
}

void Transform::SetZ(float z) {
	needsUpdate = true;
	this->z = z;
}

void Transform::SetScale(float x, float y) {
	needsUpdate = true;
	scale = XMFLOAT2(x, y);
}

void Transform::SetRotation(float rotation) {
	needsUpdate = true;
	this->rotation = rotation;
}

void Transform::TranslateRelative(float x, float y) {
	needsUpdate = true;
	XMVECTOR shift = XMVectorSet(x, y, 0, 0);
	XMVECTOR mathPos = XMLoadFloat2(&position);
	XMStoreFloat2(&position, mathPos + shift);
}

void Transform::TranslateAbsolute(float x, float y) {
	needsUpdate = true;
	XMVECTOR shift = XMVector3Rotate(XMVectorSet(x, y, 0, 0), XMQuaternionRotationAxis(XMVectorSet(0, 0, 1, 0), rotation));
	XMVECTOR mathPos = XMLoadFloat2(&position);
	XMStoreFloat2(&position, mathPos + shift);
}

void Transform::Rotate(float radians) {
	needsUpdate = true;
	rotation += radians;
}

void Transform::Scale(float multiplier) {
	needsUpdate = true;
	XMVECTOR growth = XMVectorSet(multiplier, multiplier, 1, 0);
	XMVECTOR mathScale = XMLoadFloat2(&scale);
	XMStoreFloat2(&scale, growth * mathScale);
}

DirectX::XMFLOAT2 Transform::GetPosition() {
	return position;
}

float Transform::GetZ() {
	return z;
}

DirectX::XMFLOAT2 Transform::GetScale() {
	return scale;
}

float Transform::GetRotation() {
	return rotation;
}

DirectX::XMFLOAT4X4 Transform::GetWorldMatrix() {
	if(needsUpdate) {
		UpdateMatrix();
	}
	return worldMatrix;
}

void Transform::UpdateMatrix() {
	needsUpdate = false;

	XMMATRIX translationMat = XMMatrixTranslation(position.x, position.y, z);
	XMMATRIX scaleMat = XMMatrixScalingFromVector(XMLoadFloat2(&scale));
	XMMATRIX rotMat = XMMatrixRotationAxis(XMVectorSet(0, 0, 1, 0), rotation);

	XMMATRIX worldMat = scaleMat * rotMat * translationMat;

	XMStoreFloat4x4(&worldMatrix, worldMat);
}
