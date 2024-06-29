sampler2D s0 : register(s0);

float4 main(in float2 texCoord : TEXCOORD0) : COLOR0
{
    float3 rgb = tex2D(s0, texCoord).rgb;
    
    const float grayPoint = 0.5;
    const float whitePoint = 4.0;
    const float curve = 2.0;
    const float scale = (whitePoint - grayPoint) / pow(pow((1.0 - grayPoint) / (whitePoint - grayPoint), -curve) - 1.0, 1.0 / curve);
    
    float3 tonemappedRgb = (rgb - grayPoint) / scale;
    tonemappedRgb = saturate(grayPoint + scale * tonemappedRgb / pow(1.0 + pow(tonemappedRgb, curve), 1.0 / curve));
    
    rgb = rgb >= grayPoint ? tonemappedRgb : rgb;
    
    return float4(rgb, 1.0);
}