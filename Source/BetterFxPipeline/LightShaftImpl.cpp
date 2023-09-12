#include "LightShaftImpl.h"

static uint32_t s_computeLightShaftSampleWeightFuncAddress = 0xD7C7B0;

static void computeLightShaftSampleWeight(uint32_t height, float* sunParamW, float* sampleWeight, float deviation, float multiplier)
{
    __asm
    {
        push multiplier
        push deviation
        push sampleWeight
        mov ecx, sunParamW
        mov eax, height
        call[s_computeLightShaftSampleWeightFuncAddress]
        add esp, 0xC
    }
}

static FUNCTION_PTR(void, __cdecl, screenDefaultExec, 0x651520, void* A1, void* A2, int32_t shaderIndex);

static void screenLightShaftFilter(void* A1, void* A2)
{
    uintptr_t dataPtr = **static_cast<uintptr_t**>(A1);

    const uint16_t width = *reinterpret_cast<uint16_t*>(dataPtr + 4);
    const uint16_t height = *reinterpret_cast<uint16_t*>(dataPtr + 6);
    const uint8_t cameraIndex = *reinterpret_cast<uint8_t*>(dataPtr + 11);

    const auto device = **reinterpret_cast<Hedgehog::Mirage::CRenderingDevice***>(static_cast<uint8_t*>(A1) + 16);

    float sampleWeight[64];
    float sunParam[8];

    computeLightShaftSampleWeight(
        height, &sunParam[3], sampleWeight, *reinterpret_cast<float*>(0x1A431D4), *reinterpret_cast<float*>(0x1A431D8));

    device->m_pD3DDevice->SetPixelShaderConstantF(150, sampleWeight, 16);

    dataPtr = reinterpret_cast<uintptr_t>(A1) + cameraIndex * 496;

    const auto& sunPos = *reinterpret_cast<Hedgehog::Math::CVector4*>(0x1A57410);
    const auto& proj = *reinterpret_cast<Hedgehog::Math::CMatrix44*>(dataPtr + 48);
    const auto& view = *reinterpret_cast<Hedgehog::Math::CMatrix44*>(dataPtr + 432);

    Hedgehog::Math::CVector4 projectedPos = proj * (view * sunPos);

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
    sunParam[2] = static_cast<float>(width) / static_cast<float>(height);
    sunParam[4] = visibility <= 0.0f ? 1.0f + visibility : 1.0f;
    sunParam[5] = 0.0f;
    sunParam[6] = 0.0f;
    sunParam[7] = 0.0f;

    device->m_pD3DDevice->SetPixelShaderConstantF(166, sunParam, 2);

    screenDefaultExec(A1, A2, 0x352);
}

static Hedgehog::FxRenderFramework::SScreenRenderParam s_lightShaftFilterScreenRenderParam =
{
    "light shaft filter",
    (void*)&screenLightShaftFilter,
    0x352,
    {}
};

static Hedgehog::FxRenderFramework::SDrawInstanceParam s_lightShaftFilterScreenDrawInstanceParam0 =
{
    0,
    0,
    "LightShaftFilter0",
    0,
    (void*)0x651820,
    &s_lightShaftFilterScreenRenderParam,
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

static Hedgehog::FxRenderFramework::SDrawInstanceParam s_lightShaftFilterScreenDrawInstanceParam1 =
{
    0,
    0,
    "LightShaftFilter1",
    0,
    (void*)0x651820,
    &s_lightShaftFilterScreenRenderParam,
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

static Hedgehog::FxRenderFramework::SDrawInstanceParam s_lightShaftFilterScreenDrawInstanceParamEnd =
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

static Hedgehog::FxRenderFramework::SDrawInstanceParam s_lightShaftFilterDrawInstanceChildParams[] =
{
    s_lightShaftFilterScreenDrawInstanceParam0,
    s_lightShaftFilterScreenDrawInstanceParam1,
    s_lightShaftFilterScreenDrawInstanceParam0,
    s_lightShaftFilterScreenDrawInstanceParam1,
    s_lightShaftFilterScreenDrawInstanceParam0,
    s_lightShaftFilterScreenDrawInstanceParamEnd
};

static Hedgehog::FxRenderFramework::SDrawInstanceParam s_lightShaftFilterDrawInstanceParam =
{
    0,
    0,
    "+Traverse - s_render_lightshaft_filter",
    0,
    (void*)0x651820,
    s_lightShaftFilterDrawInstanceChildParams,
    _countof(s_lightShaftFilterDrawInstanceChildParams),
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

static void SceneTraversed_ScreenLightShaftMask(void* A1, void* A2)
{
    const auto device = **reinterpret_cast<Hedgehog::Mirage::CRenderingDevice***>(static_cast<char*>(A1) + 16);

    const float maskZ = *reinterpret_cast<float*>(0x1A431E0);
    const float maskPow = *reinterpret_cast<float*>(0x1A431E4);

    float skyMaskParam[] = { maskPow, maskPow, maskPow, maskZ };
    device->m_pD3DDevice->SetPixelShaderConstantF(166, skyMaskParam, 1);

    screenDefaultExec(A1, A2, 0x351);
}

static Hedgehog::FxRenderFramework::SScreenRenderParam s_lightShaftMaskScreenRenderParam =
{
    "light shaft mask",
    (void*)&SceneTraversed_ScreenLightShaftMask,
    0x351,
    {}
};

static Hedgehog::FxRenderFramework::SDrawInstanceParam s_lightShaftMaskScreenDrawInstanceParam =
{
    0,
    0,
    "LightShaftMask",
    0,
    (void*)0x651820,
    &s_lightShaftMaskScreenRenderParam,
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

static Hedgehog::FxRenderFramework::SDrawInstanceParam s_lightShaftMaskDrawInstanceParam =
{
    0,
    0,
    "+Traverse - s_render_lightshaft_mask",
    0,
    (void*)0x651820,
    &s_lightShaftMaskScreenDrawInstanceParam,
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

static Hedgehog::FxRenderFramework::SDrawInstanceParam s_lightShaftDrawInstanceChildParams[] =
{
    s_lightShaftMaskDrawInstanceParam,
    s_lightShaftFilterDrawInstanceParam
};

static Hedgehog::FxRenderFramework::SDrawInstanceParam s_lightShaftDrawInstanceParam =
{
    0,
    0,
    "+Traverse - s_render_lightshaft",
    0,
    (void*)0x651820,
    s_lightShaftDrawInstanceChildParams,
    _countof(s_lightShaftDrawInstanceChildParams),
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

static uint32_t* s_finalPostProcessParamChildCount = reinterpret_cast<uint32_t*>(0x13DF430);
static bool* s_isLightShaftEnable = reinterpret_cast<bool*>(0x1E5E333);

HOOK(void, __fastcall, RenderSimpleExecutor, 0x654C70, void* This, void* _, void* A2, void* A3)
{
    *s_finalPostProcessParamChildCount = *s_isLightShaftEnable ? 3 : 2;
    originalRenderSimpleExecutor(This, _, A2, A3);
}

void LightShaftImpl::init()
{
    const auto finalPostProcessParam = reinterpret_cast<Hedgehog::FxRenderFramework::SDrawInstanceParam*>(0x13DF418);
    const auto childParams = static_cast<Hedgehog::FxRenderFramework::SDrawInstanceParam*>(finalPostProcessParam->m_ChildParams);
    const auto newChildParams = new Hedgehog::FxRenderFramework::SDrawInstanceParam[finalPostProcessParam->m_ChildParamCount + 1];

    for (size_t i = 0; i < finalPostProcessParam->m_ChildParamCount; i++)
        newChildParams[i] = childParams[i];

    newChildParams[finalPostProcessParam->m_ChildParamCount] = s_lightShaftDrawInstanceParam;

    WRITE_MEMORY(&finalPostProcessParam->m_ChildParams, void*, newChildParams);

    DWORD oldProtect;
    VirtualProtect(s_finalPostProcessParamChildCount, sizeof(uint32_t), PAGE_READWRITE, &oldProtect);

    INSTALL_HOOK(RenderSimpleExecutor);
}
