#pragma once

class IBehavior {
public:
	virtual ~IBehavior() {}
	virtual void Update(float deltaTime) = 0;
};