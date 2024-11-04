#include "InputManager.h"
#include "DXGame.h"
using namespace DirectX;

#define KEY_COUNT 256
#define KEY_DOWN(keyByte) (keyByte & 0x80) // high-order bit is 1 when the key is pressed

InputManager* InputManager::instance;

void InputManager::Initialize(HWND windowHandle) {
	instance = new InputManager(windowHandle);
}

InputManager* InputManager::GetInstance() {
	return instance;
}

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
	mousePosition.x = (float)mousePos.x / GameInstance->GetWindowWidth() * 2.0f - 1.0f;
	mousePosition.y = -((float)mousePos.y / GameInstance->GetWindowHeight() * 2.0f - 1.0f);
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

XMFLOAT2 InputManager::GetMousePosition(Camera* worldView) {
	XMFLOAT4X4 viewMatrix = worldView->GetView();
	XMVECTOR transformed = XMVector3Transform(XMLoadFloat2(&mousePosition), XMLoadFloat4x4(&viewMatrix));
	XMFLOAT2 result;
	XMStoreFloat2(&result, transformed);
	return result;
}
