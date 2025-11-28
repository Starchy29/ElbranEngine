#pragma once
#include "Math.h"
#include "Common.h"
#include "InputData.h"

#define MAX_PLAYERS 1

#ifdef WINDOWS
class WindowsInput;
typedef WindowsInput PlatformInput;
#endif

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

	bool IsPressed(InputAction action, uint8_t playerIndex = 0);
	bool JustPressed(InputAction action, uint8_t playerIndex = 0);
	bool JustReleased(InputAction action, uint8_t playerIndex = 0);
	Vector2 GetStick(bool left, uint8_t playerIndex = 0);

	void Rumble(uint8_t playerIndex, float strength0to1, float duration);
	Vector2 GetMousePosition(const Camera* camera);
	Vector2 GetMouseDelta(const Camera* camera);
	float GetMouseWheelSpin();

private:
	PlatformInput* platformInput;
	InputState lastState[MAX_PLAYERS];
	InputState currentState[MAX_PLAYERS];
	float rumbleDurations[MAX_PLAYERS];
	float mouseWheelDelta;
	Vector2 mouseScreenPos;
	Vector2 lastMousePos;
};

