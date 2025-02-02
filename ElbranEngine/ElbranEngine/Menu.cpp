#include "Menu.h"
#include "Application.h"

#define MENU_WIDTH 100

Menu::Menu(Color backgroundColor)
	: Scene(MENU_WIDTH, backgroundColor)
{
	mouseEnabled = true;
	keysEnabled = true;
	hovered = nullptr;
}

Menu::Menu(std::shared_ptr<Sprite> backgroundImage)
	: Scene(MENU_WIDTH, backgroundImage)
{
	mouseEnabled = true;
	keysEnabled = true;
	hovered = nullptr;
}

void Menu::Update(float deltaTime) {
	Scene::Update(deltaTime);
	InputManager* input = APP->Input();

	if(keysEnabled) {
		for(int i = 0; i < 4; i++) {
			if(!input->JustPressed(inputDirections[i])) {
				continue;
			}

			if(hovered == nullptr) {
				// when none are already hovered, go to the first non-disabled button
				for(int i = 0; i < buttons.size(); i++) {
					if(!buttons[i]->IsDisabled() && buttons[i]->IsActive()) {
						hovered = buttons[i];
						hovered->SetHovered(true);
						break;
					}
				}
			} else {
				Button* newHovered = FindClosest(hovered, inputDirections[i]);
				if(newHovered != nullptr && newHovered != hovered) {
					hovered->SetHovered(false);
					hovered = newHovered;
					hovered->SetHovered(true);
				}
			}
			break;
		}

		if(hovered != nullptr && input->JustPressed(InputAction::Select)) {
			hovered->Click();
			return;
		}
	}
	
	if(mouseEnabled) {
		if(hovered != nullptr && input->KeyJustPressed(VK_MOUSE_LEFT)) {
			hovered->Click();
			return;
		}

		Vector2 mousePos = input->GetMousePosition(camera);
		if(mousePos == lastMousePos) {
			return;
		}

		lastMousePos = mousePos;
		Button* newHover = nullptr;
		for(Button* button : buttons) {
			if(!button->IsDisabled() && button->IsActive() && button->GetTransform()->GetArea().Contains(mousePos)) {
				newHover = button;
				break;
			}
		}

		if(newHover == hovered) {
			return;
		}

		if(hovered != nullptr) {
			hovered->SetHovered(false);
		}
		if(newHover != nullptr) {
			newHover->SetHovered(true);
		}
		hovered = newHover;
	}
}

void Menu::Add(GameObject* object) {
	Scene::Add(object);
	if(object->GetType() == ObjectTag::MenuButton) {
		buttons.push_back((Button*)object);
	}
}

void Menu::Remove(GameObject* removed) {
	if(removed->GetType() == ObjectTag::MenuButton) {
		buttons.erase(std::find(buttons.begin(), buttons.end(), removed));
	}
}

Button* Menu::FindClosest(Button* button, InputAction directionInput) {
	// finds the closest neighbor in the input direction
	Vector2 direction;
	switch(directionInput) {
	case InputAction::Up:
		direction = Vector2::Up;
		break;
	case InputAction::Down:
		direction = Vector2::Down;
		break;
	case InputAction::Left:
		direction = Vector2::Left;
		break;
	case InputAction::Right:
		direction = Vector2::Right;
		break;
	}

	Vector2 screenScale = camera->GetWorldDimensions();
	Vector2 start = button->GetTransform()->GetPosition(true);
	Button* closest = nullptr;
	float minDist;
	for(Button* option : buttons) {
		Vector2 pos = option->GetTransform()->GetPosition(true);

		float dotProd = (pos - start).Dot(direction);
		if(option->IsDisabled() || !option->IsActive() || abs(dotProd) <= 0.1f) {
			continue;
		}

		// wrap around
		if(dotProd < 0) {
			pos += direction * screenScale;
		}

		float distance = pos.SqrDist(start);
		if(closest == nullptr || distance < minDist) {
			closest = option;
			minDist = distance;
		}
	}

	return closest;
}
