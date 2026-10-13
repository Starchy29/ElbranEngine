#pragma once
#include "Math.h"
#include "Common.h"
#include "AppComponents.h"

class InputManager;
struct Camera;
struct Renderer;

struct InputEventUI {
	enum class Type {
		Unfocused,
		MouseHovered,
		MouseClicked,
		Scrolled,
		Dragged,
		ControllerFocused,
		ControllerSelected,
		DirectionPressed
	} type;

	union {
		float scrollDelta;
		struct {
			Vector2 mousePos;
			Vector2 mouseDelta;
		};
		Direction direction;
	};
};

struct UIElement {
	virtual ~UIElement() {}
	virtual AlignedRect GetScreenArea() const = 0;
	virtual float GetScreenOrder() const { return 0.f; }
	virtual bool ProcessInput(InputEventUI inputEvent, AppComponents) { return false; } // returns true if responded to the user input
};

class UserInterface {
public:
	UIElement** elements;
	UIElement* focus;
	uint16_t numElements;

	bool gamepadEnabled;
	bool mouseEnabled;

	UserInterface() = default;
	void Initialize(uint16_t maxElements);
	void Release();

	void Update(AppComponents, float deltaTime, const Camera* sceneCamera);

	void Join(UIElement* element);
	UIElement* FindAdjacentElement(UIElement* closestFrom, Vector2 direction, const Camera* sceneCamera);
};

