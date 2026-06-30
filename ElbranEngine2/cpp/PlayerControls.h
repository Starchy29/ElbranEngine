#pragma once
#include "Math.h"

#pragma region Special Keys
// values 32+ are reserved for actual characters
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

enum class GamepadButton : uint8_t {
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

struct KeyBinds {
	static constexpr uint32_t MAX_BINDS = 3;
	char keys[MAX_BINDS];
	GamepadButton buttons[MAX_BINDS];
};

struct ButtonState {
	bool pressed;
	bool pressedLastFrame;
};

struct PlayerControls {
	static constexpr uint32_t MAX_BUTTONS = 32;
	KeyBinds bindings[MAX_BUTTONS];
	uint32_t numBinds;

	ButtonState buttons[MAX_BUTTONS];
	Vector2 leftStick;
	Vector2 rightStick;

	inline bool IsPressed(uint32_t button) const { return buttons[button].pressed; }
	inline bool JustPressed(uint32_t button) const { return buttons[button].pressed && !buttons[button].pressedLastFrame; }
	inline bool JustReleased(uint32_t button) const { return !buttons[button].pressed && buttons[button].pressedLastFrame; }
};