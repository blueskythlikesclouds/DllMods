cbuffer VertexShaderConstants : register(b0, space0)
{
	row_major float4x4 g_MtxProjection;
	row_major float4x4 g_MtxView;
	row_major float4x4 g_MtxWorld;
	float gap0[16];
	row_major float3x4 g_MtxPalette[25];
	float gap1[153];
	int4 i[16];
	bool b[16];
};

float4 main(in float4 pos : POSITION, in float3 normal : NORMAL, in float4 texCoord : TEXCOORD0, in uint4 blendIndices : BLENDINDICES, in float4 blendWeight : BLENDWEIGHT, out float3 outNormal : TEXCOORD0, out float2 outTexCoord : TEXCOORD1) : SV_POSITION
{
	outNormal = normal;
	outTexCoord = texCoord.xy;

	float3 position = pos.xyz;
	if (b[0])
	{
		float3x4 blendMatrix = g_MtxPalette[blendIndices[0]] * blendWeight[0];
		blendMatrix += g_MtxPalette[blendIndices[1]] * blendWeight[1];
		blendMatrix += g_MtxPalette[blendIndices[2]] * blendWeight[2];
		blendMatrix += g_MtxPalette[blendIndices[3]] * (1 - blendWeight[0] - blendWeight[1] - blendWeight[2]);

		position = mul(blendMatrix, float4(position, 1));
		outNormal = mul(blendMatrix, float4(outNormal, 0));
	}

	normal = mul(float4(normal, 0), g_MtxWorld).xyz;
	return mul(mul(mul(float4(position, 1), g_MtxWorld), g_MtxView), g_MtxProjection);
}