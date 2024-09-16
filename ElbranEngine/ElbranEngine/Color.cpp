#include "Color.h"

Color Color::Black = Color(0, 0, 0, 0);
Color Color::Red = Color(1, 0, 0, 1);
Color Color::Green = Color(0, 1, 0, 1);
Color Color::Blue = Color(0, 0, 1, 1);
Color Color::Cyan = Color(0, 1, 1, 1);
Color Color::Magenta = Color(1, 0, 1, 1);
Color Color::Yellow = Color(1, 1, 0, 1);
Color Color::White = Color(1, 1, 1, 1);

Color::Color() {
	red = 0.0f;
	green = 0.0f;
	blue = 0.0f;
	alpha = 0.0f;
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
