#pragma once
#include "Circle.h"
#include "Vector2.h"

struct RectangleBox
{
	float left;
	float right;
	float top;
	float bottom;

	RectangleBox(Vector2 center, Vector2 size);
	RectangleBox(float left, float right, float top, float bottom);

	Vector2 GetCenter() const;
	Vector2 GetSize() const;

	void SetCenterAndSize(Vector2 center, Vector2 size);
	void SetCenter(Vector2 center);
	void SetWidth(float width);
	void SetHeight(float height);
	void SetSize(Vector2 size);

	RectangleBox Expand(float shiftPerSide);

	bool Contains(const Vector2 &point) const;
	bool Contains(const RectangleBox &other) const;
	bool Intersects(const RectangleBox &other) const;
	bool Intersects(const Circle &circle) const;
};

