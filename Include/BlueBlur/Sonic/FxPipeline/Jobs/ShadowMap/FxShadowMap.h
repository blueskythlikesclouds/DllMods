#pragma once

#include <BlueBlur.h>
#include <Hedgehog/Base/Container/hhVector.h>
#include <Sonic/FxPipeline/Jobs/FxJob.h>

namespace Hedgehog::Mirage
{
    class CCamera;
}

namespace Hedgehog::Yggdrasill
{
    class CYggDevice;
    class CYggSurface;
    class CYggTexture;
}

namespace Sonic
{
    class CFxShadowMap;

    static FUNCTION_PTR(void, __thiscall, fpCFxShadowMapInitialize, 0x10C6CE0, CFxShadowMap* This);
    static FUNCTION_PTR(void, __thiscall, fpCFxShadowMapExecute, 0x10C68D0, CFxShadowMap* This);
    static FUNCTION_PTR(void, __stdcall, fpCFxShadowMapInitializeCameras, 0x10C6A10, CFxShadowMap* This);

    static void* const pCFxShadowMapSetRenderStates = (void*)0x10C61D0;
    static void* const pCFxShadowMapUnsetRenderStates = (void*)0x10C6160;

    static void fCFxShadowMapSetRenderStates(Hedgehog::Yggdrasill::CYggDevice* pDevice, uint32_t enableReverseCull)
    {
        __asm
        {
            mov eax, pDevice
            push enableReverseCull
            call[pCFxShadowMapSetRenderStates]
            add esp, 4
        }
    }

    static void fCFxShadowMapUnsetRenderStates(Hedgehog::Yggdrasill::CYggDevice* pDevice, uint32_t enableReverseCull)
    {
        __asm
        {
            mov eax, pDevice
            push enableReverseCull
            call[pCFxShadowMapUnsetRenderStates]
            add esp, 4
        }
    }

    class CFxShadowMap : public CFxJob
    {
    public:
        boost::shared_ptr<Hedgehog::Yggdrasill::CYggSurface> m_spColorSurface;
        boost::shared_ptr<Hedgehog::Yggdrasill::CYggTexture> m_spShadowMapTex;
        boost::shared_ptr<Hedgehog::Yggdrasill::CYggTexture> m_spShadowMapNoTerrainTex;
        Hedgehog::Mirage::SShaderPair m_MakeShadowMapShader;
        Hedgehog::Mirage::SShaderPair m_MakeShadowMapTransparentShader;
        Hedgehog::Mirage::CCamera* m_pCurrentShadowMapCamera;
        Hedgehog::vector<boost::shared_ptr<Hedgehog::Mirage::CCamera>> m_ShadowMapCameras;
        boost::shared_ptr<Hedgehog::Mirage::CCamera> m_spDirectionalShadowMapCamera;
        boost::shared_ptr<Hedgehog::Mirage::CCamera> m_spVerticalShadowMapCamera;
        uint32_t m_ShadowMapSize;
        INSERT_PADDING(0x4);

        void InitializeCameras()
        {
            fpCFxShadowMapInitializeCameras(this);
        }

        void SetRenderStates(const bool enableReverseCull)
        {
            fCFxShadowMapSetRenderStates(m_pScheduler->m_pMisc->m_pDevice, enableReverseCull);
        }

        void UnsetRenderStates(const bool enableReverseCull)
        {
            fCFxShadowMapUnsetRenderStates(m_pScheduler->m_pMisc->m_pDevice, enableReverseCull);
        }
    };

    ASSERT_OFFSETOF(CFxShadowMap, m_spColorSurface, 0x48);
    ASSERT_OFFSETOF(CFxShadowMap, m_spShadowMapTex, 0x50);
    ASSERT_OFFSETOF(CFxShadowMap, m_spShadowMapNoTerrainTex, 0x58);
    ASSERT_OFFSETOF(CFxShadowMap, m_MakeShadowMapShader, 0x60);
    ASSERT_OFFSETOF(CFxShadowMap, m_MakeShadowMapTransparentShader, 0x70);
    ASSERT_OFFSETOF(CFxShadowMap, m_pCurrentShadowMapCamera, 0x80);
    ASSERT_OFFSETOF(CFxShadowMap, m_ShadowMapCameras, 0x84);
    ASSERT_OFFSETOF(CFxShadowMap, m_spDirectionalShadowMapCamera, 0x94);
    ASSERT_OFFSETOF(CFxShadowMap, m_spVerticalShadowMapCamera, 0x9C);
    ASSERT_OFFSETOF(CFxShadowMap, m_ShadowMapSize, 0xA4);
    ASSERT_SIZEOF(CFxShadowMap, 0xAC);
}