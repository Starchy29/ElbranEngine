#pragma once
class GameObject
{
public:
	// Transform transform;
	bool active;

	void Update();

protected:
	// Scene* scene?
	// renderer?
	// behaviors?

private:
	GameObject* parent;

	bool IsActive() const;
};

