#pragma once

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

	Int2() {
		x = 0;
		y = 0;
	}

	Int2(int x, int y) {
		this->x = x;
		this->y = y;
	}

	Int2 operator+(const Int2& other) { return Int2(x + other.x, y + other.y); }
	Int2 operator-(const Int2& other) { return Int2(x - other.x, y - other.y); }
	Int2 operator*(const Int2& other) { return Int2(x * other.x, y * other.y); }
	Int2 operator-() { return Int2(-x, -y); }
};

struct UInt2 {
	unsigned int x;
	unsigned int y;

	UInt2() {
		x = 0u;
		y = 0u;
	}

	UInt2(unsigned int x, unsigned int y) {
		this->x = x;
		this->y = y;
	}

	UInt2 operator+(const UInt2& other) { return UInt2(x + other.x, y + other.y); }
	UInt2 operator*(const UInt2& other) { return UInt2(x * other.x, y * other.y); }
};

struct Color {
	float red;
	float green;
	float blue;
	float alpha;

	Color() {
		red = 0.f;
		green = 0.f;
		blue = 0.f;
		alpha = 1.0f;
	}

	Color(float r, float g, float b, float a = 1.f) {
		red = r;
		green = g;
		blue = b;
		alpha = a;
	}
};

namespace Colors {
	const Color Clear = Color(0, 0, 0, 0);
	const Color Black = Color(0, 0, 0, 1);
	const Color Red = Color(1, 0, 0, 1);
	const Color Green = Color(0, 1, 0, 1);
	const Color Blue = Color(0, 0, 1, 1);
	const Color Cyan = Color(0, 1, 1, 1);
	const Color Magenta = Color(1, 0, 1, 1);
	const Color Yellow = Color(1, 1, 0, 1);
	const Color White = Color(1, 1, 1, 1);
}