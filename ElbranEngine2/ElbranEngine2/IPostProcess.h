#pragma once
#include "Buffers.h"

class GraphicsAPI;

class IPostProcess
{
public:
	~IPostProcess() {}
	virtual void Render(GraphicsAPI* graphics, const RenderTarget* input, RenderTarget* output) = 0;
	virtual bool IsActive() const = 0;
};