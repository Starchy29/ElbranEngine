#pragma once
#include "Math.h"
#include "Common.h"

class InputManager;
struct Camera;
struct Renderer;

struct UIElement {
	const Matrix* selectArea;
	virtual ~UIElement(){}

	virtual void OnFocused() {}
	virtual void OnUnfocused() {}
	virtual void OnDisabled() {}
	virtual void OnEnabled() {}
	virtual void OnSelected() {}
	virtual void OnScrolled(float wheelDelta) {}
	virtual void OnMouseDragged(Vector2 mousePosition, Vector2 mouseDelta) {}
	virtual bool OnDirectionPressed(Direction direction) { return false; } // return true if the input is used, false otherwise
};

class UserInterface {
public:
	static bool gamepadEnabled;
	static bool mouseEnabled;

	UserInterface() = default;
	void Initialize(uint16_t maxElements);
	void Release();

	void Update(const InputManager*, float deltaTime, const Camera* sceneCamera);

	void Join(UIElement* element);
	void SetEnabled(UIElement* element, bool enabled);

private:
	UIElement** elements; // all disabled elements are at the end of this array
	UIElement* focus;
	uint16_t numElements;
	uint16_t numDisabled;

	UIElement* FindFurthest(Vector2 direction);
	UIElement* FindClosest(Vector2 direction, const Camera* sceneCamera);
	void ChangeFocus(UIElement* newFocus);
};

