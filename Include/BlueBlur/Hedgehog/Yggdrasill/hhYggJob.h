#pragma once

#include <BlueBlur.h>
#include <Hedgehog/Base/hhObject.h>

namespace Hedgehog::Base
{
    class CStringSymbol;
}

namespace Hedgehog::Yggdrasill
{
    class CYggAbstractBuffer;
    class CYggJob;
    class CYggScheduler;
    class CYggSurface;
    class CYggTexture;

    static FUNCTION_PTR(void, __thiscall, fpCYggJobGetDefaultTexture, 0x784A60, CYggJob* This, boost::shared_ptr<CYggTexture>& spTexture);
    static FUNCTION_PTR(void, __thiscall, fpCYggJobGetTexture, 0x7848D0, CYggJob* This, boost::shared_ptr<CYggTexture>& spTexture, const Hedgehog::Base::CStringSymbol& symbol);
    static FUNCTION_PTR(void, __thiscall, fpCYggJobGetSurface, 0x7847B0, CYggJob* This, boost::shared_ptr<CYggSurface>& spSurface, const Hedgehog::Base::CStringSymbol& symbol);

    static FUNCTION_PTR(void, __thiscall, fpCYggJobSetDefaultTexture, 0x784D00, CYggJob* This, const boost::shared_ptr<CYggTexture>& spTexture);
    static FUNCTION_PTR(void, __thiscall, fpCYggJobSetBuffer, 0x784BF0, CYggJob* This, const Hedgehog::Base::CStringSymbol& symbol, const boost::shared_ptr<CYggAbstractBuffer>& spBuffer);

    class CYggJob : public Base::CObject
    {
    public:
        INSERT_PADDING(0x8);
        CYggScheduler* m_pScheduler;
        INSERT_PADDING(0x30);

        virtual void Initialize() = 0;
        virtual void Execute() = 0;

        void GetDefaultTexture(boost::shared_ptr<CYggTexture>& spTexture)
        {
            fpCYggJobGetDefaultTexture(this, spTexture);
        }

        void GetTexture(boost::shared_ptr<CYggTexture>& spTexture, const Base::CStringSymbol& symbol)
        {
            fpCYggJobGetTexture(this, spTexture, symbol);
        }      

        void GetSurface(boost::shared_ptr<CYggSurface>& spSurface, const Base::CStringSymbol& symbol)
        {
            fpCYggJobGetSurface(this, spSurface, symbol);
        }

        void SetDefaultTexture(const boost::shared_ptr<CYggTexture>& spTexture)
        {
            fpCYggJobSetDefaultTexture(this, spTexture);
        }

        void SetBuffer(const Base::CStringSymbol& symbol, const boost::shared_ptr<CYggAbstractBuffer>& spBuffer)
        {
            fpCYggJobSetBuffer(this, symbol, spBuffer);
        }      
    };

    ASSERT_OFFSETOF(CYggJob, m_pScheduler, 0xC);
    ASSERT_SIZEOF(CYggJob, 0x40);
}
