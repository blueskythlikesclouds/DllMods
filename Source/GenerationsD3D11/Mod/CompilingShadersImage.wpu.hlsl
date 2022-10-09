cbuffer cbGlobals : register(b0)
{
    float4 g_Alpha : packoffset(c150);
}

Texture2D<float4> s0 : register(t0);
SamplerState s0_s : register(s0);

float4 compiling_shaders_image_ps_main(in float4 position : SV_Position, in float2 texCoord : TEXCOORD) : SV_Target
{
    float4 color = s0.Sample(s0_s, texCoord);
    color.a *= g_Alpha.x;
    return color;
}