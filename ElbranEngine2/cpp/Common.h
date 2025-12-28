#pragma once
#include <stdint.h>

#define ASPECT_RATIO (16.f / 9.f)

#if defined(DEBUG) | defined(_DEBUG)
#define ASSERT(condition) if(!(condition)) *(int*)0 = 0;
#else
#define ASSERT(condition)
#endif

namespace String {
	uint32_t GetStringLength(const char* string);
	void AddStrings(const char* left, const char* right, char* outBuffer);
	char* FindChar(char* string, char seeked);
	const char* FindChar(const char* string, char seeked);
	char DigitToChar(uint8_t digit);
	void IntToString(int32_t number, char* outString);
	void FloatToString(float number, uint8_t decimalPlaces, char* outString);
	int32_t ParseInt(const char* string, const char** textNumberEnd = nullptr);
	float ParseFloat(const char* string, const char** textNumberEnd = nullptr);
}

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

	Int2 operator+(const Int2& other) const;
	Int2 operator-(const Int2& other) const;
	Int2 operator-() const;
};
bool operator==(const Int2& left, const Int2& right);
bool operator!=(const Int2& left, const Int2& right);

struct UInt2 {
	uint32_t x;
	uint32_t y;

	UInt2() = default;
	UInt2(uint32_t x, uint32_t y);

	UInt2 operator+(const UInt2& other) const;
};
bool operator==(const UInt2& left, const UInt2& right);
bool operator!=(const UInt2& left, const UInt2& right);

struct Color {
	float red;
	float green;
	float blue;
	float alpha;

	Color() = default;
	Color(float r, float g, float b, float a = 1.f);

	static Color FromHSV(float hue, float saturation, float brightness, float alpha = 1.0f);
	float GetBrightness() const;
	float GetSaturation() const;
	float GetHue() const;
	Color SetBrightness(float brightness) const;
	Color SetSaturation(float saturation) const;
	Color SetHue(float hue) const;

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