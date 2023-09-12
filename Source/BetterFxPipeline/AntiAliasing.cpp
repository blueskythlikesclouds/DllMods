#include "AntiAliasing.h"

#include "Configuration.h"

void AntiAliasing::init()
{
    // Ignore Devil's Details' FXAA shader
    WRITE_NOP(0x64CC19, 2);

    if (Configuration::fxaaIntensity > FxaaIntensity::Disabled && Configuration::fxaaIntensity <= FxaaIntensity::Intensity6)
    {
        const auto newScreenRenderParam = new Hedgehog::FxRenderFramework::SScreenRenderParam();
        memcpy(newScreenRenderParam, reinterpret_cast<void*>(0x13DF5A8), sizeof(Hedgehog::FxRenderFramework::SScreenRenderParam));

        newScreenRenderParam->m_ShaderIndex = 0x350;

        // Insert our own Draw Instance Param to Render Before Particle 3
        const auto renderBeforeParticle3Param = reinterpret_cast<Hedgehog::FxRenderFramework::SDrawInstanceParam*>(0x13DDDC8);

        const auto newChildren = new Hedgehog::FxRenderFramework::SDrawInstanceParam[renderBeforeParticle3Param->m_ChildParamCount + 1];
        memcpy(newChildren, renderBeforeParticle3Param->m_ChildParams, 
            sizeof(Hedgehog::FxRenderFramework::SDrawInstanceParam) * renderBeforeParticle3Param->m_ChildParamCount);

        // Initialize FXAA parameters
        const auto fxaaParam = &newChildren[renderBeforeParticle3Param->m_ChildParamCount];
        memset(fxaaParam, 0, sizeof(Hedgehog::FxRenderFramework::SDrawInstanceParam));

        fxaaParam->m_pCallback = reinterpret_cast<void*>(0x651820);
        fxaaParam->m_ChildParams = newScreenRenderParam;
        fxaaParam->m_RenderTargetSurface = 10;
        fxaaParam->m_TemporaryRenderTargetSurface = 7;
        fxaaParam->m_S0Sampler = 0x80 | 10;
        fxaaParam->m_Unk0 = 0x3;
        fxaaParam->m_Unk2 = 0x101;

        // Pass new pointers
        WRITE_MEMORY(&renderBeforeParticle3Param->m_ChildParams, void*, newChildren);
        WRITE_MEMORY(&renderBeforeParticle3Param->m_ChildParamCount, uint32_t, renderBeforeParticle3Param->m_ChildParamCount + 1);
    }
}
