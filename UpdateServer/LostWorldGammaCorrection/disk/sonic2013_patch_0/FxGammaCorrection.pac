PACx201L     DATA  (  P       d            8   �  P   �  `   �  p      X   �  �      h   �  �     x   �  P  ^              // this is skyth coming here to call you out 
// for being curious and unpacking this PAC file

// 157.109.82.135

float4 g_Gamma : register(c150);
sampler2D s0 : register(s0);

float4 main(in float2 texCoord : TEXCOORD0) : COLOR
{
	float4 color = tex2D(s0, texCoord);
	color.rgb = pow(saturate(color.rgb), g_Gamma.rgb);
	return color;
}  H                  H             8   D           FxGammaCorrection                         H               ����) CTAB   w    ��         p   D    �  ZL       \        `       g_Gamma            s0 �           ps_3_0 Microsoft (R) HLSL Shader Compiler 10.1 �    �  �     � �B    �  � �    �  �   �  ��   �   �   �  U�   �  ��    � 䀖 �   �   �   �  U�   �  ����  wpu.hlsl:ResRawData pixelshader:ResMiragePixelShader wpu:ResMiragePixelShaderCode FxGammaCorrection MAAAAAABAABAABAA