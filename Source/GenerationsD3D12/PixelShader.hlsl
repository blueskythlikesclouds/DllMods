cbuffer PixelConstants : register(b0, space1)
{
	float4 c[224];
	int4 i[16];
	bool b[16];
};

Texture2D<float4> t0 : register(t0, space2);
SamplerState s0 : register(s0, space3);

float4 main(in float4 svPosition : SV_POSITION, in float3 outNormal : TEXCOORD0, in float2 outTexCoord : TEXCOORD1) : SV_TARGET
{
	return t0.Sample(s0, outTexCoord);
	return float4(saturate(normalize(outNormal).y * 0.5 + 0.5).xxx, 1);
}