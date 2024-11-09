#include "ShaderStructs.hlsli"

// takes a unit square and makes it occupy the whole screen
VertexToPixel main(VertexShaderInput input)
{
	VertexToPixel output;
	output.screenPosition = float4(input.position.x * 2.0, input.position.y * 2.0, 1.0, 1.0); // max depth ensures it is drawn behind everything
	output.uv = input.uv;
	return output;
}