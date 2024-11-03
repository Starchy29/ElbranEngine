#pragma once
struct Color
{
public:
	static Color Black;
	static Color Red;
	static Color Green;
	static Color Blue;
	static Color Cyan;
	static Color Magenta;
	static Color Yellow;
	static Color White;

	float red;
	float green;
	float blue;
	float alpha;

	Color();
	Color(float r, float g, float b, float a = 1.0f);

	operator float* () const;
};
