// helper functions for converting between RGB and HSV

float3 toHSV(float3 color) {
    // hue is based on the ratio from the greatest channel to the second greatest. (The minimum channel only decreases saturation)
    // saturation is the percentage of the minimum channel out of the maximum channel
    // value (brightness) is equal to the greatest value from any one channel
    // note: any grayscale color has a hue of 0, which is the same as pure red
    
    float4 constants = float4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    float4 bgComp = color.g < color.b ? float4(color.bg, constants.wz) : float4(color.gb, constants.xy); // x is max(g,b), y is min(g,b), z and w are constants
    float4 channels = color.r < bgComp.x ? float4(bgComp.xyw, color.r) : float4(color.r, bgComp.yzx); // x is max(r,g,b), z is a constant, y and w are the non-max channels
    
    float delta = channels.x - min(channels.w, channels.y); // max channel - min channel
    float tiny = 1.0e-10; // avoid divide by 0 without branching
    return float3(abs(channels.z + (channels.w - channels.y) / (6.0 * delta + tiny)), delta / (channels.x + tiny), channels.x);
}

float3 toRGB(float3 hsv) {
    // convert each channel to range [0,2] and round down to 1 so the channels overlap for colors like yellow (1, 1, 0)
    float3 ratios = abs(frac(hsv.xxx + float3(0.0, 2.0 / 3.0, 1.0 / 3.0)) * 6.0 - float3(3.0, 3.0, 3.0)) - float3(1.0, 1.0, 1.0);
    return hsv.z * lerp(float3(1.0, 1.0, 1.0), saturate(ratios), hsv.y); // lerp from grayscale to true hue, then scale by brightness
}