#pragma once
#include <DirectXMath.h>
#include "RectangleBox.h"
#include <list>

class Transform {
	friend class GameObject;

public:
	Transform();

	void SetPosition(DirectX::XMFLOAT2 position);
	void SetX(float x);
	void SetY(float y);
	void SetScale(float x, float y);
	void SetRotation(float rotation);

	void TranslateRelative(float x, float y);
	void TranslateAbsolute(float x, float y);
	void Rotate(float radians);
	
	// scaling operations that maintain aspect ratio
	void Scale(float multiplier);
	void SetWidth(float width);
	void SetHeight(float height);
	void GrowWidth(float scaleAdditive);
	void GrowHeight(float scaleAdditive);
	
	DirectX::XMFLOAT2 GetPosition();
	float GetZ();
	DirectX::XMFLOAT2 GetScale();
	float GetRotation();
	DirectX::XMFLOAT4X4 GetWorldMatrix();
	RectangleBox GetArea();

private:
	Transform* parent;
	std::list<Transform*> children;

	DirectX::XMFLOAT2 position;
	float z;
	DirectX::XMFLOAT2 scale;
	float rotation;
	DirectX::XMFLOAT4X4 worldMatrix;

	bool needsUpdate; // only update the matrix when necessary
	inline void MarkForUpdate();
	void UpdateMatrix();
	void SetZ(float z);
};

