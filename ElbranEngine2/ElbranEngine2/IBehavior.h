#pragma once

class IBehavior {
public:
	IBehavior() {}
	virtual ~IBehavior() {}

	virtual void Update(float deltaTime) = 0;
};