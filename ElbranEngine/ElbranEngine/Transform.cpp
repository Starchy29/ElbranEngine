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
	MarkForUpdate();
	this->position = position;
}

void Transform::SetX(float x) {
	MarkForUpdate();
	position.x = x;
}

void Transform::SetY(float y) {
	MarkForUpdate();
	position.y = y;
}

void Transform::SetZ(float z) {
	MarkForUpdate();
	this->z = z;
}

void Transform::SetScale(float x, float y) {
	MarkForUpdate();
	scale = XMFLOAT2(x, y);
}

void Transform::SetRotation(float rotation) {
	MarkForUpdate();
	this->rotation = rotation;
}

void Transform::TranslateRelative(float x, float y) {
	MarkForUpdate();
	XMVECTOR shift = XMVectorSet(x, y, 0, 0);
	XMVECTOR mathPos = XMLoadFloat2(&position);
	XMStoreFloat2(&position, mathPos + shift);
}

void Transform::TranslateAbsolute(float x, float y) {
	MarkForUpdate();
	XMVECTOR shift = XMVector3Rotate(XMVectorSet(x, y, 0, 0), XMQuaternionRotationAxis(XMVectorSet(0, 0, 1, 0), rotation));
	XMVECTOR mathPos = XMLoadFloat2(&position);
	XMStoreFloat2(&position, mathPos + shift);
}

void Transform::Rotate(float radians) {
	MarkForUpdate();
	rotation += radians;
}

void Transform::Scale(float multiplier) {
	MarkForUpdate();
	XMVECTOR growth = XMVectorSet(multiplier, multiplier, 1, 0);
	XMVECTOR mathScale = XMLoadFloat2(&scale);
	XMStoreFloat2(&scale, growth * mathScale);
}

void Transform::Grow(float scaleAdditive) {
	MarkForUpdate();
	scale.x += scaleAdditive;
	scale.y += scaleAdditive;
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

RectangleBox Transform::GetArea() {
	return RectangleBox(position, scale);
}

inline void Transform::MarkForUpdate() {
	needsUpdate = true;
	for(Transform* child : children) {
		child->MarkForUpdate();
	}
}

void Transform::UpdateMatrix() {
	needsUpdate = false;

	XMMATRIX translationMat = XMMatrixTranslation(position.x, position.y, z);
	XMMATRIX scaleMat = XMMatrixScalingFromVector(XMLoadFloat2(&scale));
	XMMATRIX rotMat = XMMatrixRotationAxis(XMVectorSet(0, 0, 1, 0), rotation);

	XMMATRIX worldMat = scaleMat * rotMat * translationMat;

	if(parent != nullptr) {
		XMFLOAT4X4 parentMat = parent->GetWorldMatrix();
		worldMat = XMMatrixMultiply(worldMat, XMLoadFloat4x4(&parentMat));
	}

	XMStoreFloat4x4(&worldMatrix, worldMat);
}
