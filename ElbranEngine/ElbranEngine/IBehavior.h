#pragma once
#include "GameObject.h"

class IBehavior
{
public:
	IBehavior(GameObject* owner) {
		this->owner = owner;
	}

	virtual void Update(float deltaTime) = 0;
	virtual IBehavior* Copy() = 0;

protected:
	GameObject* owner;
};

