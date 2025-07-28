#ifdef WINDOWS
#pragma once
#include "InputManager.h"
#include <Windows.h>
#include <Xinput.h>

#define KEY_COUNT 256

class WindowsInput :
    public InputManager
{
public:
    float mouseWheelDelta;

    WindowsInput(HWND windowHandle);

protected:
    void CheckInputs() override;
    void ClearInputs() override;
    bool IsKeyPressed(char key, int playerIndex) override;
    bool IsButtonPressed(GamepadButton button, int playerIndex) override;
    Vector2 GetGamepadStick(bool left, int playerIndex) override;
    Vector2 GetMouseScreenPosition() override;
    float DetermineMouseSpin() override;
    void SetRumble(int playerIndex, float strength) override;

private:
    struct GamepadData {
        bool connected;
        XINPUT_GAMEPAD state;
        Vector2 leftStick;
        Vector2 rightStick;
    };

    HWND windowHandle;

    BYTE keyboard[KEY_COUNT];
    GamepadData gamepads[4];
};
#endif