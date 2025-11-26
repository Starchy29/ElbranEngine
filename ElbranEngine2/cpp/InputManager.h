#pragma once
#include "AppPointer.h"
#include "Math.h"

#define MAX_PLAYERS 1

struct Camera;

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

struct InputState {
	bool buttons[(int)InputAction::ACTION_COUNT];
	Vector2 leftStick;
	Vector2 rightStick;
};

struct KeyBinds {
	char keys[3];
	GamepadButton buttons[3];
};

class InputManager
{
public:
	KeyBinds bindings[MAX_PLAYERS][(int)InputAction::ACTION_COUNT];

	InputManager();
	virtual ~InputManager() {}

	void Update(float deltaTime);

	bool IsPressed(InputAction action, uint8_t playerIndex = 0);
	bool JustPressed(InputAction action, uint8_t playerIndex = 0);
	bool JustReleased(InputAction action, uint8_t playerIndex = 0);
	Vector2 GetStick(bool left, uint8_t playerIndex = 0);

	void Rumble(uint8_t playerIndex, float strength0to1, float duration);
	Vector2 GetMousePosition(const Camera* camera);
	Vector2 GetMouseDelta(const Camera* camera);
	float GetMouseWheelSpin();

protected:
	virtual void CheckInputs() = 0;
	virtual void ClearInputs() = 0;
	virtual bool IsKeyPressed(char key, uint8_t playerIndex) = 0;
	virtual bool IsButtonPressed(GamepadButton button, uint8_t playerIndex) = 0;
	virtual Vector2 GetGamepadStick(bool left, uint8_t playerIndex) = 0;
	virtual Vector2 GetMouseScreenPosition() = 0;
	virtual float DetermineMouseSpin() = 0;
	virtual void SetRumble(uint8_t playerIndex, float strength) = 0;

private:
	InputState lastState[MAX_PLAYERS];
	InputState currentState[MAX_PLAYERS];
	float rumbleDurations[MAX_PLAYERS];
	float mouseWheelDelta;
	Vector2 mouseScreenPos;
	Vector2 lastMousePos;
};

