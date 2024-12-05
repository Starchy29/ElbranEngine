#pragma once
#include "Scene.h"
#include "Button.h"
#include "InputManager.h"

class Menu :
    public Scene
{
public:
    bool mouseEnabled;
    bool keysEnabled;

    Menu(Color backgroundColor = Color::Clear);
    Menu(std::shared_ptr<Sprite> backgroundImage);

    void Update(float deltaTime) override;
    virtual void Add(GameObject* object) override;

protected:
    virtual void Remove(GameObject* removed) override;

private:
    InputManager* input;
    Vector2 lastMousePos;
    Button* hovered;
    std::vector<Button*> buttons;

    Button* FindClosest(Button* button, InputAction directionInput);
    static constexpr InputAction inputDirections[4] = { InputAction::Up, InputAction::Down, InputAction::Left, InputAction::Right };
};

