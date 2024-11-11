#pragma once
#include <DirectXMath.h>

struct RectangleBox
{
	float left;
	float right;
	float top;
	float bottom;

	RectangleBox(DirectX::XMFLOAT2 center, DirectX::XMFLOAT2 size);
	RectangleBox(float left, float right, float top, float bottom);

	DirectX::XMFLOAT2 GetCenter();
	DirectX::XMFLOAT2 GetSize();

	void SetCenterAndSize(DirectX::XMFLOAT2 center, DirectX::XMFLOAT2 size);
	void SetCenter(DirectX::XMFLOAT2 center);
	void SetWidth(float width);
	void SetHeight(float height);
	void SetSize(DirectX::XMFLOAT2 size);

	bool Contains(const DirectX::XMFLOAT2 & point);
	bool Contains(const RectangleBox & other);
	bool Intersects(const RectangleBox & other);
};

