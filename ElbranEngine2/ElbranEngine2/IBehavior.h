#pragma once
class Scene;

class IBehavior {
public:
	Scene* scene;
	bool enabled = true;

	virtual ~IBehavior() {}
	virtual void Update(float deltaTime) = 0;
};