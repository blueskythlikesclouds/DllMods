#pragma once

#include <BlueBlur.h>

#include <Hedgehog/Base/Container/hhMap.h>
#include <Sonic/Tool/EditParam/ParamBase.h>

namespace Sonic
{
    class CParamTypeList : public CParamBase
    {
    public:
        static constexpr void* ms_pVfTable = (void*)0x016E5C74;

        class CMember
        {
        public:
            class FuncData : public Hedgehog::Base::CRefCountObject
            {
            public:
                INSERT_PADDING(0x4);
                hh::map<uint32_t, Hedgehog::Base::CSharedString> m_ValueMap;
                INSERT_PADDING(0x44);
                uint32_t* m_pValue;
                uint32_t m_DefaultValue;
                Hedgehog::Base::CSharedString m_Description;
                INSERT_PADDING(0xC);
            };

            FuncData* m_pFuncData;
            uint32_t m_Value;

            uint32_t m_DefaultValue;
            Hedgehog::Base::CSharedString m_DefaultValueName;
        };

        CMember* m_pMember;
    };

    ASSERT_OFFSETOF(CParamTypeList::CMember::FuncData, m_ValueMap, 0xC);
    ASSERT_OFFSETOF(CParamTypeList::CMember::FuncData, m_pValue, 0x58);
    ASSERT_OFFSETOF(CParamTypeList::CMember::FuncData, m_DefaultValue, 0x5C);
    ASSERT_OFFSETOF(CParamTypeList::CMember::FuncData, m_Description, 0x60);
    ASSERT_SIZEOF(CParamTypeList::CMember::FuncData, 0x70);

    ASSERT_OFFSETOF(CParamTypeList::CMember, m_pFuncData, 0x0);
    ASSERT_OFFSETOF(CParamTypeList::CMember, m_Value, 0x4);
    ASSERT_OFFSETOF(CParamTypeList::CMember, m_DefaultValue, 0x8);
    ASSERT_OFFSETOF(CParamTypeList::CMember, m_DefaultValueName, 0xC);
    ASSERT_SIZEOF(CParamTypeList::CMember, 0x10);

    ASSERT_OFFSETOF(CParamTypeList, m_pMember, 0x14);
    ASSERT_SIZEOF(CParamTypeList, 0x18);
}
