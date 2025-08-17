#pragma once
class Scene;

class IBehavior {
public:
	virtual ~IBehavior() {}
	virtual void Update(float deltaTime) = 0;
};