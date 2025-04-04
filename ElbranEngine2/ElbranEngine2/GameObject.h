#pragma once
class GameObject
{
public:
	// Transform transform; this should have parent, not the game object
	bool active;

	void Update();

protected:
	// Scene* scene? pls no
	// renderer?
	// behaviors?

private:
	GameObject* parent;

	bool IsActive() const;
};

