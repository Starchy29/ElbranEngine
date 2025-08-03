#pragma once
#include "IBehavior.h"
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
	: public IBehavior
{
public:
	bool gamepadEnabled;
	bool mouseEnabled;

	UserInterface();
	UserInterface(unsigned int maxElements);
	~UserInterface();

	void Update(float deltaTime) override;

	void Join(UIElement* element);
	void Disable(UIElement* element);
	void Enable(UIElement* element);

private:
	DynamicFixedList<UIElement*> elements;
	DynamicFixedList<UIElement*> disabled;
	UIElement* focus;

	UIElement* FindFurthest(Vector2 direction);
	UIElement* FindClosest(Vector2 direction);
	void ChangeFocus(UIElement* newFocus);
};

