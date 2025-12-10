#include "Common.h"
#include "Math.h"

uint32_t GetStringLength(const char* string) {
	uint32_t length = 0;
	while(string[length]) length++;
	return length;
}

void AddStrings(const char* left, const char* right, char* outBuffer) {
	uint64_t outIndex = 0;
	while(left[outIndex]) {
		outBuffer[outIndex] = left[outIndex];
		outIndex++;
	}
	uint64_t rightIndex = 0;
	while(right[rightIndex]) {
		outBuffer[outIndex] = right[rightIndex];
		outIndex++;
		rightIndex++;
	}
	outBuffer[outIndex+1] = 0;
}

bool operator==(const Int2& left, const Int2& right) {
	return left.x == right.x && left.y == right.y;
}

bool operator!=(const Int2& left, const Int2& right) {
	return left.x != right.x || left.y != right.y;
}

bool operator==(const UInt2& left, const UInt2& right) {
	return left.x == right.x && left.y == right.y;
}

bool operator!=(const UInt2& left, const UInt2& right) {
	return left.x != right.x || left.y != right.y;
}

Int2::Int2(int32_t x, int32_t y) :
	x{x},
	y{y}
{ }

Int2 Int2::operator+(const Int2& other) { return Int2(x + other.x, y + other.y); }
Int2 Int2::operator-(const Int2& other) { return Int2(x - other.x, y - other.y); }
Int2 Int2::operator-() { return Int2(-x, -y); }

UInt2::UInt2(uint32_t x, uint32_t y) :
	x{x},
	y{y}
{ }

UInt2 UInt2::operator+(const UInt2& other) { return UInt2(x + other.x, y + other.y); }

Color::Color(float r, float g, float b, float a) :
	red{r},
	green{g},
	blue{b},
	alpha{a}
{ }

Color Color::FromHSV(float hue, float saturation, float brightness, float alpha) {
	// restrict inputs to valid values
	while(hue < 0.f) hue += 1.0f;
	while(hue > 1.f) hue -= 1.0f;
	brightness = Math::Clamp(brightness, 0.01f, 1.f); // don't allow brightness to become completely 0 to retain hue information
	saturation = Math::Clamp(saturation, 0.01f, 1.f); // don't allow saturation to become completely 0 to retain hue information

	Color result;

	// determine full color from hue value, where 0 and 1 are red
	result.red = Math::Clamp(2.0f - Math::Abs(6.f * (hue - (hue > 0.5f ? 1.0f : 0.f))), 0.f, 1.0f);
	result.green = Math::Clamp(2.0f - Math::Abs(6.f * (hue - 1.f/3.f)), 0.f, 1.0f);
	result.blue = Math::Clamp(2.0f - Math::Abs(6.f * (hue - 2.f/3.f)), 0.f, 1.0f);

	// lerp from grayscale to full hue by the saturation
	result.red = Tween::Lerp(1.0f, result.red, saturation);
	result.green = Tween::Lerp(1.0f, result.green, saturation);
	result.blue = Tween::Lerp(1.0f, result.blue, saturation);

	// scale by the brightness
	result.red *= brightness;
	result.green *= brightness;
	result.blue *= brightness;

	result.alpha = alpha;
	return result;
}

float Color::GetBrightness() const {
	return Math::Max(red, Math::Max(green, blue));
}

float Color::GetSaturation() const {
	float max = Math::Max(red, Math::Max(green, blue));
	if(max <= 0.f) return 0.f;
	float min = Math::Min(red, Math::Min(green, blue));
	return (max - min) / max;
}

float Color::GetHue() const {
	// fully saturate
	float min = Math::Min(red, Math::Min(green, blue));
	Color adjusted = Color(red - min, green - min, blue - min); 
	float max = Math::Max(adjusted.red, Math::Max(adjusted.green, adjusted.blue));

	// fully brighten
	if(max <= 0.f) adjusted = Color(1.f, 1.f, 1.f);
	else adjusted = Color(adjusted.red / max, adjusted.green / max, adjusted.blue / max);

	// determine hue
	if(adjusted.red <= 0.f) {
		// in range [1/3,2/3]
		return (adjusted.blue + (1.f - adjusted.green)) / 6.0f + 1.f/3.f;
	}
	else if(adjusted.green <= 0.f) {
		// in range [2/3,1]
		return (adjusted.red + (1.f - adjusted.blue)) / 6.0f + 2.f/3.f;
	}
	else if(adjusted.blue <= 0.f) {
		// in range [0,1/3]
		return (adjusted.green + (1.f - adjusted.red)) / 6.0f;
	}

	return 0.0f;
}

Color Color::SetBrightness(float brightness) const {
	return FromHSV(GetHue(), GetSaturation(), brightness);
}

Color Color::SetSaturation(float saturation) const {
	return FromHSV(GetHue(), saturation, GetBrightness());
}

Color Color::SetHue(float hue) const {
	return FromHSV(hue, GetSaturation(), GetBrightness());
}

const Color Color::Clear = Color(0, 0, 0, 0);
const Color Color::Black = Color(0, 0, 0, 1);
const Color Color::Red = Color(1, 0, 0, 1);
const Color Color::Green = Color(0, 1, 0, 1);
const Color Color::Blue = Color(0, 0, 1, 1);
const Color Color::Cyan = Color(0, 1, 1, 1);
const Color Color::Magenta = Color(1, 0, 1, 1);
const Color Color::Yellow = Color(1, 1, 0, 1);
const Color Color::White = Color(1, 1, 1, 1);