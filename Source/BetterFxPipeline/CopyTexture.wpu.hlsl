sampler2D s0 : register(s0);

float4 main(in float2 texCoord : TEXCOORD) : COLOR
{
    return tex2Dlod(s0, float4(texCoord, 0, 0));
}