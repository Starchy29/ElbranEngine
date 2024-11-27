#include "Color.h"

const Color Color::Clear = Color(0, 0, 0, 0);
const Color Color::Black = Color(0, 0, 0, 1);
const Color Color::Red = Color(1, 0, 0, 1);
const Color Color::Green = Color(0, 1, 0, 1);
const Color Color::Blue = Color(0, 0, 1, 1);
const Color Color::Cyan = Color(0, 1, 1, 1);
const Color Color::Magenta = Color(1, 0, 1, 1);
const Color Color::Yellow = Color(1, 1, 0, 1);
const Color Color::White = Color(1, 1, 1, 1);

Color::Color() {
	red = 0;
	green = 0;
	blue = 0;
	alpha = 0;
}

Color::Color(float r, float g, float b, float a) {
	red = r;
	green = g;
	blue = b;
	alpha = a;
}

Color::operator float* () const {
	return (float*)this;
}

Color::operator DirectX::XMFLOAT4() const {
	return DirectX::XMFLOAT4(red, green, blue, alpha);
}
