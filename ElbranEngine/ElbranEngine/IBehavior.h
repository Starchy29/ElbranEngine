#pragma once
#include "GameObject.h"

class IBehavior
{
public:
	bool enabled;

	IBehavior(GameObject* owner) {
		this->owner = owner;
		enabled = true;
	}

	virtual void Update(float deltaTime) = 0;
	virtual IBehavior* Copy() = 0;

protected:
	GameObject* owner;
};

