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

	XMINT2 viewShift = GameInstance->GetViewportShift();
	mousePos.x -= viewShift.x;
	mousePos.y -= viewShift.y;

	XMINT2 viewDims = GameInstance->GetViewportDims();
	mouseScreenPos.x = (float)mousePos.x / viewDims.x * 2.0f - 1.0f;
	mouseScreenPos.y = (float)mousePos.y / viewDims.y * 2.0f - 1.0f;
	mouseScreenPos.y *= -1.0f;

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
	XMFLOAT2 cameraCenter = worldView->GetPosition();
	XMFLOAT2 worldScale = worldView->GetWorldDimensions();
	
	XMVECTOR shiftFromMid = XMVectorMultiply(XMLoadFloat2(&mouseScreenPos), XMVectorScale(XMLoadFloat2(&worldScale), 0.5f));
	shiftFromMid = XMVector3Rotate(shiftFromMid, XMQuaternionRotationAxis(XMVectorSet(0, 0, 1, 0), worldView->GetRotation()));
	XMVECTOR worldPos = XMVectorAdd(XMLoadFloat2(&cameraCenter), shiftFromMid);

	XMFLOAT2 result;
	XMStoreFloat2(&result, worldPos);
	return result;
}
