#pragma once
#include <DirectXMath.h>

class Transform {
public:
	Transform();

	void SetPosition(DirectX::XMFLOAT2 position);
	void SetX(float x);
	void SetY(float y);
	void SetZ(float z);
	void SetScale(float x, float y);
	void SetHoriScale(float x);
	void SetVertScale(float y);
	void SetRotation(float rotation);

	void TranslateRelative(float x, float y);
	void TranslateAbsolute(float x, float y);
	void Rotate(float radians);
	void Scale(float xMult, float yMult);

	DirectX::XMFLOAT2 GetPosition();
	float GetZ();
	DirectX::XMFLOAT2 GetScale();
	float GetRotation();
	DirectX::XMFLOAT4X4 GetWorldMatrix();

private:
	DirectX::XMFLOAT2 position;
	float z;
	DirectX::XMFLOAT2 scale;
	float rotation;
	DirectX::XMFLOAT4X4 worldMatrix;

	bool needsUpdate; // only update the matrix when necessary
	void UpdateMatrix();
};

