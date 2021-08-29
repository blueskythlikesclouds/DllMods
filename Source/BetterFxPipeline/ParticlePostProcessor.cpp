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
        void** renderSceneBaseChildren = (void**)0x13DDC9C;
        uint32_t* renderSceneBaseChildCount = (uint32_t*)0x13DDCA0;

        const uint32_t renderSceneBaseChildrenByteSize =
            *renderSceneBaseChildCount * sizeof(hh::fx::SDrawInstanceParam);

        uint8_t* newChildren = (uint8_t*)operator new(renderSceneBaseChildrenByteSize + sizeof(hh::fx::SDrawInstanceParam));
        memcpy(newChildren, *renderSceneBaseChildren, renderSceneBaseChildrenByteSize);
        memcpy(newChildren + renderSceneBaseChildrenByteSize, (void*)0x13DD778, sizeof(hh::fx::SDrawInstanceParam));

        // Render only normal particles in Render Scene Base
        *(uint32_t*)(newChildren + renderSceneBaseChildrenByteSize + 0x14) += 2 * sizeof(hh::fx::SDrawInstanceParam);
        *(uint32_t*)(newChildren + renderSceneBaseChildrenByteSize + 0x18) = 1;

        WRITE_MEMORY(renderSceneBaseChildren, void*, newChildren);
        WRITE_MEMORY(renderSceneBaseChildCount, uint32_t, *renderSceneBaseChildCount + 1);

        // Render only framebuffer/stencil particles in Render Particle
        WRITE_MEMORY(0x13DD790, uint32_t, 2);
    }
}
