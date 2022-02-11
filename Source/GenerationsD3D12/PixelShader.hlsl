float4 main(in float4 svPosition : SV_POSITION, in float3 outNormal : TEXCOORD0) : SV_TARGET
{
	return float4(saturate(normalize(outNormal).y * 0.5 + 0.5).xxx, 1);
}