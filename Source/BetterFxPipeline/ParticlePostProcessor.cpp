#include "ParticlePostProcessor.h"

bool ParticlePostProcessor::enabled = false;

void ParticlePostProcessor::applyPatches()
{
    if (enabled)
        return;

    enabled = true;

    // Don't render anything in FxRenderParticle
    WRITE_MEMORY(0x10C8273, uint8_t, 0x83, 0xC4, 0x08, 0x90, 0x90);

    // Render particles in FxRenderGameScene
    WRITE_MEMORY(0x10CAD02, uint8_t, 0x1C);

    // TODO: Fix distortion particles since they don't take water into account. Makes Aquarium Park look wrong.
}
