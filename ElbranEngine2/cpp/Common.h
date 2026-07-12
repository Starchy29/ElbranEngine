#pragma once
#include <stdint.h>

#define ASPECT_RATIO (16.f / 9.f)

#if defined(DEBUG) | defined(_DEBUG)
#define ASSERT(condition) if(!(condition)) *(int*)0 = 0;
#else
#define ASSERT(condition)
#endif

enum class InputAction {
	Up = 0,
	Down,
	Left,
	Right,
	Select,
	Back,

	COUNT
};

namespace StringConversion {
	char DigitToChar(uint8_t digit);
	void IntToString(int32_t number, char* outString);
	void FloatToString(float number, uint8_t decimalPlaces, char* outString);
	int32_t ParseInt(const char* string, const char** textNumberEnd = nullptr);
	float ParseFloat(const char* string, const char** textNumberEnd = nullptr);
}

struct StringBuffer {
	char* chars;
	uint32_t capacity;

	static StringBuffer FromText(char*);
	static uint32_t FindStringLength(const char*);

	uint32_t FindTextLength() const;
	bool MatchesText(StringBuffer) const;
	char* FindChar(char);
	char* FindString(StringBuffer);
	char* FindString(const char*);
	
	StringBuffer& CopyText(StringBuffer);
	StringBuffer& CopyText(const char*);
	StringBuffer& Append(StringBuffer);
	StringBuffer& Append(const char*);
	StringBuffer& Append(char);
	StringBuffer& Insert(StringBuffer, uint32_t index);
	StringBuffer& Insert(const char*, uint32_t index);
	StringBuffer& RemoveAt(uint32_t index, uint32_t length = 1);
};

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