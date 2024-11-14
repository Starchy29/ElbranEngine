#include "Vector2.h"
using namespace DirectX;

Vector2::Vector2() {
	x = 0;
	y = 0;
}

Vector2::Vector2(float x, float y) {
	this->x = x;
	this->y = y;
}

Vector2::Vector2(DirectX::XMFLOAT2 float2) {
	x = float2.x;
	y = float2.y;
}

void Vector2::Rotate(float radians) {
	XMFLOAT2 result;
	XMVECTOR rotated = XMVector3Rotate(XMVectorSet(x, y, 0, 0), XMQuaternionRotationAxis(XMVectorSet(0, 0, 1, 0), radians));
	XMStoreFloat2(&result, rotated);
	x = result.x;
	y = result.y;
}

float Vector2::Length() const {
	float result;
	XMStoreFloat(&result, XMVector2Length(XMVectorSet(x, y, 0, 0)));
	return result;
}

float Vector2::Angle() const {
	if(x == 0.0f && y == 0.0f) {
		return 0.0f;
	}

	float result;
	XMVECTOR angle = XMVector2AngleBetweenVectors(XMVectorSet(x, y, 0, 0), XMVectorSet(1, 0, 0, 0));
	XMStoreFloat(&result, angle);

	if(y < 0) {
		result = XM_2PI - result;
	}

	return result;
}

float Vector2::Distance(const Vector2& other) const {
	float result;
	XMStoreFloat(&result, XMVector2Length(XMVectorSet(x, y, 0, 0) - XMVectorSet(other.x, other.y, 0, 0)));
	return result;
}

float Vector2::SqrDist(const Vector2& other) const {
	float result;
	XMStoreFloat(&result, XMVector2LengthSq(XMVectorSet(x, y, 0, 0) - XMVectorSet(other.x, other.y, 0, 0)));
	return result;
}

float Vector2::Dot(const Vector2& other) const {
	float result;
	XMStoreFloat(&result, XMVector2Dot(XMVectorSet(x, y, 0, 0), XMVectorSet(other.x, other.y, 0, 0)));
	return result;
}

Vector2 Vector2::GetPerpendicular(bool rightSide) const {
	Vector2 result = Vector2(y, -x);

	if(!rightSide) {
		result = -result;
	}

	return result;
}

Vector2 Vector2::Normalize() const {
	float length = Length();
	if(length == 0.0f) {
		return ZERO_VECTOR;
	}

	return Vector2(x / length, y / length);
}

Vector2 Vector2::operator-() const {
	return Vector2(-x, -y);
}

Vector2 Vector2::operator+(const Vector2& other) const {
	return Vector2(x + other.x, y + other.y);
}

Vector2 Vector2::operator-(const Vector2& other) const {
	return Vector2(x - other.x, y - other.y);
}

Vector2 Vector2::operator*(const Vector2& other) const {
	return Vector2(x * other.x, y * other.y);
}

Vector2::operator DirectX::XMFLOAT2() const {
	return XMFLOAT2(x, y);
}
