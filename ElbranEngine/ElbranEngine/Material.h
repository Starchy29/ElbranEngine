#pragma once
#include <memory>
#include "VertexShader.h"
#include "PixelShader.h"

class Material {
public:
	Material(std::shared_ptr<VertexShader> vertexShader, std::shared_ptr<PixelShader> pixelShader);

	std::shared_ptr<VertexShader> GetVertexShader();
	std::shared_ptr<PixelShader> GetPixelShader();

private:
	std::shared_ptr<VertexShader> vertexShader;
	std::shared_ptr<PixelShader> pixelShader;
};

