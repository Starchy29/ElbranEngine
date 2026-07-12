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
	virtual void OnDeselected() {}
	virtual void OnScrolled(float wheelDelta) {}
	virtual void OnMouseDragged(Vector2 mousePosition, Vector2 mouseDelta) {}
	virtual void OnDirectionPressed(Direction direction, bool* outInputUsed) {}
};

class UserInterface {
public:
	bool gamepadEnabled;
	bool mouseEnabled;

	UserInterface() = default;
	void Initialize(uint16_t maxElements);
	void Release();

	void Update(const InputManager*, float deltaTime, const Camera* sceneCamera);

	void Join(UIElement* element);

private:
	UIElement** elements;
	UIElement* focus;
	uint16_t numElements;

	UIElement* FindFurthest(Vector2 direction);
	UIElement* FindClosest(Vector2 direction, const Camera* sceneCamera);
	void ChangeFocus(UIElement* newFocus);
};

