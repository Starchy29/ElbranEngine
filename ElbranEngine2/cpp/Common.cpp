#include "Common.h"
#include "Math.h"

char StringConversion::DigitToChar(uint8_t digit) {
	switch(digit) {
	case 0: return '0';
	case 1: return '1';
	case 2: return '2';
	case 3: return '3';
	case 4: return '4';
	case 5: return '5';
	case 6: return '6';
	case 7: return '7';
	case 8: return '8';
	case 9: return '9';
	}
	return 0;
}

void StringConversion::IntToString(int32_t number, char* outString) {
	if(number < 0) {
		*outString = '-';
		outString++;
		number *= -1;
	}

	int32_t digitTracker = number;
	int32_t numDigits = 0;
	while(digitTracker > 0) {
		digitTracker /= 10;
		numDigits++;
	}

	if(numDigits == 0) {
		*outString = '0';
		outString[1] = 0;
		return;
	}

	digitTracker = number;
	for(int32_t i = numDigits - 1; i >= 0; i--) {
		outString[i] = DigitToChar(digitTracker % 10);
		digitTracker /= 10;
	}
	outString[numDigits] = 0;
	return;
}

void StringConversion::FloatToString(float number, uint8_t decimalPlaces, char* outString) {
	IntToString((int32_t)number, outString);
	if(decimalPlaces == 0) return;
	while(*outString) outString++; // find the new end of the string

	*outString = '.';
	outString++;
	for(uint8_t i = 0; i < decimalPlaces; i++) {
		number *= 10;
		*outString = DigitToChar(((int32_t)number) % 10);
		outString++;
	}
	*outString = 0;
}

int32_t StringConversion::ParseInt(const char* string, const char** textNumberEnd) {
	bool negative = string[0] == '-';
	if(negative) string++;
	int32_t total = 0;
	bool isDigit = true;
	while(isDigit) {
		switch(*string) {
		case '0':
			total *= 10;
			break;
		case '1':
			total = 10 * total + 1;
			break;
		case '2':
			total = 10 * total + 2;
			break;
		case '3':
			total = 10 * total + 3;
			break;
		case '4':
			total = 10 * total + 4;
			break;
		case '5':
			total = 10 * total + 5;
			break;
		case '6':
			total = 10 * total + 6;
			break;
		case '7':
			total = 10 * total + 7;;
			break;
		case '8':
			total = 10 * total + 8;
			break;
		case '9':
			total = 10 * total + 9;
			break;
		default:
			isDigit = false;
			break;
		}
		string++;
	}
	if(textNumberEnd) *textNumberEnd = string - 1;
	return (negative ? -1 : 1) * total;
}

float StringConversion::ParseFloat(const char* string, const char** textNumberEnd) {
	const char* readLoc;
	int32_t leftSide = ParseInt(string, &readLoc);
	if(*readLoc != '.') {
		if(textNumberEnd) *textNumberEnd = readLoc;
		return (float)leftSide;
	}
	const char* decimalLoc = readLoc;
	readLoc++; // skip decimal point
	int32_t rightSide = ParseInt(readLoc, &readLoc);
	if(rightSide < 0) {
		if(textNumberEnd) *textNumberEnd = decimalLoc;
		return (float)leftSide;
	}

	uint32_t decimalShift = Math::Pow(10,  readLoc - decimalLoc - 1);
	if(textNumberEnd) *textNumberEnd = readLoc;
	int32_t intResult = leftSide * decimalShift + Math::Sign(leftSide) * rightSide;
	return (float)intResult / decimalShift;
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

Int2 Int2::operator+(const Int2& other) const { return Int2(x + other.x, y + other.y); }
Int2 Int2::operator-(const Int2& other) const { return Int2(x - other.x, y - other.y); }
Int2 Int2::operator-() const { return Int2(-x, -y); }

UInt2::UInt2(uint32_t x, uint32_t y) :
	x{x},
	y{y}
{ }

UInt2 UInt2::operator+(const UInt2& other) const { return UInt2(x + other.x, y + other.y); }

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

StringBuffer StringBuffer::FromText(char* text) {
	// assumes text is null terminated
	StringBuffer result = { text, UINT32_MAX };
	result.capacity = StringBuffer::FindStringLength(text);
	return result;
}

uint32_t StringBuffer::FindStringLength(const char* nullTerminated) {
	uint32_t index = 0;
	while(nullTerminated[index]) index++;
	return index;
}

uint32_t StringBuffer::FindTextLength() const {
	uint32_t length = 0;
	for(uint32_t i = 0; i < capacity; i++) if(chars[i] == 0) return i;
	return 0; // does not count as text unless it's null-terminated
}

bool StringBuffer::MatchesText(StringBuffer other) const {
	// the buffers need not have the same length, but they must null-terminate at the same index
	uint32_t index = 0;
	uint32_t maxIndex = Math::Min((int32_t)capacity, (int32_t)other.capacity) - 1;
	while(index <= maxIndex && chars[index] == other.chars[index]) {
		if(chars[index] == 0) return true;
		index++;
	}
	return false;
}

char* StringBuffer::FindChar(char seeked) {
	uint32_t index = 0;
	while(index < capacity && chars[index] != 0) {
		if(chars[index] == seeked) return chars + index;
		index++;
	}
	return nullptr;
}

char* StringBuffer::FindString(StringBuffer seeked) {
	uint32_t seekedLength = seeked.FindTextLength();
	if(seekedLength == 0) return nullptr;
	uint32_t index = 0;
	uint32_t matchLength = 0;
	while(index <= capacity - seekedLength) {
		if(chars[index] == seeked.chars[index]) { 
			matchLength++;
			if(matchLength == seekedLength) return chars + index;
		}
		else matchLength = 0;
	}
	return nullptr;
}

char* StringBuffer::FindString(const char* seeked) {
	uint32_t seekedLength = StringBuffer::FindStringLength(seeked);
	uint32_t index = 0;
	uint32_t matchLength = 0;
	while(index <= capacity - seekedLength) {
		if(chars[index] == seeked[index]) { 
			matchLength++;
			if(matchLength == seekedLength) return chars + index;
		}
		else matchLength = 0;
	}
	return nullptr;
}

StringBuffer& StringBuffer::CopyText(StringBuffer string) {
	uint32_t maxIndex = Math::Min((int32_t)capacity, (int32_t)string.capacity) - 1;
	for(uint32_t i = 0; i <= maxIndex; i++) {
		chars[i] = string.chars[i];
		if(string.chars[i] == 0) return *this;
	}
	if(capacity > string.capacity) chars[string.capacity + 1] = 0; // null-terminate in cases when the input string was not null-terminated
	else chars[capacity - 1] = 0; // null-terminate when the target buffer is too small to fit all of the text
	return *this;
}

StringBuffer& StringBuffer::CopyText(const char* string) {
	uint32_t stringLength = StringBuffer::FindStringLength(string);
	uint32_t maxIndex = Math::Min((int32_t)capacity, (int32_t)stringLength) - 1;
	for(uint32_t i = 0; i <= maxIndex; i++) {
		chars[i] = string[i];
		if(string[i] == 0) return *this;
	}
	if(capacity > stringLength) chars[stringLength + 1] = 0; // null-terminate in cases when the input string was not null-terminated
	else chars[capacity - 1] = 0; // null-terminate when the target buffer is too small to fit all of the text
	return *this;
}

StringBuffer& StringBuffer::Append(StringBuffer string) {
	uint32_t textLength = FindTextLength();
	StringBuffer bufferEnd = { chars + textLength, capacity - textLength };
	bufferEnd.CopyText(string);
	return *this;
}

StringBuffer& StringBuffer::Append(const char* string) {
	uint32_t textLength = FindTextLength();
	StringBuffer bufferEnd = { chars + textLength, capacity - textLength };
	bufferEnd.CopyText(string);
	return *this;
}

StringBuffer& StringBuffer::Append(char character) {
	uint32_t textLength = FindTextLength();
	if(textLength >= capacity - 1) return *this;
	if(textLength > 0) {
		chars[textLength] = character;
		chars[textLength + 1] = 0;
	}
	else if(capacity > 1) {
		chars[0] = character;
		chars[1] = 0;
	}
	return *this;
}

StringBuffer& StringBuffer::Insert(StringBuffer inserted, uint32_t index) {
	uint32_t insertLength = inserted.FindTextLength();
	uint32_t i = index + insertLength;
	if(i >= capacity) {
		chars[capacity - 1] = 0; // null terminate if grew past capacity
		i = capacity - 2;
	}

	// shift existing chars to make room for inserted chars
	for(; i >= index + insertLength; i--) chars[i] = chars[i - insertLength];

	// insert new chars
	for(; i >= index; i--) chars[i] = inserted.chars[i - index];
	
	return *this;
}

StringBuffer& StringBuffer::Insert(const char* inserted, uint32_t index) {
	uint32_t insertLength = StringBuffer::FindStringLength(inserted);
	uint32_t i = 2 * insertLength + index;
	if(i >= capacity) {
		chars[capacity - 1] = 0; // null terminate if grew past capacity
		i = capacity - 2;
	}

	// shift existing chars to make room for inserted chars
	for(; i >= index + insertLength; i--) chars[i] = chars[i - insertLength];

	// insert new chars
	for(; i >= index; i--) chars[i] = inserted[i - index];

	return *this;
}

StringBuffer& StringBuffer::RemoveAt(uint32_t index, uint32_t length) {
	// do not modify past the capacity or end of the text
	uint32_t maxIndex = Math::Min((int32_t)FindTextLength() + 1, capacity);
	if(index >= maxIndex) return *this;
	if(index + length >= maxIndex) length = maxIndex - index - 1;

	for(uint32_t i = index; i < maxIndex; i++) chars[i] = chars[i+1];

	return *this;
}