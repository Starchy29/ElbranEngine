#include "ShaderStructs.hlsli"

struct VertexShaderInput {
	float2 screenPos : POSITION;
	float2 uv : TEXCOORD;
};

VertexToPixel main(VertexShaderInput input)
{
	VertexToPixel output;

	output.screenPosition = float4(input.screenPos.x, input.screenPos.y, 0, 0);
	output.uv = input.uv;

	return output;
}