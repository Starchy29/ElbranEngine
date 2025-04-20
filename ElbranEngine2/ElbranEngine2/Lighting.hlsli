#define PI 3.141592653

struct Light {
	float2 worldPosition;
	float2 direction;
	float4 color;
	float radius;
	float brightness;
	float2 coneEdge;
};

cbuffer LightData : register(b1) { // max buffer register
	float4 ambientLight;
	int numLights;
}
StructuredBuffer<Light> Lights : register(t127); // max texture register

float4 ApplyLights(float4 startColor, float2 worldPosition) {
	startColor = float4(pow(startColor.rgb, 2.2), startColor.a); // gamma correction
	float4 multiplier = ambientLight;
	for(int i = 0; i < numLights; i++) {
		Light light = Lights[i];
		
		float dist = distance(worldPosition, light.worldPosition);
		float scalar = saturate(1.0 - dist * dist / (light.radius * light.radius));
		scalar *= scalar;
		
		// become 0 light if out of cone
		float coneDot = dot(light.direction, light.coneEdge);
		float pixelDot = dot(normalize(worldPosition - light.worldPosition), light.direction) + 1.0e-5;
		scalar *= saturate(ceil((pixelDot - coneDot) / (1.0 - coneDot))); // 1 when pixel = direction, 0 when pixel = coneEdge
		
		multiplier += (scalar * light.brightness) * light.color;
	}
	
	return float4(pow(saturate(startColor.rgb * multiplier.rgb), 1.0f / 2.2f), startColor.a);
}