#include "LoadingScreenFixer.h"

FUNCTION_PTR(void, __thiscall, CRenderDirectorFxPipelineRender, 0xD1D570, void* This, void* pRenderFramework, void* pRenderData);

HOOK(void, __fastcall, RenderResident, 0x64C900, void* This, void* Edx, void* pRenderFramework, void* pRenderData, void* A3, void* A4)
{
    originalRenderResident(This, Edx, pRenderFramework, pRenderData, A3, A4);
    CRenderDirectorFxPipelineRender((*Sonic::CGameDocument::ms_pInstance)->m_pMember->m_spRenderDirector.get(), pRenderFramework, pRenderData);
}

bool LoadingScreenFixer::enabled = false;

void LoadingScreenFixer::applyPatches()
{
    if (enabled)
        return;

    enabled = true;

    // FxPipeline tries to render the loading screen 
    // with MTFx's render director when the thread is busy.
    // Let's not do that.
    INSTALL_HOOK(RenderResident);
    WRITE_MEMORY(0x64C6D0, uint8_t, 0xC2, 0x0C, 0x00);
}
