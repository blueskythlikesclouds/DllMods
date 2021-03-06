#pragma once

#include <BlueBlur.h>

#include <Sonic/System/RenderDirector/RenderDirector.h>
#include <Hedgehog/Universe/Engine/hhStateMachineMessageReceiver.h>

namespace Sonic
{
    class CFxScheduler;

    class CRenderDirectorFxPipeline : public CRenderDirector, public Hedgehog::Universe::IStateMachineMessageReceiver
    {
    public:
        CFxScheduler* m_pScheduler;
    };

    ASSERT_OFFSETOF(CRenderDirectorFxPipeline, m_pScheduler, 0xC4);
    ASSERT_SIZEOF(CRenderDirectorFxPipeline, 0xC8);
}
