#include "ShaderStructs.hlsli"

cbuffer Constants : register(b0) {
	matrix worldViewProj;
	matrix worldTransform;
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

	float4 fullPosition = float4(input.position, 0, 1);
	output.screenPosition = mul(worldViewProj, fullPosition);
	output.worldPosition = mul(worldTransform, fullPosition);
	
	if(flipX) {
        input.uv.x = 1 - input.uv.x;
    }
	if(flipY) {
        input.uv.y = 1 - input.uv.y;
    }

    output.uv = (input.uv + float2(col, row)) * float2(spriteWidth, spriteHeight);

	return output;
}