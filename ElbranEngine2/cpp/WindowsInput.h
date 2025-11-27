#ifdef WINDOWS
#pragma once
#include "InputManager.h"
#include <Windows.h>
#include <Xinput.h>

#define KEY_COUNT 256

class WindowsInput {
public:
    float mouseWheelDelta;

    WindowsInput(HWND windowHandle);

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
        bool connected;
        XINPUT_GAMEPAD state;
        Vector2 leftStick;
        Vector2 rightStick;
    };

    HWND windowHandle;

    uint8_t keyboard[KEY_COUNT];
    GamepadData gamepads[4];
};
#endif