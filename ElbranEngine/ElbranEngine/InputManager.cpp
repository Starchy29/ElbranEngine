#include "InputManager.h"
#include "Application.h"
#include <Xinput.h>
#pragma comment(lib,"XInput.lib")
#pragma comment(lib,"Xinput9_1_0.lib")
using namespace DirectX;

#define STICK_MAX 32767.0f
#define GAMEPAD_DEAD_ZONE 0.2f
#define KEY_COUNT 256
#define KEY_DOWN(keyByte) (keyByte & 0x80) // high-order bit is 1 when the key is pressed

InputManager::InputManager(HWND windowHandle) {
	this->windowHandle = windowHandle;

	keyboardState = new BYTE[KEY_COUNT];
	previousKeyboard = new BYTE[KEY_COUNT];
	memset(keyboardState, 0, sizeof(BYTE) * KEY_COUNT);
	memset(previousKeyboard, 0, sizeof(BYTE) * KEY_COUNT);
}

InputManager::~InputManager() {
	delete[] keyboardState;
	delete[] previousKeyboard;
}

void InputManager::Update() {
	// update buttons
	memcpy(previousKeyboard, keyboardState, sizeof(unsigned char) * KEY_COUNT);
	(void)GetKeyboardState(keyboardState);

	// update mouse
	POINT mousePos;
	GetCursorPos(&mousePos);
	ScreenToClient(windowHandle, &mousePos);

	XMINT2 viewShift = APP->Graphics()->GetViewOffset();
	mousePos.x -= viewShift.x;
	mousePos.y -= viewShift.y;

	XMINT2 viewDims = APP->Graphics()->GetViewDimensions();
	mouseScreenPos.x = (float)mousePos.x / viewDims.x * 2.0f - 1.0f;
	mouseScreenPos.y = (float)mousePos.y / viewDims.y * 2.0f - 1.0f;
	mouseScreenPos.y *= -1.0f;

	// update gamepad sticks
	DWORD result;
	for(DWORD i = 0; i < XUSER_MAX_COUNT; i++) {
		XINPUT_STATE state = {};
		result = XInputGetState(i, &state);

		if(result == ERROR_SUCCESS) { // if connected
			short x = state.Gamepad.sThumbLX;
			short y = state.Gamepad.sThumbLY;
			if(x < 0) {
				x++; // range is -32768 to 32767
			}
			if(y < 0) {
				y++;
			}
			gamepadLeftSticks[i] = Vector2(x / STICK_MAX, y / STICK_MAX);

			x = state.Gamepad.sThumbRX;
			y = state.Gamepad.sThumbRY;
			if(x < 0) {
				x++; // range is -32768 to 32767
			}
			if(y < 0) {
				y++;
			}
			gamepadRightSticks[i] = Vector2(x / STICK_MAX, y / STICK_MAX);

			if(gamepadLeftSticks[i].Length() <= GAMEPAD_DEAD_ZONE) {
				gamepadLeftSticks[i] = Vector2::Zero;
			}
			if(gamepadRightSticks[i].Length() <= GAMEPAD_DEAD_ZONE) {
				gamepadRightSticks[i] = Vector2::Zero;
			}
		} else {
			gamepadLeftSticks[i] = Vector2::Zero;
			gamepadRightSticks[i] = Vector2::Zero;
		}
	}
}

bool InputManager::IsPressed(int key) {
	return KEY_DOWN(keyboardState[key]);
}

bool InputManager::JustPressed(int key) {
	return KEY_DOWN(keyboardState[key]) && !KEY_DOWN(previousKeyboard[key]);
}

bool InputManager::JustReleased(int key) {
	return !KEY_DOWN(keyboardState[key]) && KEY_DOWN(previousKeyboard[key]);
}

Vector2 InputManager::GetStick(bool left, int slot) {
	if(left) {
		return gamepadLeftSticks[slot];
	}
	return gamepadRightSticks[slot];
}

Vector2 InputManager::GetMousePosition(Camera* worldView) {
	XMFLOAT2 cameraCenter = worldView->GetPosition();
	XMFLOAT2 worldScale = worldView->GetWorldDimensions();
	
	XMVECTOR shiftFromMid = XMVectorMultiply(XMLoadFloat2(&mouseScreenPos), XMVectorScale(XMLoadFloat2(&worldScale), 0.5f));
	shiftFromMid = XMVector3Rotate(shiftFromMid, XMQuaternionRotationAxis(XMVectorSet(0, 0, 1, 0), worldView->GetRotation()));
	XMVECTOR worldPos = XMVectorAdd(XMLoadFloat2(&cameraCenter), shiftFromMid);

	XMFLOAT2 result;
	XMStoreFloat2(&result, worldPos);
	return Vector2(result);
}

float InputManager::GetMouseWheelSpin() {
	return mouseWheelDelta;
}
