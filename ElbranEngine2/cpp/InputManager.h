#pragma once
#include "Math.h"
#include "Common.h"

#define MAX_PLAYERS 1

#ifdef WINDOWS
class WindowsInput;
typedef WindowsInput PlatformInput;
#endif

struct Camera;

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

enum class InputAction {
	Up = 0,
	Down,
	Left,
	Right,
	Select,
	Back,
	RightClick,
	LeftCLick,

	ACTION_COUNT // number of elements, must always be last
};

struct InputState {
	bool buttons[(int)InputAction::ACTION_COUNT];
	Vector2 leftStick;
	Vector2 rightStick;
};

struct KeyBinds {
	char keys[3];
	GamepadButton buttons[3];
};

class InputManager {
public:
	KeyBinds bindings[MAX_PLAYERS][(int)InputAction::ACTION_COUNT];

	void Initialize(PlatformInput* platformInput);
	void Release();

	void Update(float deltaTime, UInt2 viewDimensions, UInt2 viewOffset);

	bool IsPressed(InputAction action, uint8_t playerIndex = 0) const;
	bool JustPressed(InputAction action, uint8_t playerIndex = 0) const;
	bool JustReleased(InputAction action, uint8_t playerIndex = 0) const;
	Vector2 GetStick(bool left, uint8_t playerIndex = 0) const;

	void Rumble(uint8_t playerIndex, float strength0to1, float duration);
	Vector2 GetMousePosition(const Camera* camera) const;
	Vector2 GetMouseDelta(const Camera* camera) const;
	float GetMouseWheelSpin() const;

private:
	PlatformInput* platformInput;
	InputState lastState[MAX_PLAYERS];
	InputState currentState[MAX_PLAYERS];
	float rumbleDurations[MAX_PLAYERS];
	float mouseWheelDelta;
	Vector2 mouseScreenPos;
	Vector2 lastMousePos;
};

