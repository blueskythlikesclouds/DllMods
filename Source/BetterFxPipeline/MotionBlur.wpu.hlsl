float4 g_BlurRate : register(c150);
float4 g_ViewportSize : register(c24);

sampler2D sampColor : register(s0);
sampler2D sampVelocityMap : register(s1);
sampler2D sampZBuffer : register(s2);

float4 main(in float2 texCoord : TEXCOORD) : COLOR
{
	float depth = tex2Dlod(sampZBuffer, float4(texCoord, 0, 0)).x;
	float4 velocityMap = tex2Dlod(sampVelocityMap, float4(texCoord, 0, 0));
	float2 velocity = (velocityMap.xz + velocityMap.yw / 255.0) * 2.0 - 1.0;

	int sampleCount = min(255, ceil(length(velocity * g_ViewportSize.xy)));
	float2 sampleOffset = velocity / (float)sampleCount;

	float3 color = tex2Dlod(sampColor, float4(texCoord, 0, 0)).rgb;
	int count = 1;

	for (int i = 1; i <= sampleCount; i++)
	{
		float4 sampleCoord = float4(texCoord + sampleOffset * i, 0, 0);
		float3 sampleColor = tex2Dlod(sampColor, sampleCoord).rgb;
		float sampleDepth = tex2Dlod(sampZBuffer, sampleCoord).x;

		if (depth - sampleDepth < 0.01)
		{
			color += sampleColor;
			count += 1;
		}
	}

	return float4(color / count, g_BlurRate.x * saturate(dot(abs(velocity), g_ViewportSize.xy) / 8.0) * saturate(count - 1));
}
