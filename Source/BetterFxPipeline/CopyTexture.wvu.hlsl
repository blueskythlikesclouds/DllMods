void main(
	in float2 vPosition : POSITION, in float2 vTexCoord : TEXCOORD,
	out float4 oPosition : POSITION, out float2 oTexCoord : TEXCOORD)
{
	oPosition = float4(vPosition.xy, 0, 1);
	oTexCoord = vTexCoord;
}