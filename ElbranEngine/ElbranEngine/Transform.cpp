#include "Transform.h"
using namespace DirectX;

Transform::Transform() {
	position = XMFLOAT2(0, 0);
	z = 0;
	scale = XMFLOAT2(1, 1);
	rotation = 0;
	parent = nullptr;

	XMStoreFloat4x4(&worldMatrix, XMMatrixIdentity());
	needsUpdate = false;
}

void Transform::SetPosition(Vector2 position) {
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

void Transform::TranslateRelative(Vector2 displacement) {
	MarkForUpdate();
	XMVECTOR shift = XMVectorSet(displacement.x, displacement.y, 0, 0);
	XMVECTOR mathPos = XMLoadFloat2(&position);
	XMStoreFloat2(&position, mathPos + shift);
}

void Transform::TranslateAbsolute(Vector2 displacement) {
	MarkForUpdate();
	XMVECTOR shift = XMVector3Rotate(XMVectorSet(displacement.x, displacement.y, 0, 0), XMQuaternionRotationAxis(XMVectorSet(0, 0, 1, 0), rotation));
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

void Transform::SetWidth(float width) {
	MarkForUpdate();
	float aspectRatio = scale.x / scale.y;
	scale.x = width;
	scale.y = width / aspectRatio;
}

void Transform::SetHeight(float height) {
	MarkForUpdate();
	float aspectRatio = scale.x / scale.y;
	scale.y = height;
	scale.x = height * aspectRatio;
}

void Transform::GrowWidth(float scaleAdditive) {
	MarkForUpdate();
	float aspectRatio = scale.x / scale.y;
	scale.x += scaleAdditive;
	scale.y = scale.x / aspectRatio;
}

void Transform::GrowHeight(float scaleAdditive) {
	MarkForUpdate();
	float aspectRatio = scale.x / scale.y;
	scale.y += scaleAdditive;
	scale.x = scale.y * aspectRatio;
}

Vector2 Transform::GetPosition() const {
	return Vector2(position);
}

float Transform::GetGlobalZ() const {
	float worldZ = z;
	Transform* nextParent = parent;
	while(nextParent != nullptr) {
		worldZ += nextParent->z;
		nextParent = nextParent->parent;
	}

	return worldZ;
}

Vector2 Transform::GetScale() const {
	return Vector2(scale);
}

float Transform::GetRotation() const {
	return rotation;
}

DirectX::XMFLOAT4X4 Transform::GetWorldMatrix() const {
	if(needsUpdate) {
		UpdateMatrix();
	}
	return worldMatrix;
}

RectangleBox Transform::GetArea() const {
	return RectangleBox(position, scale);
}

inline void Transform::MarkForUpdate() {
	needsUpdate = true;
	for(Transform* child : children) {
		child->MarkForUpdate();
	}
}

void Transform::UpdateMatrix() const {
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
