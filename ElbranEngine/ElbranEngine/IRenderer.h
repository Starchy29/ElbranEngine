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
	std::shared_ptr<Mesh> mesh;

	virtual ~IRenderer() { }
	virtual void Update(float deltaTime) { }
	virtual void Draw(Camera* camera, const Transform& transform) = 0;

protected:
	std::shared_ptr<VertexShader> vertexShader;
	std::shared_ptr<PixelShader> pixelShader;

	static DirectX::XMMATRIX CreateWorldViewProjection(Camera* camera, const Transform &transform);
};