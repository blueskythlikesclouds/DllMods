#pragma once

#include <BlueBlur.h>

#include <Hedgehog/Base/Type/hhSharedString.h>
#include <Sonic/Tool/ParameterEditor/AbstractParameter.h>

namespace Sonic
{
    class CParameterFile;
    class CParameterGroup;

    static FUNCTION_PTR(void, __stdcall, fpCParameterFileCreateParameterGroup, 0x1257EF0,
        CParameterFile* pParameterFile, boost::shared_ptr<CParameterGroup>& spParameterGroup, const Hedgehog::Base::CSharedString& name, const Hedgehog::Base::CSharedString& description);

    class CParameterFile : public CAbstractParameter
    {
    public:
        static constexpr void* ms_pVfTable = (void*)0x16E5E88;

        void CreateParameterGroup(boost::shared_ptr<CParameterGroup>& spParameterGroup, const Hedgehog::Base::CSharedString& name, const Hedgehog::Base::CSharedString& description)
        {
            fpCParameterFileCreateParameterGroup(this, spParameterGroup, name, description);
        }
    };

    ASSERT_SIZEOF(CParameterFile, 0x34);
}