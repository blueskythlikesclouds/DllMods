#pragma once

#include <BlueBlur.h>

#include <Hedgehog/Yggdrasill/hhYggMisc.h>
#include <Hedgehog/Yggdrasill/hhYggSceneRenderer.h>
#include <Hedgehog/Yggdrasill/hhYggScheduler.h>

#include <Sonic/FxPipeline/Jobs/FxJob.h>

namespace Sonic
{
    class CFxRenderScene : public CFxJob
    {
    public:
        Hedgehog::Yggdrasill::ERenderCategory m_RenderCategories;
        Hedgehog::Yggdrasill::ERenderLevel m_RenderLevels;
        INSERT_PADDING(0x20);

        void RenderScene(Hedgehog::Yggdrasill::ERenderCategory category, Hedgehog::Yggdrasill::ERenderLevel level)
        {
            m_pScheduler->m_pMisc->m_spSceneRenderer->Render(category & m_RenderCategories, level & m_RenderLevels);
        }

        void RenderScene(uint32_t category, uint32_t level)
        {
            m_pScheduler->m_pMisc->m_spSceneRenderer->Render(category & m_RenderCategories, level & m_RenderLevels);
        }
    };

    ASSERT_OFFSETOF(CFxRenderScene, m_RenderCategories, 0x48);
    ASSERT_OFFSETOF(CFxRenderScene, m_RenderLevels, 0x4C);
    ASSERT_SIZEOF(CFxRenderScene, 0x70);
}
