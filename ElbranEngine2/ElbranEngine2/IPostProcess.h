#pragma once
#include "Texture2D.h"

class IPostProcess
{
public:
	~IPostProcess() {}
	virtual void Render(const Texture2D* input, Texture2D* output) = 0;
	virtual bool IsActive() const = 0;
};