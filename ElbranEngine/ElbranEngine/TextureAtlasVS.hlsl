#include "ShaderStructs.hlsli"

cbuffer Constants : register(b0) {
	matrix worldViewProj;
	bool flipX;
	bool flipY;
    float spriteWidth;
    float spriteHeight;
    int row;
    int col;
}

VertexToPixel main(VertexShaderInput input)
{
	VertexToPixel output;

	output.screenPosition = mul(worldViewProj, float4(input.position, 0, 1));
	
	if(flipX) {
        input.uv.x = 1 - input.uv.x;
    }
	if(flipY) {
        input.uv.y = 1 - input.uv.y;
    }

    output.uv = (input.uv + float2(row, col)) * float2(spriteWidth, spriteHeight);

	return output;
}