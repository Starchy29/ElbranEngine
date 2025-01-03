struct Light {
	float4 color;
	float2 worldPosition;
	float radius;
	float brightness;
};

cbuffer LightData : register(b13) {
	float4 ambientLight;
	int numLights;
}
StructuredBuffer<Light> Lights : register(t127);

float4 ApplyLights(float4 startColor, float2 worldPosition) {
	float4 multiplier = ambientLight;
	for(int i = 0; i < numLights; i++) {
		Light light = Lights[i];
		float scalar = saturate(1.0 - distance(worldPosition, light.worldPosition) / light.radius);
		multiplier += (scalar * light.brightness) * light.color;
	}
	multiplier.a = 1;
	
	return saturate(startColor * multiplier);
}