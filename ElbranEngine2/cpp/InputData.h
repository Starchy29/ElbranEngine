#pragma once
enum class GamepadButton {
	None,
	A,
	B,
	X,
	Y,
	LStickUp,
	LStickDown,
	LStickLeft,
	LStickRight,
	RStickUp,
	RStickDown,
	RStickLeft,
	RStickRight,
	DpadUp,
	DpadDown,
	DpadLeft,
	DpadRight,
	LeftTrigger,
	RightTrigger,
	LeftBumper,
	RightBumper,
	LStickPress,
	RStickPress,
	Start,
	Select
};

#pragma region Special Keys
// values 32+ are actual characters
#define KEY_NONE 0x00
#define KEY_UP_ARROW 0x01
#define KEY_DOWN_ARROW 0x02
#define KEY_LEFT_ARROW 0x03
#define KEY_RIGHT_ARROW 0x04
#define KEY_MOUSE_LEFT 0x05
#define KEY_MOUSE_RIGHT 0x06
#define KEY_MOUSE_MIDDLE 0x07
#define KEY_SHIFT 0x08
#define KEY_ENTER 0x09
#define KEY_BACKSPACE 0x0A
#define KEY_CONTROL 0x0B
#define KEY_ALT 0x0C
#define KEY_ESCAPE 0x0D
#pragma endregion