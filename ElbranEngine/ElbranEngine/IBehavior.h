#pragma once
class GameObject;

class IBehavior
{
public:
	bool enabled;

	IBehavior(GameObject* owner) {
		this->owner = owner;
		enabled = true;
	}

	virtual void Update(float deltaTime) = 0;
	virtual IBehavior* Clone() = 0;

protected:
	GameObject* owner;
};

