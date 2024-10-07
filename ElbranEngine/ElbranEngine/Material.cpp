#include "Material.h"

Material::Material(std::shared_ptr<VertexShader> vertexShader, std::shared_ptr<PixelShader> pixelShader) {
	this->vertexShader = vertexShader;
	this->pixelShader = pixelShader;
}

std::shared_ptr<VertexShader> Material::GetVertexShader() {
	return vertexShader;
}

std::shared_ptr<PixelShader> Material::GetPixelShader() {
	return pixelShader;
}
