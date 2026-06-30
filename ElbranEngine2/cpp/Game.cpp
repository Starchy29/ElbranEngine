#include "Game.h"
#include "InputManager.h"

enum class InputAction {
	Up = 0,
	Down,
	Left,
	Right,
	Select,
	Back,

	COUNT
};

void Game::Initialize(AppComponents app) {
	// set default key bindings
	app.input->controllers = &controls;
	app.input->playerCount = 1;
	controls.bindings[(int)InputAction::Up] ={{'W', KEY_UP_ARROW, 0}, {GamepadButton::LStickUp, GamepadButton::DpadUp, GamepadButton::None}};
	controls.bindings[(int)InputAction::Down] = {{'S', KEY_DOWN_ARROW, 0}, { GamepadButton::LStickDown, GamepadButton::DpadDown, GamepadButton::None}};
	controls.bindings[(int)InputAction::Left] = {{'A', KEY_LEFT_ARROW, 0}, { GamepadButton::LStickLeft, GamepadButton::DpadLeft, GamepadButton::None}};
	controls.bindings[(int)InputAction::Right] = {{'D', KEY_RIGHT_ARROW, 0}, { GamepadButton::LStickRight, GamepadButton::DpadRight, GamepadButton::None}};
	controls.bindings[(int)InputAction::Select] = {{KEY_MOUSE_LEFT, KEY_ENTER, 0}, { GamepadButton::A, GamepadButton::None, GamepadButton::None}};
	controls.bindings[(int)InputAction::Back] = {{KEY_BACKSPACE, KEY_ESCAPE, 0}, { GamepadButton::B, GamepadButton::None, GamepadButton::None}};
}

void Game::Release(const GraphicsAPI* graphics) {
	
}

void Game::Update(AppComponents app, float deltaTime) {
	
}

void Game::Draw(DrawComponents app) {
	
}
