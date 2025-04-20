#include "Common.h"

Int2::Int2() {
	x = 0;
	y = 0;
}

Int2::Int2(int x, int y) {
	this->x = x;
	this->y = y;
}

Int2 Int2::operator+(const Int2& other) { return Int2(x + other.x, y + other.y); }
Int2 Int2::operator-(const Int2& other) { return Int2(x - other.x, y - other.y); }
Int2 Int2::operator-() { return Int2(-x, -y); }

UInt2::UInt2() {
	x = 0u;
	y = 0u;
}

UInt2::UInt2(unsigned int x, unsigned int y) {
	this->x = x;
	this->y = y;
}

UInt2 UInt2::operator+(const UInt2& other) { return UInt2(x + other.x, y + other.y); }

Color::Color() {
	red = 0.f;
	green = 0.f;
	blue = 0.f;
	alpha = 1.0f;
}

Color::Color(float r, float g, float b, float a) {
	this->red = r;
	this->green = g;
	this->blue = b;
	this->alpha = a;
}

const Color Color::Clear = Color(0, 0, 0, 0);
const Color Color::Black = Color(0, 0, 0, 1);
const Color Color::Red = Color(1, 0, 0, 1);
const Color Color::Green = Color(0, 1, 0, 1);
const Color Color::Blue = Color(0, 0, 1, 1);
const Color Color::Cyan = Color(0, 1, 1, 1);
const Color Color::Magenta = Color(1, 0, 1, 1);
const Color Color::Yellow = Color(1, 1, 0, 1);
const Color Color::White = Color(1, 1, 1, 1);