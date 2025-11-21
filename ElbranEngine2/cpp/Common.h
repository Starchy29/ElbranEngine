#pragma once
#include <stdint.h>

#if defined(DEBUG) | defined(_DEBUG)
#define ASSERT(condition) if(!(condition)) *(int*)0 = 0;
#else
#define ASSERT(condition)
#endif

enum class Direction {
	None,
	Up,
	Down,
	Left,
	Right
};

struct Int2 {
	int32_t x;
	int32_t y;

	Int2() = default;
	Int2(int32_t x, int32_t y);

	Int2 operator+(const Int2& other);
	Int2 operator-(const Int2& other);
	Int2 operator-();
};

struct UInt2 {
	uint32_t x;
	uint32_t y;

	UInt2() = default;
	UInt2(uint32_t x, uint32_t y);

	UInt2 operator+(const UInt2& other);
};

struct Color {
	float red;
	float green;
	float blue;
	float alpha;

	Color() = default;
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