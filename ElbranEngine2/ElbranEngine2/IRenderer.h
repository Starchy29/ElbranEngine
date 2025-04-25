#pragma once
#include "Transform.h"

class IRenderer {
public:
	Transform* transform;
	const Matrix* worldMatrix;

	virtual ~IRenderer() {}

	virtual void Draw() = 0;
};