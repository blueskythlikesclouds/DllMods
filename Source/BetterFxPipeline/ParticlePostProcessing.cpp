#include "ParticlePostProcessing.h"

#include "Configuration.h"

void ParticlePostProcessing::init()
{
    if (Configuration::postProcessingOnParticles)
    {
        // Append Render Particle's last child to Render Scene Base
        const auto renderSceneBase = reinterpret_cast<Hedgehog::FxRenderFramework::SDrawInstanceParam*>(0x13DDC88);

        const auto newChildren = new Hedgehog::FxRenderFramework::SDrawInstanceParam[renderSceneBase->m_ChildParamCount + 1];
        memcpy(newChildren, renderSceneBase->m_ChildParams, renderSceneBase->m_ChildParamCount * sizeof(Hedgehog::FxRenderFramework::SDrawInstanceParam));
        memcpy(&newChildren[renderSceneBase->m_ChildParamCount], reinterpret_cast<void*>(0x13DC8C8), sizeof(Hedgehog::FxRenderFramework::SDrawInstanceParam));

        // Pass new children
        WRITE_MEMORY(&renderSceneBase->m_ChildParams, void*, newChildren);
        WRITE_MEMORY(&renderSceneBase->m_ChildParamCount, uint32_t, renderSceneBase->m_ChildParamCount + 1);

        // Render only framebuffer/stencil particles in Render Particle
        WRITE_MEMORY(0x13DD790, uint32_t, 2);
    }
}
