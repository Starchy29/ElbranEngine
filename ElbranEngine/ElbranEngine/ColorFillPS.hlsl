#include "ShaderStructs.hlsli"

float4 main(VertexToPixel input) : SV_TARGET
{
	return input.color;
}