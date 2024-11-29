#pragma once
#include "Scene.h"
#include "Button.h"

class Menu :
    public Scene
{
public:

private:
    std::vector<Button*> buttons;
};

