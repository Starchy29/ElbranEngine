#pragma once
#include <SimpleMath.h>
#include "Circle.h"

struct RectangleBox
{
	float left;
	float right;
	float top;
	float bottom;

	RectangleBox(DirectX::SimpleMath::Vector2 center, DirectX::SimpleMath::Vector2 size);
	RectangleBox(float left, float right, float top, float bottom);

	DirectX::SimpleMath::Vector2 GetCenter() const;
	DirectX::SimpleMath::Vector2 GetSize() const;

	void SetCenterAndSize(DirectX::SimpleMath::Vector2 center, DirectX::SimpleMath::Vector2 size);
	void SetCenter(DirectX::SimpleMath::Vector2 center);
	void SetWidth(float width);
	void SetHeight(float height);
	void SetSize(DirectX::SimpleMath::Vector2 size);
	void Expand(float shiftPerSide);

	bool Contains(const DirectX::SimpleMath::Vector2 &point) const;
	bool Contains(const RectangleBox &other) const;
	bool Intersects(const RectangleBox &other) const;
	bool Intersects(const Circle& circle) const;
};

