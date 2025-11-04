#pragma once
#include "RenderGroup.h"
#include "Math.h"
#include "FixedList.h"
#include "Common.h"

struct UIElement {
	const Matrix* selectArea;
	virtual ~UIElement(){}

	virtual void OnFocused() {}
	virtual void OnUnfocused() {}
	virtual void OnDisabled() {}
	virtual void OnEnabled() {}
	virtual void OnSelected() {}
	virtual void OnScrolled(float wheelDelta) {}
	virtual void OnMouseDragged(Vector2 mouseDelta) {}
	virtual bool OnDirectionPressed(Direction direction) { return false; } // return true if the input is used, false otherwise
};

class UserInterface
{
public:
	bool gamepadEnabled;
	bool mouseEnabled;

	UserInterface() {}
	void Initialize(RenderGroup* scene, uint16_t maxElements);
	void Release();

	void Update(float deltaTime);

	void Join(UIElement* element);
	void SetEnabled(UIElement* element, bool enabled);

private:
	RenderGroup* scene;
	DynamicFixedList<UIElement*> elements;
	DynamicFixedList<UIElement*> disabled;
	UIElement* focus;

	UIElement* FindFurthest(Vector2 direction);
	UIElement* FindClosest(Vector2 direction);
	void ChangeFocus(UIElement* newFocus);
};

