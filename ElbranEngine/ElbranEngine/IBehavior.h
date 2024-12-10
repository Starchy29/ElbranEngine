#pragma once
#include "GameObject.h"

class IBehavior
{
public:
	bool enabled;

	IBehavior() {
		owner = nullptr;
		enabled = true;
	}
	IBehavior(const IBehavior& original) {
		owner = nullptr; // will be set by the copy owner
		enabled = original.enabled;
	}

	virtual ~IBehavior() {}
	virtual void SetOwner(GameObject* owner) { this->owner = owner; }
	virtual void Update(float deltaTime) = 0;
	virtual IBehavior* Clone() = 0;

protected:
	GameObject* owner;
};

