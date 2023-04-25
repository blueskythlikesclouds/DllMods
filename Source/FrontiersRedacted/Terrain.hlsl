cbuffer cbWorld : register(b0)
{
    row_major float4x4 view_matrix : packoffset(c0);
    row_major float4x4 proj_matrix : packoffset(c4);
    row_major float4x4 inv_view_matrix : packoffset(c8);
    row_major float4x4 inv_proj_matrix : packoffset(c12);
    float4 global_clip_plane : packoffset(c16);
    float4 clear_color : packoffset(c17);
    row_major float4x4 view_proj_matrix : packoffset(c18);
    row_major float4x4 inv_view_proj_matrix : packoffset(c22);
    row_major float4x4 prev_view_proj_matrix : packoffset(c26);
    row_major float4x4 uv_reproj_matrix : packoffset(c30);
    float4 jitter_offset : packoffset(c34);
    float4 global_mip_bias : packoffset(c35);
    float4 shadow_camera_view_matrix_third_row : packoffset(c36);
    row_major float4x4 shadow_view_matrix : packoffset(c37);
    row_major float4x4 shadow_view_proj_matrix : packoffset(c41);
    float4 shadow_map_parameter[2] : packoffset(c45);
    float4 shadow_map_size : packoffset(c47);
    float4 shadow_cascade_offset[4] : packoffset(c48);
    float4 shadow_cascade_scale[4] : packoffset(c52);
    float4 shadow_parallax_correct_param[4] : packoffset(c56);
    float4 shadow_cascade_frustums_eye_space_depth : packoffset(c60);
    float4 shadow_cascade_transition_scale : packoffset(c61);
    row_major float4x4 heightmap_view_matrix : packoffset(c62);
    row_major float4x4 heightmap_view_proj_matrix : packoffset(c66);
    float4 heightmap_parameter : packoffset(c70);
    float4 planar_reflection_parameter : packoffset(c71);
    float4 u_lightColor : packoffset(c72);
    float4 u_lightDirection : packoffset(c73);
    float4 u_cameraPosition : packoffset(c74);
    float4 g_probe_data[72] : packoffset(c75);
    float4 g_probe_pos[24] : packoffset(c147);
    float4 g_probe_param[24] : packoffset(c171);
    float4 g_probe_shparam[168] : packoffset(c195);
    float4 g_probe_count : packoffset(c363);
    float4 g_LightScattering_Ray_Mie_Ray2_Mie2 : packoffset(c364);
    float4 g_LightScattering_ConstG_FogDensity : packoffset(c365);
    float4 g_LightScatteringFarNearScale : packoffset(c366);
    float4 g_LightScatteringColor : packoffset(c367);
    float4 g_alphathreshold : packoffset(c368);
    float4 g_smoothness_param : packoffset(c369);
    float4 g_time_param : packoffset(c370);
    float4 u_ibl_param : packoffset(c371);
    float4 u_planar_projection_shadow_plane : packoffset(c372);
    float3 u_planar_projection_shadow_light_position : packoffset(c373);
    float4 u_planar_projection_shadow_color : packoffset(c374);
    float4 u_planar_projection_shadow_param : packoffset(c375);
    float4 g_global_user_param_0 : packoffset(c376);
    float4 g_global_user_param_1 : packoffset(c377);
    float4 g_global_user_param_2 : packoffset(c378);
    float4 g_global_user_param_3 : packoffset(c379);
    bool u_enable_fog_d : packoffset(c380);
    bool u_enable_fog_h : packoffset(c380.y);
    float4 u_fog_param_0 : packoffset(c381);
    float4 u_fog_param_1 : packoffset(c382);
    float4 u_fog_param_2 : packoffset(c383);
    float4 u_fog_param_3 : packoffset(c384);
    float4 g_tonemap_param : packoffset(c385);
    float4 u_contrast_factor[3] : packoffset(c386);
    float4 u_hls_offset : packoffset(c389);
    float4 u_hls_rgb : packoffset(c390);
    float4 u_hls_colorize : packoffset(c391);
    float4 u_color_grading_factor : packoffset(c392);
    float4 u_screen_info : packoffset(c393);
    float4 u_current_viewport_mask : packoffset(c394);
    float4 u_viewport_info : packoffset(c395);
    float4 u_prev_viewport_info : packoffset(c396);
    float4 u_view_param : packoffset(c397);
    float4 u_sggi_param[2] : packoffset(c398);
    float4 u_histogram_param : packoffset(c400);
    float4 u_occlusion_capsule_param[2] : packoffset(c401);
    float4 u_ssao_param : packoffset(c403);
    float4 u_wind_param[5] : packoffset(c404);
    float4 u_wind_frequencies : packoffset(c409);
    float4 u_wind_global_param[3] : packoffset(c410);
    float4 u_grass_lod_distance : packoffset(c413);
    float4 u_grass_dither_distance : packoffset(c414);
    float4 u_weather_param : packoffset(c415);
    float4 u_hiz_param : packoffset(c416);
    float4 u_rlr_param[2] : packoffset(c417);
    float4 u_sky_sh_param[9] : packoffset(c419);
    float4 u_tile_info : packoffset(c428);
    row_major float4x4 u_local_light_shadow_matrix : packoffset(c429);
    float4 u_local_light_shadow_param : packoffset(c433);
    float4 u_detail_param : packoffset(c434);
    bool enable_ibl_plus_directional_specular : packoffset(c435);
    bool enable_rlr : packoffset(c435.y);
    bool enable_rlr_trans_surface : packoffset(c435.z);
    bool enable_vol_shadow : packoffset(c435.w);
    float4 u_interaction_param[2] : packoffset(c436);
    float4 u_cloud_shadow_param : packoffset(c438);
    float4 u_vol_shadow_param[5] : packoffset(c439);
}

cbuffer cbMaterialDynamic : register(b2)
{
    row_major float4x4 world_matrix : packoffset(c0);
    row_major float4x4 prev_world_matrix : packoffset(c4);
    float4 light_field_color[8] : packoffset(c8);
    float4 u_modulate_color : packoffset(c16);
    float4 u_forcetrans_param : packoffset(c17);
    float4 u_view_offset : packoffset(c18);
    float4 u_vat_param : packoffset(c19);
    float4 u_timestamp : packoffset(c20);
    float4 u_model_user_param_0 : packoffset(c21);
    float4 u_model_user_param_1 : packoffset(c22);
    float4 u_model_user_param_2 : packoffset(c23);
    float4 u_model_user_param_3 : packoffset(c24);
    bool enable_shadow_map : packoffset(c25);
    bool u_disable_max_bone_influences_8 : packoffset(c25.y);
}

cbuffer cb_shlightfield_probes : register(b6)
{
    float4 shlightfield_default : packoffset(c0);
    float4 shlightfield_multiply_color_up : packoffset(c1);
    float4 shlightfield_multiply_color_down : packoffset(c2);
    float4 shlightfield_probes_SHLightFieldProbe[27] : packoffset(c3);
    float4 shlightfield_probe_SHLightFieldProbe_end : packoffset(c256);
}

cbuffer cbMaterialImmutable : register(b1)
{
    float4 PBRFactor : packoffset(c0);
    float4 TerrainInfo : packoffset(c1);
    float4 UVScale : packoffset(c2);
    float4 BlendParam : packoffset(c3);
    float4 PageInfo : packoffset(c4);
    float4 VirtualTextureParam[6] : packoffset(c5);
    float4 SectorCenter : packoffset(c11);
    float4 SectorInfo[25] : packoffset(c12);
    float4 CutParam[2] : packoffset(c37);
    float4 WetParam[3] : packoffset(c39);
    float4 MaskParam : packoffset(c42);
    float4 DentInfo[2] : packoffset(c43);
    int4 MaterialIdTable[64] : packoffset(c45);
    int4 MaterialAttributes[64] : packoffset(c109);
    int4 MaterialTypes : packoffset(c173);
    bool enable_debug_draw_grid : packoffset(c174);
    bool enable_debug_highlight_hole_material : packoffset(c174.y);
}

SamplerState s_DepthBuffer_sampler_s : register(s0);
SamplerState s_TerrainDetailAlbedoMap_sampler_s : register(s1);
SamplerState s_TerrainDetailNormalHeightMap_sampler_s : register(s2);
SamplerState s_TerrainDetailParameterMap_sampler_s : register(s3);
SamplerState s_TerrainBaseAlbedoMap_sampler_s : register(s4);
SamplerState s_TerrainBaseParameterMap_sampler_s : register(s5);
SamplerState s_TerrainSplatMap_sampler_s : register(s6);
SamplerState s_TerrainNormalMap_s : register(s7);
SamplerState s_DentBuffer_sampler_s : register(s8);
SamplerState s_Noise_s : register(s9);
SamplerState s_Ripple_s : register(s10);
Texture2D<float4> s_TerrainNormalMap : register(t0);
Texture3D<float4> s_Noise : register(t1);
Texture2D<float4> s_Ripple : register(t2);
Texture2D<float4> s_DepthBuffer : register(t3);
Texture2DArray<float4> s_TerrainDetailAlbedoMap : register(t4);
Texture2DArray<float4> s_TerrainDetailNormalHeightMap : register(t5);
Texture2DArray<float4> s_TerrainDetailParameterMap : register(t6);
Texture2DArray<float4> s_TerrainBaseAlbedoMap : register(t7);
Texture2DArray<float4> s_TerrainBaseParameterMap : register(t8);
Texture2DArray<float4> s_TerrainSplatMap : register(t9);
Texture2D<float4> s_DentBuffer : register(t10);

static const float2 UV_SCALES[] =
{
    float2(0.10, 0.10),
    float2(0.05, 0.05),
    float2(0.10, 0.10),
    float2(0.04, 0.04),
    float2(0.05, 0.05),
    float2(0.10, 0.10),
    float2(0.05, 0.05),
    float2(0.05, 0.05),
    float2(0.10, 0.10),
    float2(0.10, 0.10),
    float2(0.05, 0.05),
    float2(0.09, 0.09),
    float2(0.05, 0.05),
    float2(0.11, 0.11),
    float2(0.20, 0.20),
    float2(0.14, 0.14),
    float2(0.05, 0.05),
    float2(0.05, 0.05),
    float2(0.05, 0.05),
    float2(0.05, 0.05),
    float2(0.05, 0.05),
    float2(0.07, 0.07),
    float2(0.07, 0.07),
    float2(0.05, 0.05),
    float2(0.15, 0.15),
    float2(0.10, 0.10),
    float2(0.10, 0.10),
    float2(0.07, 0.07),
    float2(0.04, 0.04),
    float2(0.05, 0.16),
    float2(0.03, 0.03),
    float2(0.05, 0.05),
    float2(0.05, 0.05),
    float2(0.03, 0.03),
    float2(0.05, 0.05),
    float2(0.45, 0.45),
    float2(0.20, 0.20),
    float2(0.10, 0.10),
    float2(0.59, 0.59),
    float2(0.15, 0.15),
    float2(0.20, 0.20),
    float2(0.35, 0.35),
    float2(0.20, 0.20),
    float2(0.10, 0.10),
    float2(0.15, 0.15),
    float2(0.20, 0.20),
    float2(0.15, 0.15),
    float2(0.20, 0.20),
    float2(0.05, 0.05),
    float2(0.05, 0.05),
    float2(0.05, 0.05),
    float2(0.05, 0.05),
    float2(0.05, 0.05),
    float2(0.10, 0.10),
    float2(0.10, 0.10),
    float2(0.05, 0.05),
    float2(0.05, 0.05),
    float2(0.05, 0.05),
    float2(0.10, 0.10),
    float2(0.08, 0.08),
    float2(0.04, 0.04),
    float2(0.10, 0.10),
    float2(0.05, 0.05),
    float2(0.05, 0.05),
    float2(0.10, 0.10),
    float2(0.25, 0.25),
    float2(0.04, 0.05),
    float2(0.08, 0.08),
    float2(0.08, 0.08),
    float2(0.20, 0.20),
    float2(0.10, 0.10),
    float2(0.15, 0.15),
    float2(0.04, 0.04),
    float2(0.25, 0.25),
    float2(0.05, 0.05),
    float2(0.15, 0.15),
    float2(0.05, 0.05),
    float2(0.08, 0.08),
    float2(0.10, 0.10),
    float2(0.07, 0.07),
    float2(0.05, 0.05),
    float2(0.23, 0.23),
    float2(0.16, 0.16),
    float2(0.16, 0.16),
    float2(0.04, 0.04),
    float2(0.10, 0.10),
    float2(0.05, 0.05),
    float2(0.10, 0.10)
};

#define cmp -

void main(
    float4 v0 : SV_POSITION0,
    float2 v1 : TEXCOORD0,
    out float4 o0 : SV_Target0,
    out float4 o1 : SV_Target1,
    out float4 o2 : SV_Target2,
    out float4 o3 : SV_Target3)
{
    float2 screenPos = u_screen_info.zw * v0.xy;
    float depth = s_DepthBuffer.Sample(s_DepthBuffer_sampler_s, screenPos).x;
    float2 normalizedPos = u_viewport_info.zw * screenPos * float2(2.0, -2.0) + float2(-1.0, 1.0);
    float4 worldPos = mul(float4(normalizedPos, depth, 1.0), inv_view_proj_matrix);
    worldPos.xyz /= worldPos.w;

    float2 trrTexCoord = worldPos.xz / TerrainInfo.x * 0.5 + 0.5;

    float3 normal = normalize(s_TerrainNormalMap.Sample(s_TerrainNormalMap_s, trrTexCoord).xyz * 2.0 - 1.0);
    float3 tangent = normalize(cross(normal, float3(0, 0, 1)));
    float3 binormal = normalize(cross(normal, tangent));

    uint4 matIndexA = uint4(s_TerrainSplatMap.GatherRed(s_TerrainSplatMap_sampler_s, float3(trrTexCoord, 0)) * 255.0 + 0.5);
    uint4 matIndexB = uint4(s_TerrainSplatMap.GatherRed(s_TerrainSplatMap_sampler_s, float3(trrTexCoord, 1)) * 255.0 + 0.5);
    float4 matBlend = s_TerrainSplatMap.GatherRed(s_TerrainSplatMap_sampler_s, float3(trrTexCoord, 2));

    float4 matAlbedo[4];
    matAlbedo[0] = 0.0;
    matAlbedo[1] = 0.0;
    matAlbedo[2] = 0.0;
    matAlbedo[3] = 0.0;

    float4 matNormalMap[4];
    matNormalMap[0] = 0.0;
    matNormalMap[1] = 0.0;
    matNormalMap[2] = 0.0;
    matNormalMap[3] = 0.0;

    float2 matTexCoords[3];
    matTexCoords[0] = worldPos.yz;
    matTexCoords[1] = worldPos.xz;
    matTexCoords[2] = worldPos.xy;

    [unroll] for (uint i = 0; i < 4; i++)
    {
        [unroll] for (uint j = 0; j < 3; j++)
        {
            uint texIndexA = MaterialIdTable[matIndexA[i] / 4][matIndexA[i] % 4];
            uint texIndexB = MaterialIdTable[matIndexB[i] / 4][matIndexB[i] % 4];

            float2 texCoordA = matTexCoords[j] * UV_SCALES[matIndexA[i]];
            float2 texCoordB = matTexCoords[j] * UV_SCALES[matIndexB[i]];

            float4 matAlbedoA = s_TerrainDetailAlbedoMap.SampleBias(s_TerrainDetailAlbedoMap_sampler_s, float3(texCoordA, texIndexA), global_mip_bias.x);
            float4 matAlbedoB = s_TerrainDetailAlbedoMap.SampleBias(s_TerrainDetailAlbedoMap_sampler_s, float3(texCoordB, texIndexB), global_mip_bias.x);

            float4 matNormalMapA = s_TerrainDetailNormalHeightMap.SampleBias(s_TerrainDetailNormalHeightMap_sampler_s, float3(texCoordA, texIndexA), global_mip_bias.x);
            float4 matNormalMapB = s_TerrainDetailNormalHeightMap.SampleBias(s_TerrainDetailNormalHeightMap_sampler_s, float3(texCoordB, texIndexB), global_mip_bias.x);

            matAlbedoA.rgb = pow(abs(matAlbedoA.rgb), 2.2);
            matAlbedoB.rgb = pow(abs(matAlbedoB.rgb), 2.2);

            matAlbedo[i] += lerp(matAlbedoA, matAlbedoB, matBlend[i]) * (normal[j] * normal[j]);
            matNormalMap[i] += lerp(matNormalMapA, matNormalMapB, matBlend[i]) * (normal[j] * normal[j]);
        }
    }

    uint4 dimensions;
    s_TerrainSplatMap.GetDimensions(0, dimensions.x, dimensions.y, dimensions.z, dimensions.w);

    float2 fraction = frac(trrTexCoord * dimensions.xy + 0.5);

    float4 albedo = lerp(lerp(matAlbedo[3], matAlbedo[2], fraction.x), lerp(matAlbedo[0], matAlbedo[1], fraction.x), fraction.y);
    float4 normalMap = lerp(lerp(matNormalMap[3], matNormalMap[2], fraction.x), lerp(matNormalMap[0], matNormalMap[1], fraction.x), fraction.y);
    float roughness = max(0.01, 1.0 - normalMap.z);

    normalMap.xy = normalMap.xy * 2.0 - 1.0;
    normalMap.z = sqrt(1.0 - saturate(dot(normalMap.xy, normalMap.xy)));

    normal = normalize(normalMap.x * tangent + normalMap.y * binormal + normalMap.z * normal);

    float4 r0 = float4(v0.xy, 0.0, 0.0);
    float4 r1 = 0.0;
    float4 r2 = mul(float4(worldPos.xyz, 1.0), prev_view_proj_matrix).xyww;
    float4 r4 = 0.0;
    float4 r5 = float4(normal.xzy, 0.0);
    float4 r12 = 0.0;

    r0.w = -r5.x;
    r0.w = max(r5.x, r0.w);
    r1.w = -r5.y;
    r1.w = max(r5.y, r1.w);
    r0.w = r1.w + r0.w;
    r1.w = -r5.z;
    r1.w = max(r5.z, r1.w);
    r0.w = r1.w + r0.w;
    r4.yzw = r5.yxz / r0.www;
    r0.w = cmp(r4.w >= 0);
    r5.xy = -r4.yz;
    r5.xy = max(r5.xy, r4.yz);
    r5.xy = -r5.xy;
    r5.xy = float2(1, 1) + r5.xy;
    r1.w = cmp(r4.z >= 0);
    r2.w = -r5.x;
    r12.x = r1.w ? r5.x : r2.w;
    r1.w = cmp(r4.y >= 0);
    r2.w = -r5.y;
    r12.y = r1.w ? r5.y : r2.w;
    r4.yz = r0.ww ? r4.zy : r12.xy;
    r4.yz = float2(0.5, 0.5) * r4.yz;
    r4.yz = float2(0.5, 0.5) + r4.yz;
    r4.yz = max(float2(0, 0), r4.yz);
    r5.xy = min(float2(1, 1), r4.yz);
    r2.xy = r2.xy / r2.zz;
    r2.xy = float2(1, -1) * r2.xy;
    r2.xy = float2(1, 1) + r2.xy;
    r2.xy = float2(0.5, 0.5) * r2.xy;
    r2.xy = u_viewport_info.xy * r2.xy;
    r2.zw = -jitter_offset.xy;
    r0.xy = r2.zw + r0.xy;
    r2.zw = -jitter_offset.zw;
    r2.xy = r2.xy + r2.zw;
    r2.xy = -r2.xy;
    r5.zw = r2.xy + r0.xy;

    o0 = float4(albedo.rgb, 66.5 / 255.0);
    o1 = r5;
    o2 = float4(0.0, 0.0, 0.0, 0.0001);
    o3 = float4(0.04, roughness, 1.0, 0.0);
}