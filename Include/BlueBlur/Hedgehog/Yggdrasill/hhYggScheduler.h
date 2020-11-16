#pragma once

#include <BlueBlur.h>
#include <Hedgehog/Base/hhObject.h>

namespace Hedgehog::Mirage
{
    class CPixelShaderData;
    class CVertexShaderData;

    struct SShaderPair
    {
        boost::shared_ptr<CVertexShaderData> m_spVertexShader;
        boost::shared_ptr<CPixelShaderData> m_spPixelShader;
    };
}

namespace Hedgehog::Yggdrasill
{
    class CYggMisc;
    class CYggPicture;
    class CYggScheduler;

    static FUNCTION_PTR(void*, __thiscall, fpCYggSchedulerGetShader, 0x789DB0,
        CYggScheduler* This, Hedgehog::Mirage::SShaderPair& pair, const char* pVertexShaderName, const char* pPixelShaderName);

    static FUNCTION_PTR(void*, __thiscall, fpCYggSchedulerGetPicture, 0x789DD0,
        CYggScheduler* This, boost::shared_ptr<CYggPicture>& spPicture, const char* name);


    class CYggScheduler : public Base::CObject
    {
    public:
        CYggMisc* m_pMisc;
        INSERT_PADDING(0x4);

        virtual void _4() = 0;
        virtual void _8() = 0;
        virtual void _C() = 0;
        virtual void _10() = 0;

        void GetShader(Mirage::SShaderPair& pair, const char* pVertexShaderName, const char* pPixelShaderName)
        {
            fpCYggSchedulerGetShader(this, pair, pVertexShaderName, pPixelShaderName);
        }

        void GetPicture(boost::shared_ptr<CYggPicture>& spPicture, const char* name)
        {
            fpCYggSchedulerGetPicture(this, spPicture, name);
        }
    };

    ASSERT_OFFSETOF(CYggScheduler, m_pMisc, 0x4);
    ASSERT_SIZEOF(CYggScheduler, 0xC);
}
