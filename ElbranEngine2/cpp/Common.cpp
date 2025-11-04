#include "Common.h"

Int2::Int2(int32_t x, int32_t y) :
	x{x},
	y{y}
{ }

Int2 Int2::operator+(const Int2& other) { return Int2(x + other.x, y + other.y); }
Int2 Int2::operator-(const Int2& other) { return Int2(x - other.x, y - other.y); }
Int2 Int2::operator-() { return Int2(-x, -y); }

UInt2::UInt2(uint32_t x, uint32_t y) :
	x{x},
	y{y}
{ }

UInt2 UInt2::operator+(const UInt2& other) { return UInt2(x + other.x, y + other.y); }

Color::Color(float r, float g, float b, float a) :
	red{r},
	green{g},
	blue{b},
	alpha{a}
{ }

const Color Color::Clear = Color(0, 0, 0, 0);
const Color Color::Black = Color(0, 0, 0, 1);
const Color Color::Red = Color(1, 0, 0, 1);
const Color Color::Green = Color(0, 1, 0, 1);
const Color Color::Blue = Color(0, 0, 1, 1);
const Color Color::Cyan = Color(0, 1, 1, 1);
const Color Color::Magenta = Color(1, 0, 1, 1);
const Color Color::Yellow = Color(1, 1, 0, 1);
const Color Color::White = Color(1, 1, 1, 1);