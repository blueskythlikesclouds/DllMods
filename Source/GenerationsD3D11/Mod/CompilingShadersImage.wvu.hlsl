void compiling_shaders_image_vs_main(in uint id : SV_VertexID, out float4 position : SV_Position, out float2 texCoord : TEXCOORD)
{
    texCoord = float2((id << 1) & 2, id & 2) / 2.0;
    position.xy = texCoord;
    position.xy *= float2(157, 22) / float2(1280, 720);
    position.xy += float2(3, 697) / float2(1280, 720);
    position.xy = position.xy * float2(2, -2) + float2(-1, 1);
    position.zw = float2(0, 1);
}