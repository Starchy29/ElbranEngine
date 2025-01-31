#pragma once
#include <DirectXMath.h>
#include "RectangleBox.h"
#include "Enums.h"
#include <list>

class Transform {
	friend class GameObject;

public:
	Transform();
	Transform& operator=(const Transform& original);

	void SetPosition(Vector2 position);
	void SetX(float x);
	void SetY(float y);
	void Translate(Vector2 displacement);
	void SetEdge(Direction edge, float coordinate);
	
	void SetRotation(float rotation);
	void Rotate(float radians);

	void SetScale(float x, float y);
	void Stretch(float horiontal, float vertical);
	
	// scaling operations that maintain aspect ratio
	void Scale(float multiplier);
	void SetWidth(float width);
	void SetHeight(float height);
	void GrowWidth(float scaleAdditive);
	void GrowHeight(float scaleAdditive);
	
	Vector2 GetPosition(bool global) const;
	Vector2 GetScale(bool global) const;
	float GetRotation(bool global) const;
	float GetGlobalZ() const;
	const DirectX::XMFLOAT4X4* GetWorldMatrix() const;
	RectangleBox GetArea() const;

private:
	Transform* parent;
	std::list<Transform*> children;

	Vector2 position;
	float z;
	Vector2 scale;
	float rotation;
	mutable DirectX::XMFLOAT4X4 worldMatrix;

	mutable bool needsUpdate; // only update the matrix when necessary
	inline void MarkForUpdate();
	void UpdateMatrix() const;
	void SetZ(float z);
};

