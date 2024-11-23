#pragma once

struct Color
{
public:
	static const Color Clear;
	static const Color Black;
	static const Color Red;
	static const Color Green;
	static const Color Blue;
	static const Color Cyan;
	static const Color Magenta;
	static const Color Yellow;
	static const Color White;

	float red;
	float green;
	float blue;
	float alpha;

	Color();
	Color(float r, float g, float b, float a = 1.0f);

	operator float* () const;
};
