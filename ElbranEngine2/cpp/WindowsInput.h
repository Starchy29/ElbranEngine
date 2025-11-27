#ifdef WINDOWS
#pragma once
#include "InputData.h"
#include "Math.h"

#define KEY_COUNT 256

struct HWND__;
struct _XINPUT_GAMEPAD;

class WindowsInput {
public:
    float mouseWheelDelta;

    WindowsInput(HWND__* windowHandle);
    ~WindowsInput();

    void CheckInputs();
    void ClearInputs();
    bool IsKeyPressed(char key, uint8_t playerIndex);
    bool IsButtonPressed(GamepadButton button, uint8_t playerIndex);
    Vector2 GetGamepadStick(bool left, uint8_t playerIndex);
    Vector2 GetMouseScreenPosition();
    float DetermineMouseSpin();
    void SetRumble(uint8_t playerIndex, float strength);

private:
    struct GamepadData {
        Vector2 leftStick;
        Vector2 rightStick;
        bool connected;
    };

    HWND__* windowHandle;

    uint8_t keyboard[KEY_COUNT];
    GamepadData gamepads[4];
    _XINPUT_GAMEPAD* gamepadStates;
};
#endif