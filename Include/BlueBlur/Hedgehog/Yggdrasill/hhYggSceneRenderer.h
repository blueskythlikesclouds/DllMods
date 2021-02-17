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
        HH_YGG_RENDER_TYPE_TERRAIN              = 0x2,
        HH_YGG_RENDER_TYPE_OBJECT               = 0x4,
        HH_YGG_RENDER_TYPE_OBJECT_XLU           = 0x8,
        HH_YGG_RENDER_TYPE_OBJECT_PRE_ZPASS     = 0x10,
        HH_YGG_RENDER_TYPE_OBJECT_ZPASS         = 0x20,
        HH_YGG_RENDER_TYPE_OBJECT_ZPASS_CONST   = 0x200000,
        HH_YGG_RENDER_TYPE_OBJECT_OVERLAY_ZPASS = 0x40,
        HH_YGG_RENDER_TYPE_OBJECT_OVERLAY       = 0x80,
        HH_YGG_RENDER_TYPE_OBJECT_ICON          = 0x100,
        HH_YGG_RENDER_TYPE_PLAYER               = 0x1,
        HH_YGG_RENDER_TYPE_SPARKLE_FRAMEBUFFER  = 0x80000,
        HH_YGG_RENDER_TYPE_SPARKLE_OBJECT       = 0x40000,
        HH_YGG_RENDER_TYPE_SPARKLE_STENCIL      = 0x100000,
        HH_YGG_RENDER_TYPE_EFFECT               = 0x400,
        HH_YGG_RENDER_TYPE_SHADOW_MAP_OBJECT    = 0x4000,
        HH_YGG_RENDER_TYPE_SHADOW_MAP_TERRAIN   = 0x8000,
        HH_YGG_RENDER_TYPE_MOTION_BLUR_PLAYER   = 0x10000,
        HH_YGG_RENDER_TYPE_MOTION_BLUR_OBJECT   = 0x20000,
        HH_YGG_RENDER_TYPE_DEBUG                = 0x800,
        HH_YGG_RENDER_TYPE_HUD                  = 0x1000,
        HH_YGG_RENDER_TYPE_SYSTEM_HUD           = 0x2000,
        HH_YGG_RENDER_TYPE_SKY                  = 0x200
    };

    enum ERenderSlot : uint32_t
    {
        HH_YGG_RENDER_SLOT_OPAQUE        = 0x1,
        HH_YGG_RENDER_SLOT_PUNCH_THROUGH = 0x2,
        HH_YGG_RENDER_SLOT_TRANSPARENT   = 0x4,
        HH_YGG_RENDER_SLOT_WATER         = 0x8,
        HH_YGG_RENDER_SLOT_SPECIAL1      = 0x10,
        HH_YGG_RENDER_SLOT_SPECIAL2      = 0x20,
        HH_YGG_RENDER_SLOT_MAIN          = 0x40,
        HH_YGG_RENDER_SLOT_DEBUG         = 0x80,
        HH_YGG_RENDER_SLOT_PRIMITIVE     = 0x100,
        HH_YGG_RENDER_SLOT_RESIDENT      = 0x200
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