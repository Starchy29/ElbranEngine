#include "Transform.h"
using namespace DirectX;

Transform::Transform() {
	position = Vector2(0, 0);
	z = 0;
	scale = Vector2(1, 1);
	rotation = 0;
	parent = nullptr;

	XMStoreFloat4x4(&worldMatrix, XMMatrixIdentity());
	needsUpdate = false;
}

Transform& Transform::operator=(const Transform& original) {
	position = original.position;
	z = original.z;
	scale = original.scale;
	rotation = original.rotation;
	worldMatrix = original.worldMatrix;
	needsUpdate = original.needsUpdate;

	// when copied, clear parent and child relationships and recreate manually
	Transform* parent = nullptr;
	children.clear();
	return *this;
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
	scale = Vector2(x, y);
}

void Transform::SetRotation(float rotation) {
	MarkForUpdate();
	this->rotation = rotation;
}

void Transform::Translate(Vector2 displacement) {
	MarkForUpdate();
	XMVECTOR shift = XMVector3Rotate(XMVectorSet(displacement.x, displacement.y, 0, 0), XMQuaternionRotationAxis(XMVectorSet(0, 0, 1, 0), rotation));
	XMVECTOR mathPos = XMLoadFloat2(&position);
	XMStoreFloat2(&position, mathPos + shift);
}

// assumes this is not rotated. Places the center so that the input edge lines up with the input coordinate
void Transform::SetEdge(Direction edge, float coordinate) {
	MarkForUpdate();
	RectangleBox area = GetArea();

	switch(edge) {
	case Direction::Up:
		position.y += coordinate - area.top;
		break;
	case Direction::Down:
		position.y += coordinate - area.bottom;
		break;
	case Direction::Left:
		position.x += coordinate - area.left;
		break;
	case Direction::Right:
		position.x += coordinate - area.right;
		break;
	}
}

void Transform::Rotate(float radians) {
	MarkForUpdate();
	rotation += radians;
}

void Transform::Stretch(float horiontal, float vertical) {
	MarkForUpdate();
	scale.x += horiontal;
	scale.y += vertical;
}

void Transform::Scale(float multiplier) {
	MarkForUpdate();
	XMVECTOR growth = XMVectorSet(multiplier, multiplier, 1, 0);
	XMVECTOR mathScale = XMLoadFloat2(&scale);
	XMStoreFloat2(&scale, growth * mathScale);
}

void Transform::SetWidth(float width) {
	MarkForUpdate();
	float aspectRatio = scale.y != 0 ? scale.x / scale.y : 1;
	scale.x = width;
	scale.y = width / aspectRatio;
}

void Transform::SetHeight(float height) {
	MarkForUpdate();
	float aspectRatio = scale.y != 0 ? scale.x / scale.y : 1;
	scale.y = height;
	scale.x = height * aspectRatio;
}

void Transform::GrowWidth(float scaleAdditive) {
	MarkForUpdate();
	float aspectRatio = scale.y != 0 ? scale.x / scale.y : 1;
	scale.x += scaleAdditive;
	scale.y = scale.x / aspectRatio;
}

void Transform::GrowHeight(float scaleAdditive) {
	MarkForUpdate();
	float aspectRatio = scale.y != 0 ? scale.x / scale.y : 1;
	scale.y += scaleAdditive;
	scale.x = scale.y * aspectRatio;
}

Vector2 Transform::GetPosition(bool global) const {
	if(!global || parent == nullptr) {
		return position;
	}

	return Vector2::Zero.Transform(GetWorldMatrix());
}

Vector2 Transform::GetScale(bool global) const {
	if(!global || parent == nullptr) {
		return scale;
	}

	const XMFLOAT4X4* transform = GetWorldMatrix();
	Vector2 center = Vector2::Zero.Transform(transform);
	Vector2 right = Vector2(0.5f, 0.f).Transform(transform);
	Vector2 up = Vector2(0.f, 0.5f).Transform(transform);
	
	return Vector2(2.f * center.Distance(right), 2.f * center.Distance(up));
}

float Transform::GetRotation(bool global) const {
	if(!global || parent == nullptr) {
		return rotation;
	}

	const XMFLOAT4X4* transform = GetWorldMatrix();
	Vector2 center = Vector2::Zero.Transform(transform);
	Vector2 right = Vector2(0.5f, 0.f).Transform(transform);
	right = right - center;

	return right.Angle();
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

const XMFLOAT4X4* Transform::GetWorldMatrix() const {
	if(needsUpdate) {
		UpdateMatrix();
	}
	return &worldMatrix;
}

RectangleBox Transform::GetArea() const {
	// does not work when rotated
	if(parent) {
		RectangleBox localArea = RectangleBox(position, scale);
		XMMATRIX transMat = XMLoadFloat4x4(parent->GetWorldMatrix());
		XMVECTOR bottomLeft = XMVectorSet(localArea.left, localArea.bottom, 0, 0);
		XMVECTOR topRight = XMVectorSet(localArea.right, localArea.top, 0, 0);
		bottomLeft = XMVector3Transform(bottomLeft, transMat);
		topRight = XMVector3Transform(topRight, transMat);
		
		XMFLOAT2 trueBottomLeft;
		XMFLOAT2 trueTopRight;
		XMStoreFloat2(&trueBottomLeft, bottomLeft);
		XMStoreFloat2(&trueTopRight, topRight);
		return RectangleBox(trueBottomLeft.x, trueTopRight.x, trueTopRight.y, trueBottomLeft.y);
	}

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
		worldMat = XMMatrixMultiply(worldMat, XMLoadFloat4x4(parent->GetWorldMatrix()));
	}

	XMStoreFloat4x4(&worldMatrix, worldMat);
}
