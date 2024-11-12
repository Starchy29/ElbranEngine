#include "RectangleBox.h"

RectangleBox::RectangleBox(Vector2 center, Vector2 size) {
	SetCenterAndSize(center, size);
}

RectangleBox::RectangleBox(float left, float right, float top, float bottom) {
	this->left = left;
	this->right = right;
	this->top = top;
	this->bottom = bottom;
}

Vector2 RectangleBox::GetCenter() const {
	return Vector2((right + left) / 2.0f, (top + bottom) / 2.0f);
}

Vector2 RectangleBox::GetSize() const {
	return Vector2(right - left, top - bottom);
}

void RectangleBox::SetCenter(Vector2 center) {
	SetCenterAndSize(center, GetSize());
}

void RectangleBox::SetWidth(float width) {
	Vector2 size = GetSize();
	size.x = width;
	SetCenterAndSize(GetCenter(), size);
}

void RectangleBox::SetHeight(float height) {
	Vector2 size = GetSize();
	size.y = height;
	SetCenterAndSize(GetCenter(), size);
}

void RectangleBox::SetSize(Vector2 size) {
	SetCenterAndSize(GetCenter(), size);
}

void RectangleBox::Expand(float shiftPerSide) {
	left -= shiftPerSide;
	right += shiftPerSide;
	top += shiftPerSide;
	bottom -= shiftPerSide;
}

bool RectangleBox::Contains(const Vector2 &point) const {
	return point.x >= left && point.x <= right && point.y <= top && point.y >= bottom;
}

bool RectangleBox::Contains(const RectangleBox &other) const {
	return other.left >= left && other.right <= right && other.top <= top && other.bottom >= bottom;
}

bool RectangleBox::Intersects(const RectangleBox &other) const {
	return !(right < other.left || left > other.right || top < other.bottom || bottom > other.top);
}

bool RectangleBox::Intersects(const Circle& circle) const {
	if(circle.center.x >= left && circle.center.x <= right) {
		return fabs(circle.center.y - GetCenter().y) < circle.radius + (top - bottom) / 2.0f;
	}

	if(circle.center.y >= bottom && circle.center.y <= top) {
		return fabs(circle.center.x - GetCenter().x) < circle.radius + (right - left) / 2.0f;
	}

	Vector2 corner = Vector2(circle.center.x < left ? left : right, circle.center.y < bottom ? bottom : top);
	return circle.center.SqrDist(corner) <= circle.radius * circle.radius;
}

void RectangleBox::SetCenterAndSize(Vector2 center, Vector2 size) {
	left = center.x - size.x / 2.0f;
	right = center.x + size.x / 2.0f;
	top = center.y + size.y / 2.0f;
	bottom = center.y - size.y / 2.0f;
}
