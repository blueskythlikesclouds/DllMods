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
    enum ERenderType : uint32_t
    {
        eRenderType_Terrain            = 0x2,
        eRenderType_Object             = 0x4,
        eRenderType_ObjectXlu          = 0x8,
        eRenderType_ObjectPreZPass     = 0x10,
        eRenderType_ObjectZPass        = 0x20,
        eRenderType_ObjectZPassConst   = 0x200000,
        eRenderType_ObjectOverlayZPass = 0x40,
        eRenderType_ObjectOverlay      = 0x80,
        eRenderType_ObjectIcon         = 0x100,
        eRenderType_Player             = 0x1,
        eRenderType_SparkleFramebuffer = 0x80000,
        eRenderType_SparkleObject      = 0x40000,
        eRenderType_SparkleStencil     = 0x100000,
        eRenderType_Effect             = 0x400,
        eRenderType_ShadowMapObject    = 0x4000,
        eRenderType_ShadowMapTerrain   = 0x8000,
        eRenderType_MotionBlurPlayer   = 0x10000,
        eRenderType_MotionBlurObject   = 0x20000,
        eRenderType_Debug              = 0x800,
        eRenderType_HUD                = 0x1000,
        eRenderType_SystemHUD          = 0x2000,
        eRenderType_Sky                = 0x200
    };

    enum ERenderSlot : uint32_t
    {
        eRenderSlot_Opaque       = 0x1,
        eRenderSlot_PunchThrough = 0x2,
        eRenderSlot_Transparent  = 0x4,
        eRenderSlot_Water        = 0x8,
        eRenderSlot_Special1     = 0x10,
        eRenderSlot_Special2     = 0x20,
        eRenderSlot_Main         = 0x40,
        eRenderSlot_Debug        = 0x80,
        eRenderSlot_Primitive    = 0x100,
        eRenderSlot_Resident     = 0x200
    };

    class CYggSceneRenderer;
    class CYggScheduler;

    static FUNCTION_PTR(void, __thiscall, fpCYggSceneRendererRender, 0x789890, CYggSceneRenderer* This, ERenderType type, ERenderSlot slot);
    static FUNCTION_PTR(void, __thiscall, fpCYggSceneRendererResetMaterialColor, 0x10D4EB0, CYggSceneRenderer* This);
    static FUNCTION_PTR(void, __thiscall, fpCYggSceneRendererSetLightViewProjection, 0x789360, CYggSceneRenderer* This, const Eigen::Matrix4f& view, const Eigen::Matrix4f& projection);
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

        virtual void _4() = 0;
        virtual void _8() = 0;

        void Render(ERenderType type, ERenderSlot slot)
        {
            fpCYggSceneRendererRender(this, type, slot);
        }

        void Render(uint32_t type, uint32_t slot)
        {
            fpCYggSceneRendererRender(this, (ERenderType)type, (ERenderSlot)slot);
        }

        void ResetMaterialColor()
        {
            fpCYggSceneRendererResetMaterialColor(this);
        }

        void SetLightViewProjection(const Eigen::Matrix4f& view, const Eigen::Matrix4f& projection)
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