#pragma once

class IBehavior {
public:
	bool enabled = true;

	virtual ~IBehavior() {}
	virtual void Update(float deltaTime) = 0;
};