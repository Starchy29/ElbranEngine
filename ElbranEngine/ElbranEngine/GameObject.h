#pragma once
#include <memory>
#include "Mesh.h"
#include "Transform.h"
#include "Camera.h"
#include "Color.h"
#include "VertexShader.h"
#include "PixelShader.h"

class GameObject {
public:
	bool active;
	bool visible;
	bool toBeDeleted;

	std::shared_ptr<Mesh> mesh;
	std::shared_ptr<VertexShader> vertexShader;
	std::shared_ptr<PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> sprite;
	Color colorTint;

	GameObject(Color color);
	GameObject(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> sprite);
	Transform* GetTransform();

	virtual void Update(float deltaTime);
	virtual void Draw(Camera* camera);

private:
	Transform transform;
};

