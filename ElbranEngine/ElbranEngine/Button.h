#pragma once
#include "GameObject.h"

class Button :
    public GameObject
{
public:
    void Update(float deltaTime) override;
};

