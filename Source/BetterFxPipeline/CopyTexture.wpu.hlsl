float4 c0 : register(c0);
sampler2D s0 : register(s0);

float4 main(in float2 vPos : VPOS) : COLOR
{
    return tex2Dlod(s0, float4((vPos + 0.5) * c0.zw, 0, 0));
}