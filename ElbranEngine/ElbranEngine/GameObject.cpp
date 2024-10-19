#include "GameObject.h"
#include "DXGame.h"

GameObject::GameObject(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material) {
	this->mesh = mesh;
	this->material = material;
	colorTint = Color::White;
}

Transform* GameObject::GetTransform() {
	return &transform;
}

void GameObject::Draw(std::shared_ptr<Camera> camera) {
	std::shared_ptr<VertexShader> vertexShader = material->GetVertexShader();
	DirectX::XMFLOAT4X4 worldMat = transform.GetWorldMatrix();
	DirectX::XMFLOAT4X4 viewMat = camera->GetView();
	DirectX::XMFLOAT4X4 projMat = camera->GetProjection();
	vertexShader->SetConstantVariable("worldTransform", &worldMat);
	vertexShader->SetConstantVariable("view", &viewMat);
	vertexShader->SetConstantVariable("projection", &projMat);

	std::shared_ptr<PixelShader> pixelShader = material->GetPixelShader();
	pixelShader->SetConstantVariable("color", &colorTint);
	pixelShader->SetSampler(GameInstance->GetSamplerState());
	pixelShader->SetTexture(sprite);

	vertexShader->SetShader();
	pixelShader->SetShader();

	mesh->Draw();
}
