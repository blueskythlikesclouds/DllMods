#pragma once

#include <BlueBlur.h>

#include <Hedgehog/Base/System/hhSymbol.h>
#include <Hedgehog/Base/hhObject.h>

namespace Hedgehog::Mirage
{
    class CCamera;
    class CLightManager;
    class CRenderingDevice;
    class CRenderingInfrastructure;
    struct SShaderPair;
}

namespace Hedgehog::Yggdrasill
{
    enum ERenderCategory : uint32_t
    {
        eRenderCategory_Terrain            = 0x2,
        eRenderCategory_Object             = 0x4,
        eRenderCategory_ObjectXlu          = 0x8,
        eRenderCategory_ObjectPreZPass     = 0x10,
        eRenderCategory_ObjectZPass        = 0x20,
        eRenderCategory_ObjectZPassConst   = 0x200000,
        eRenderCategory_ObjectOverlayZPass = 0x40,
        eRenderCategory_ObjectOverlay      = 0x80,
        eRenderCategory_ObjectIcon         = 0x100,
        eRenderCategory_Player             = 0x1,
        eRenderCategory_SparkleFramebuffer = 0x80000,
        eRenderCategory_SparkleObject      = 0x40000,
        eRenderCategory_SparkleStencil     = 0x100000,
        eRenderCategory_Effect             = 0x400,
        eRenderCategory_ShadowMapObject    = 0x4000,
        eRenderCategory_ShadowMapTerrain   = 0x8000,
        eRenderCategory_MotionBlurPlayer   = 0x10000,
        eRenderCategory_MotionBlurObject   = 0x20000,
        eRenderCategory_Debug              = 0x800,
        eRenderCategory_HUD                = 0x1000,
        eRenderCategory_SystemHUD          = 0x2000,
        eRenderCategory_Sky                = 0x200
    };

    enum ERenderLevel : uint32_t
    {
        eRenderLevel_Opaque       = 0x1,
        eRenderLevel_PunchThrough = 0x2,
        eRenderLevel_Transparent  = 0x4,
        eRenderLevel_Water        = 0x8,
        eRenderLevel_Special1     = 0x10,
        eRenderLevel_Special2     = 0x20,
        eRenderLevel_Main         = 0x40,
        eRenderLevel_Debug        = 0x80,
        eRenderLevel_Primitive    = 0x100,
        eRenderLevel_Resident     = 0x200
    };

    class CYggSceneRenderer;
    class CYggScheduler;

    static FUNCTION_PTR(void, __thiscall, fpCYggSceneRendererRender, 0x789890, CYggSceneRenderer* This, ERenderCategory type, ERenderLevel slot);
    static FUNCTION_PTR(void, __thiscall, fpCYggSceneRendererResetMaterialColor, 0x10D4EB0, CYggSceneRenderer* This);
    static FUNCTION_PTR(void, __thiscall, fpCYggSceneRendererSetLightViewProjection, 0x789360, CYggSceneRenderer* This, const Math::CMatrix& view, const Math::CMatrix44& projection);
    static FUNCTION_PTR(void, __thiscall, fpCYggSceneRendererLockShader, 0x789BE0, CYggSceneRenderer* This, const Hedgehog::Mirage::SShaderPair& shaderPair, size_t flags);
    static FUNCTION_PTR(void, __thiscall, fpCYggSceneRendererUnlockShader, 0x789860, CYggSceneRenderer* This);
    static FUNCTION_PTR(void, __thiscall, fpCYggSceneRendererSetCamera, 0x7890F0, CYggSceneRenderer* This, Hedgehog::Mirage::CCamera* pCamera, size_t flags);
    static FUNCTION_PTR(void, __thiscall, fpCYggSceneRendererUnsetCamera, 0x789110, CYggSceneRenderer* This);

    class CYggSceneRenderer : public Base::CObject
    {
    public:
        INSERT_PADDING(0xC);
        CYggScheduler* m_pScheduler;
        Mirage::CRenderingDevice* m_pRenderingDevice;
        Mirage::CRenderingInfrastructure* m_pRenderingInfrastructure;
        Mirage::CCamera* m_pCamera;
        Base::CStringSymbol m_SymbolDefault;
        INSERT_PADDING(0xC);
        Mirage::CLightManager* m_pLightManager;
        
        INSERT_PADDING(0x7C);

        virtual ~CYggSceneRenderer() = default;
        virtual void _4() = 0;
        virtual void _8() = 0;

        void Render(ERenderCategory type, ERenderLevel slot)
        {
            fpCYggSceneRendererRender(this, type, slot);
        }

        void Render(uint32_t type, uint32_t slot)
        {
            fpCYggSceneRendererRender(this, (ERenderCategory)type, (ERenderLevel)slot);
        }

        void ResetMaterialColor()
        {
            fpCYggSceneRendererResetMaterialColor(this);
        }

        void SetLightViewProjection(const Math::CMatrix& view, const Math::CMatrix44& projection)
        {
            fpCYggSceneRendererSetLightViewProjection(this, view, projection);
        }

        void LockShader(const Hedgehog::Mirage::SShaderPair& shaderPair, const size_t flags)
        {
            fpCYggSceneRendererLockShader(this, shaderPair, flags);
        }

        void UnlockShader()
        {
            fpCYggSceneRendererUnlockShader(this);
        }

        void SetCamera(Hedgehog::Mirage::CCamera* pCamera, const size_t flags)
        {
            fpCYggSceneRendererSetCamera(this, pCamera, flags);
        }

        void UnsetCamera()
        {
            fpCYggSceneRendererUnsetCamera(this);
        }
    };

    ASSERT_OFFSETOF(CYggSceneRenderer, m_pScheduler, 0x10);
    ASSERT_OFFSETOF(CYggSceneRenderer, m_pRenderingDevice, 0x14);
    ASSERT_OFFSETOF(CYggSceneRenderer, m_pRenderingInfrastructure, 0x18);
    ASSERT_OFFSETOF(CYggSceneRenderer, m_pCamera, 0x1C);
    ASSERT_OFFSETOF(CYggSceneRenderer, m_SymbolDefault, 0x20);
    ASSERT_OFFSETOF(CYggSceneRenderer, m_pLightManager, 0x30);
    ASSERT_SIZEOF(CYggSceneRenderer, 0xB0);
}