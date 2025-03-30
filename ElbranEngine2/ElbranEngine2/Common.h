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