#pragma once
#include "Camera.h"
#include <memory>
#include "Transform.h"
#include "Mesh.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "IBehavior.h"

class IRenderer
	: public IBehavior
{
public:
	bool screenSpace; // if true, render relative to the camera (including z position)

	IRenderer() { screenSpace = false; }
	virtual ~IRenderer() { }
	virtual void Update(float deltaTime) { }
	virtual void Draw(Camera* camera, const Transform& transform) = 0;

protected:
	std::shared_ptr<VertexShader> vertexShader;
	std::shared_ptr<PixelShader> pixelShader;

	void GetScreenTransform(DirectX::XMFLOAT4X4* output, Camera* camera, const Transform& transform);
};