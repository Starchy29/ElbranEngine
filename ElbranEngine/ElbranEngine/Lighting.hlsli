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
	startColor = float4(pow(startColor.rgb, 2.2), startColor.a);
	float4 multiplier = ambientLight;
	for(int i = 0; i < numLights; i++) {
		Light light = Lights[i];
		float dist = distance(worldPosition, light.worldPosition);
		float scalar = saturate(1.0 - dist * dist / (light.radius * light.radius));
		scalar *= scalar;
		multiplier += (scalar * light.brightness) * light.color;
	}
	
	return float4(pow(saturate(startColor.rgb * multiplier.rgb), 1.0f / 2.2f), startColor.a);
}