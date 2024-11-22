#include "ShaderStructs.hlsli"

// creates a full screen triangle from 3 vertices
VertexToPixel main(uint id : SV_VERTEXID)
{
	// position is -1 to 3, uvs are 0 to 2
	VertexToPixel output;
	output.uv = float2((id << 1) & 2, id & 2);
	output.screenPosition = float4(output.uv.x * 2 - 1, output.uv.y * -2 + 1, 1.0, 1.0); // max depth ensures it is drawn behind everything
	return output;
}