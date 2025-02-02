#pragma once
#include <memory>
#include "IBehavior.h"

class Camera;
class Transform;
class VertexShader;
class PixelShader;
namespace DirectX { struct XMFLOAT4X4; }

class IRenderer
	: public IBehavior
{
public:
	bool screenSpace; // if true, render relative to the camera

	IRenderer() { screenSpace = false; }
	virtual ~IRenderer() { }
	virtual void Update(float deltaTime) { }
	virtual void Draw(Camera* camera, const Transform& transform) = 0;

protected:
	std::shared_ptr<VertexShader> vertexShader;
	std::shared_ptr<PixelShader> pixelShader;

	void GetScreenTransform(DirectX::XMFLOAT4X4* output, Camera* camera, const Transform& transform);
};