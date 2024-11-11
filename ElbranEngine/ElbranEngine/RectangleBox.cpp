#include "RectangleBox.h"

RectangleBox::RectangleBox(DirectX::XMFLOAT2 center, DirectX::XMFLOAT2 size) {
	SetCenterAndSize(center, size);
}

RectangleBox::RectangleBox(float left, float right, float top, float bottom) {
	this->left = left;
	this->right = right;
	this->top = top;
	this->bottom = bottom;
}

DirectX::XMFLOAT2 RectangleBox::GetCenter() {
	return DirectX::XMFLOAT2((right + left) / 2.0f, (top + bottom) / 2.0f);
}

DirectX::XMFLOAT2 RectangleBox::GetSize() {
	return DirectX::XMFLOAT2(right - left, top - bottom);
}

void RectangleBox::SetCenter(DirectX::XMFLOAT2 center) {
	SetCenterAndSize(center, GetSize());
}

void RectangleBox::SetWidth(float width) {
	DirectX::XMFLOAT2 size = GetSize();
	size.x = width;
	SetCenterAndSize(GetCenter(), size);
}

void RectangleBox::SetHeight(float height) {
	DirectX::XMFLOAT2 size = GetSize();
	size.y = height;
	SetCenterAndSize(GetCenter(), size);
}

void RectangleBox::SetSize(DirectX::XMFLOAT2 size) {
	SetCenterAndSize(GetCenter(), size);
}

bool RectangleBox::Contains(const DirectX::XMFLOAT2 & point) {
	return point.x >= left && point.x <= right && point.y <= top && point.y >= bottom;
}

bool RectangleBox::Contains(const RectangleBox & other) {
	return other.left >= left && other.right <= right && other.top <= top && other.bottom >= bottom;
}

bool RectangleBox::Intersects(const RectangleBox & other) {
	return false;
}

void RectangleBox::SetCenterAndSize(DirectX::XMFLOAT2 center, DirectX::XMFLOAT2 size) {
	left = center.x - size.x / 2.0f;
	right = center.x + size.x / 2.0f;
	top = center.y + size.y / 2.0f;
	bottom = center.y - size.y / 2.0f;
}
