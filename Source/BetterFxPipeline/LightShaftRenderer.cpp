#include "LightShaftRenderer.h"

uint32_t computeLightShaftSampleWeightFuncAddress = 0xD7C7B0;

void computeLightShaftSampleWeight(uint32_t height, float* sunParamW, float* sampleWeight, const float deviation, const float multiplier)
{
    __asm
    {
        push multiplier
        push deviation
        push sampleWeight
        mov ecx, sunParamW
        mov eax, height
        call [computeLightShaftSampleWeightFuncAddress]
    }
}

FUNCTION_PTR(void, __cdecl, SceneTraversed_ScreenDefaultExec, 0x651520, void* A1, void* A2, int32_t shaderIndex);

void SceneTraversed_ScreenLightShaftFilter(void* A1, void* A2)
{
    uint32_t dataPtr = **(uint32_t**)A1;

    const uint16_t width = *(uint16_t*)(dataPtr + 4);
    const uint16_t height = *(uint16_t*)(dataPtr + 6);
    const uint8_t cameraIndex = *(uint8_t*)(dataPtr + 11);

    hh::mr::CRenderingDevice* device = **(hh::mr::CRenderingDevice***)((char*)A1 + 16);

    float sampleWeight[64];
    float sunParam[8];

    computeLightShaftSampleWeight(height, &sunParam[3], sampleWeight, *(float*)0x1A431D4, *(float*)0x1A431D8);

    device->m_pD3DDevice->SetPixelShaderConstantF(150, sampleWeight, 16);

    dataPtr = (uint32_t)A1 + cameraIndex * 496;

    const Eigen::Vector4f& sunPos = *(Eigen::Vector4f*)0x1A57410;
    const Eigen::Matrix4f& proj = *(Eigen::Matrix4f*)(dataPtr + 48);
    const Eigen::Matrix4f& view = *(Eigen::Matrix4f*)(dataPtr + 432);

    Eigen::Vector4f projectedPos = proj * (view * sunPos);

    float x = projectedPos.x() / projectedPos.w() * +0.5f + 0.5f;
    float y = projectedPos.y() / projectedPos.w() * -0.5f + 0.5f;

    if (projectedPos.z() < 0.0f)
    {
        x *= -1.0f;
        y *= -1.0f;
    }

    const float visibility = (projectedPos.head<3>() / projectedPos.w()).normalized().z();

    sunParam[0] = x;
    sunParam[1] = y;
    sunParam[2] = (float)width / (float)height;
    sunParam[4] = visibility <= 0.0f ? 1.0f + visibility : 1.0f;
    sunParam[5] = 0.0f;
    sunParam[6] = 0.0f;
    sunParam[7] = 0.0f;

    device->m_pD3DDevice->SetPixelShaderConstantF(166, sunParam, 2);

    SceneTraversed_ScreenDefaultExec(A1, A2, 0x352);
}

hh::fx::SScreenRenderParam lightShaftFilterScreenRenderParam =
{
    "light shaft filter",
    (void*)&SceneTraversed_ScreenLightShaftFilter,
    0x352,
    {}
};

hh::fx::SDrawInstanceParam lightShaftFilterScreenDrawInstanceParam0 =
{
    0,
    0,
    "LightShaftFilter0",
    0,
    (void*)0x651820,
    &lightShaftFilterScreenRenderParam,
    0,
    {},
    0,
    16,
    0,
    16,
    {
        {
            0xFF,
            0xFF,
            0xFF,
            15
        }
    },
    3,
    0,
    0x101,
    0,
    0,
    0
};

hh::fx::SDrawInstanceParam lightShaftFilterScreenDrawInstanceParam1 =
{
    0,
    0,
    "LightShaftFilter1",
    0,
    (void*)0x651820,
    &lightShaftFilterScreenRenderParam,
    0,
    {},
    0,
    15,
    0,
    15,
    {
        {
            0xFF,
            0xFF,
            0xFF,
            16
        }
    },
    3,
    0,
    0x101,
    0,
    0,
    0
};

hh::fx::SDrawInstanceParam lightShaftFilterScreenDrawInstanceParamEnd =
{
    0,
    0,
    "LightShaftFilterEnd",
    0,
    (void*)0x651820,
    (void*)0x13DF5A8,
    0,
    {},
    0,
    10,
    0,
    10,
    {
        {
            0xFF,
            0xFF,
            0xFF,
            16
        }
    },
    4,
    0,
    0x101,
    0,
    0,
    0
};

hh::fx::SDrawInstanceParam lightShaftFilterDrawInstanceChildParams[] =
{
    lightShaftFilterScreenDrawInstanceParam0,
    lightShaftFilterScreenDrawInstanceParam1,
    lightShaftFilterScreenDrawInstanceParam0,
    lightShaftFilterScreenDrawInstanceParam1,
    lightShaftFilterScreenDrawInstanceParam0,
    lightShaftFilterScreenDrawInstanceParamEnd
};

hh::fx::SDrawInstanceParam lightShaftFilterDrawInstanceParam =
{
    0,
    0,
    "+Traverse - s_render_lightshaft_filter",
    0,
    (void*)0x651820,
    lightShaftFilterDrawInstanceChildParams,
    _countof(lightShaftFilterDrawInstanceChildParams),
    {},
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    {},
    0xFFFFFFFF,
    0,
    0,
    0,
    0,
    0
};

void SceneTraversed_ScreenLightShaftMask(void* A1, void* A2)
{
    hh::mr::CRenderingDevice* device = **(hh::mr::CRenderingDevice***)((char*)A1 + 16);

    const float maskZ = *(float*)0x1A431E0;
    const float maskPow = *(float*)0x1A431E4;

    float skyMaskParam[] = { maskPow, maskPow, maskPow, maskZ };
    device->m_pD3DDevice->SetPixelShaderConstantF(166, skyMaskParam, 1);

    SceneTraversed_ScreenDefaultExec(A1, A2, 0x351);
}

hh::fx::SScreenRenderParam lightShaftMaskScreenRenderParam =
{
    "light shaft mask",
    (void*)&SceneTraversed_ScreenLightShaftMask,
    0x351,
    {}
};

hh::fx::SDrawInstanceParam lightShaftMaskScreenDrawInstanceParam =
{
    0,
    0,
    "LightShaftMask",
    0,
    (void*)0x651820,
    &lightShaftMaskScreenRenderParam,
    0,
    {},
    0,
    15,
    0,
    15,
    {
        {
            0xFF,
            0xFF,
            11,
            10
        }
    },
    3,
    0,
    0x101,
    0,
    0,
    0
};

hh::fx::SDrawInstanceParam lightShaftMaskDrawInstanceParam =
{
    0,
    0,
    "+Traverse - s_render_lightshaft_mask",
    0,
    (void*)0x651820,
    &lightShaftMaskScreenDrawInstanceParam,
    1,
    {},
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    {},
    0xFFFFFFFF,
    0,
    0,
    0,
    0,
    0
};

hh::fx::SDrawInstanceParam lightShaftDrawInstanceChildParams[] =
{
    lightShaftMaskDrawInstanceParam,
    lightShaftFilterDrawInstanceParam
};

hh::fx::SDrawInstanceParam lightShaftDrawInstanceParam =
{
    0,
    0,
    "+Traverse - s_render_lightshaft",
    0,
    (void*)0x651820,
    lightShaftDrawInstanceChildParams,
    _countof(lightShaftDrawInstanceChildParams),
    {},
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    {},
    0xFFFFFFFF,
    0,
    0,
    0,
    0,
    0
};

uint32_t* FINAL_POST_PROCESS_PARAM_CHILD_COUNT = (uint32_t*)0x13DD6D0;
bool* IS_LIGHT_SHAFT_ENABLE = (bool*)0x1E5E333;

bool LightShaftRenderer::enabled = false;

void LightShaftRenderer::update()
{
    if (!enabled)
        return;

    *FINAL_POST_PROCESS_PARAM_CHILD_COUNT = *IS_LIGHT_SHAFT_ENABLE ? 2 : 1;
}

void LightShaftRenderer::applyPatches()
{
    if (enabled) 
        return;

    enabled = true;

    hh::fx::SDrawInstanceParam* finalPostProcessParam = (hh::fx::SDrawInstanceParam*)0x13DD6B8;

    hh::fx::SDrawInstanceParam* childParams = (hh::fx::SDrawInstanceParam*)finalPostProcessParam->m_ChildParams;
    hh::fx::SDrawInstanceParam* newChildParams = new hh::fx::SDrawInstanceParam[finalPostProcessParam->m_ChildParamCount + 1];

    for (size_t i = 0; i < finalPostProcessParam->m_ChildParamCount; i++)
        newChildParams[i] = childParams[i];

    newChildParams[finalPostProcessParam->m_ChildParamCount] = lightShaftDrawInstanceParam;

    WRITE_MEMORY(&finalPostProcessParam->m_ChildParams, void*, newChildParams);

    DWORD oldProtect;
    VirtualProtect(FINAL_POST_PROCESS_PARAM_CHILD_COUNT, sizeof(uint32_t), PAGE_READWRITE, &oldProtect);
}
