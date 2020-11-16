#pragma once

#include <BlueBlur.h>

#include <Hedgehog/Yggdrasill/hhYggSceneRenderer.h>
#include <Sonic/FxPipeline/Jobs/FxJob.h>

namespace Sonic
{
    class CFxRenderScene : public CFxJob
    {
    public:
        Hedgehog::Yggdrasill::ERenderType m_RenderTypes;
        Hedgehog::Yggdrasill::ERenderSlot m_RenderSlots;
        INSERT_PADDING(0x20);

        void RenderScene(Hedgehog::Yggdrasill::ERenderType type, Hedgehog::Yggdrasill::ERenderSlot slot)
        {
            m_pScheduler->m_pMisc->m_spSceneRenderer->Render(type & m_RenderTypes, slot & m_RenderSlots);
        }

        void RenderScene(uint32_t type, uint32_t slot)
        {
            m_pScheduler->m_pMisc->m_spSceneRenderer->Render(type & m_RenderTypes, slot & m_RenderSlots);
        }
    };

    ASSERT_OFFSETOF(CFxRenderScene, m_RenderTypes, 0x48);
    ASSERT_OFFSETOF(CFxRenderScene, m_RenderSlots, 0x4C);
    ASSERT_SIZEOF(CFxRenderScene, 0x70);
}
