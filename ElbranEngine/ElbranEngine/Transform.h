#pragma once
#include <DirectXMath.h>
#include "RectangleBox.h"
#include "Vector2.h"
#include <list>

class Transform {
	friend class GameObject;

public:
	Transform();
	Transform& operator=(const Transform& original);

	void SetPosition(Vector2 position);
	void SetX(float x);
	void SetY(float y);
	void SetScale(float x, float y);
	void SetRotation(float rotation);

	void Translate(Vector2 displacement);
	void Rotate(float radians);
	
	// scaling operations that maintain aspect ratio
	void Scale(float multiplier);
	void SetWidth(float width);
	void SetHeight(float height);
	void GrowWidth(float scaleAdditive);
	void GrowHeight(float scaleAdditive);
	
	Vector2 GetPosition() const;
	float GetGlobalZ() const;
	Vector2 GetScale() const;
	float GetRotation() const;
	DirectX::XMFLOAT4X4 GetWorldMatrix() const;
	RectangleBox GetArea() const;

private:
	Transform* parent;
	std::list<Transform*> children;

	DirectX::XMFLOAT2 position;
	float z;
	DirectX::XMFLOAT2 scale;
	float rotation;
	mutable DirectX::XMFLOAT4X4 worldMatrix;

	mutable bool needsUpdate; // only update the matrix when necessary
	inline void MarkForUpdate();
	void UpdateMatrix() const;
	void SetZ(float z);
};

