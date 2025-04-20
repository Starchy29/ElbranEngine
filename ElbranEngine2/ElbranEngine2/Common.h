#pragma once
#include "Math.h"

enum class Direction {
	None,
	Up,
	Down,
	Left,
	Right
};

struct Int2 {
	int x;
	int y;

	Int2();
	Int2(int x, int y);

	Int2 operator+(const Int2& other);
	Int2 operator-(const Int2& other);
	Int2 operator-();
};

struct UInt2 {
	unsigned int x;
	unsigned int y;

	UInt2();
	UInt2(unsigned int x, unsigned int y);

	UInt2 operator+(const UInt2& other);
};

struct Color {
	float red;
	float green;
	float blue;
	float alpha;

	Color();
	Color(float r, float g, float b, float a = 1.f);

	static const Color Clear;
	static const Color Black;
	static const Color Red;
	static const Color Green;
	static const Color Blue;
	static const Color Cyan;
	static const Color Magenta;
	static const Color Yellow;
	static const Color White;
};