#include "ParticlePostProcessor.h"

bool ParticlePostProcessor::enabled = false;

void ParticlePostProcessor::applyPatches()
{
    if (enabled)
        return;

    enabled = true;

    //
    // FxPipeline
    //
    {
        // Don't render anything in FxRenderParticle
        WRITE_MEMORY(0x10C8273, uint8_t, 0x83, 0xC4, 0x08, 0x90, 0x90);

        // Render particles in FxRenderGameScene
        WRITE_MEMORY(0x10CAD02, uint8_t, 0x1C);

        // TODO: Fix distortion particles since they don't take water into account. Makes Aquarium Park look wrong.
    }

    //
    // MTFx
    //
    {
        // Expand GameScene's Render Scene Base to include Render Particle
        hh::fx::SDrawInstanceParam* renderSceneBase = (hh::fx::SDrawInstanceParam*)0x13DDC88;
        hh::fx::SDrawInstanceParam* renderParticle = (hh::fx::SDrawInstanceParam*)0x13DD778;

        hh::fx::SDrawInstanceParam* newChildren = new hh::fx::SDrawInstanceParam[renderSceneBase->m_ChildParamCount + 1];
        memcpy(newChildren, renderSceneBase->m_ChildParams, renderSceneBase->m_ChildParamCount * sizeof(hh::fx::SDrawInstanceParam));

        hh::fx::SDrawInstanceParam* newChild = &newChildren[renderSceneBase->m_ChildParamCount];

        *newChild = *renderParticle;
        newChild->m_DepthStencilSurface = renderSceneBase->m_DepthStencilSurface;
        newChild->m_RenderTargetSurface = renderSceneBase->m_RenderTargetSurface;
        newChild->m_MsaaDepthStencilSurface = renderSceneBase->m_MsaaDepthStencilSurface;
        newChild->m_MsaaRenderTargetSurface = renderSceneBase->m_MsaaRenderTargetSurface;
        newChild->m_ReflectionMap2Sampler = renderSceneBase->m_ReflectionMap2Sampler;
        newChild->m_ReflectionMapSampler = renderSceneBase->m_ReflectionMapSampler;
        newChild->m_VerticalShadowMapSampler = renderSceneBase->m_VerticalShadowMapSampler;
        newChild->m_ShadowMapSampler = renderSceneBase->m_ShadowMapSampler;

        // Render only normal particles in Render Scene Base
        newChild->m_ChildParams = (char*)newChild->m_ChildParams + 2 * sizeof(hh::fx::SDrawInstanceParam);
        newChild->m_ChildParamCount = 1;

        // Pass new children
        WRITE_MEMORY(&renderSceneBase->m_ChildParams, void*, newChildren);
        WRITE_MEMORY(&renderSceneBase->m_ChildParamCount, uint32_t, renderSceneBase->m_ChildParamCount + 1);

        // Render only framebuffer/stencil particles in Render Particle
        WRITE_MEMORY(&renderParticle->m_ChildParamCount, uint32_t, 2);
    }
}
