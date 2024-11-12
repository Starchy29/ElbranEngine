#pragma once
#define CLEAR_COLOR Color(0, 0, 0, 0)
#define BLACK Color(0, 0, 0, 1)
#define RED Color(1, 0, 0, 1)
#define GREEN Color(0, 1, 0, 1)
#define BLUE Color(0, 0, 1, 1)
#define CYAN Color(0, 1, 1, 1)
#define MAGENTA Color(1, 0, 1, 1)
#define YELLOW Color(1, 1, 0, 1)
#define WHITE Color(1, 1, 1, 1)

struct Color
{
public:
	float red;
	float green;
	float blue;
	float alpha;

	Color() {
		red = 0.0f;
		green = 0.0f;
		blue = 0.0f;
		alpha = 0.0f;
	}

	Color(float r, float g, float b, float a = 1.0f) {
		red = r;
		green = g;
		blue = b;
		alpha = a;
	}

	operator float* () const {
		return (float*)this;
	}
};
