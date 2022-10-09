cbuffer cbGlobals : register(b0)
{
    float4 g_ViewportSize : packoffset(c180);
}

void compiling_shaders_image_vs_main(in uint id : SV_VertexID, out float4 position : SV_Position, out float2 texCoord : TEXCOORD)
{
    texCoord = float2((id << 1) & 2, id & 2) / 2.0;
    position.xy = texCoord;

    float2 resolution = 1.0 / 720;
    resolution.x *= g_ViewportSize.y * g_ViewportSize.z;

    position.xy *= float2(157, 22) * resolution;
    position.xy += float2(3, 697) * resolution;
    position.xy = position.xy * float2(2, -2) + float2(-1, 1);
    position.zw = float2(0, 1);
}