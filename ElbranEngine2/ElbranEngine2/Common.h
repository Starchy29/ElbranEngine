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