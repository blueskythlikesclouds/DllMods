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
        // Append Render Particle's last child to Render Scene Base
        hh::fx::SDrawInstanceParam* renderSceneBase = (hh::fx::SDrawInstanceParam*)0x13DDC88;

        hh::fx::SDrawInstanceParam* newChildren = new hh::fx::SDrawInstanceParam[renderSceneBase->m_ChildParamCount + 1];
        memcpy(newChildren, renderSceneBase->m_ChildParams, renderSceneBase->m_ChildParamCount * sizeof(hh::fx::SDrawInstanceParam));
        memcpy(&newChildren[renderSceneBase->m_ChildParamCount], (void*)0x13DC8C8, sizeof(hh::fx::SDrawInstanceParam));

        // Pass new children
        WRITE_MEMORY(&renderSceneBase->m_ChildParams, void*, newChildren);
        WRITE_MEMORY(&renderSceneBase->m_ChildParamCount, uint32_t, renderSceneBase->m_ChildParamCount + 1);

        // Render only framebuffer/stencil particles in Render Particle
        WRITE_MEMORY(0x13DD790, uint32_t, 2);
    }
}
