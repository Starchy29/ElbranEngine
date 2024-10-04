#include "ShaderStructs.hlsli"

struct VertexShaderInput {
	float2 screenPos : SV_POSITION;
	float2 uv : TEXCOORD;
};

VertexToPixel main(VertexShaderInput input)
{
	VertexToPixel output;

	output.screenPosition = float4(input.screenPos.x, input.screenPos.y, 0, 1);
	output.uv = input.uv;

	return output;
}