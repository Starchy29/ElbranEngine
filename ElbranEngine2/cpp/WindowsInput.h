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
    bool IsKeyPressed(char key, uint8_t playerIndex) override;
    bool IsButtonPressed(GamepadButton button, uint8_t playerIndex) override;
    Vector2 GetGamepadStick(bool left, uint8_t playerIndex) override;
    Vector2 GetMouseScreenPosition() override;
    float DetermineMouseSpin() override;
    void SetRumble(uint8_t playerIndex, float strength) override;

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