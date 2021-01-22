#pragma once

#include <BlueBlur.h>

namespace Hedgehog::Base
{
    class CStringSymbol;
}

namespace Hedgehog::Mirage
{
    class CPixelShaderData;

    class CRenderingDevice;
    class CRenderingInfrastructure;

    static FUNCTION_PTR(void*, __thiscall, fpCRenderingDeviceSetRenderState, 0x40BB20,
        CRenderingDevice* This, D3DRENDERSTATETYPE type, uint32_t value);

    static FUNCTION_PTR(void*, __thiscall, fpCRenderingDevicePushRenderState, 0x6F7FD0,
        CRenderingDevice* This, D3DRENDERSTATETYPE type, uint32_t value);

    static FUNCTION_PTR(void*, __thiscall, fpCRenderingDevicePopRenderState, 0x6F8040,
        CRenderingDevice* This, D3DRENDERSTATETYPE type);

    static FUNCTION_PTR(void*, __thiscall, fpCRenderingDeviceLockRenderState, 0x6F7C60,
        CRenderingDevice* This, D3DRENDERSTATETYPE type);

    static FUNCTION_PTR(void*, __thiscall, fpCRenderingDeviceUnlockRenderState, 0x6F7C90,
        CRenderingDevice* This, D3DRENDERSTATETYPE type);

    static FUNCTION_PTR(void*, __thiscall, fpCRenderingDeviceSetPixelShaderParameterF, 0x411BF0,
        CRenderingDevice* This, const Hedgehog::Base::CStringSymbol& symbol, const float* data, uint32_t count, uint32_t offset, const boost::shared_ptr<CPixelShaderData>& spPixelShaderData);

    static FUNCTION_PTR(void, __thiscall, fpCRenderingDeviceSetViewMatrix, 0x6FA760,
        CRenderingDevice* This, const Eigen::Matrix4f& viewMatrix);

    static FUNCTION_PTR(void, __thiscall, fpCRenderingDeviceSetAtlasParameterData, 0x6FA080,
        CRenderingDevice* This, float* const pData);

    class CRenderingDevice
    {
    public:
        CRenderingInfrastructure* m_pRenderingInfrastructure;
        DX_PATCH::IDirect3DDevice9* m_pD3DDevice;

        void* m_pGlobalParametersData;
        uint32_t m_GlobalParameterIndex;

        INSERT_PADDING(0x75E0);

        void SetRenderState(D3DRENDERSTATETYPE type, uint32_t value)
        {
            fpCRenderingDeviceSetRenderState(this, type, value);
        }

        void PushRenderState(D3DRENDERSTATETYPE type, uint32_t value)
        {
            fpCRenderingDevicePushRenderState(this, type, value);
        }

        void PopRenderState(D3DRENDERSTATETYPE type)
        {
            fpCRenderingDevicePopRenderState(this, type);
        }

        void LockRenderState(D3DRENDERSTATETYPE type)
        {
            fpCRenderingDeviceLockRenderState(this, type);
        }

        void UnlockRenderState(D3DRENDERSTATETYPE type)
        {
            fpCRenderingDeviceUnlockRenderState(this, type);
        }

        void SetPixelShaderParameterF(const Hedgehog::Base::CStringSymbol& symbol, const float* data, uint32_t count, uint32_t offset, const boost::shared_ptr<CPixelShaderData>& spPixelShaderData)
        {
            fpCRenderingDeviceSetPixelShaderParameterF(this, symbol, data, count, offset, spPixelShaderData);
        }

        void SetViewMatrix(const Eigen::Matrix4f& viewMatrix)
        {
            fpCRenderingDeviceSetViewMatrix(this, viewMatrix);
        }

        void SetAtlasParameterData(float* const pData)
        {
            fpCRenderingDeviceSetAtlasParameterData(this, pData);
        }
    };

    ASSERT_OFFSETOF(CRenderingDevice, m_pRenderingInfrastructure, 0x0);
    ASSERT_OFFSETOF(CRenderingDevice, m_pD3DDevice, 0x4);
    ASSERT_OFFSETOF(CRenderingDevice, m_pGlobalParametersData, 0x8);
    ASSERT_OFFSETOF(CRenderingDevice, m_GlobalParameterIndex, 0xC);
    ASSERT_SIZEOF(CRenderingDevice, 0x75F0);
}