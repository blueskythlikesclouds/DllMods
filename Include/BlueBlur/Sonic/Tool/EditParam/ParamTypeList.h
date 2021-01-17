#pragma once

#include <BlueBlur.h>

#include <Hedgehog/Base/Container/hhMap.h>
#include <Hedgehog/Base/System/hhAllocator.h>
#include <Sonic/Tool/EditParam/ParamBase.h>

namespace Sonic
{
    class CParamTypeList;

    static void* const pCParamTypeListConstructor = (void*)0xCEBF90;
    static void* const pCParamTypeListAddValue = (void*)0xCEBD70;

    static CParamTypeList* fCParamTypeListConstructor(
        CParamTypeList* pParamTypeList, const Hedgehog::Base::CSharedString* pDescription, uint32_t* pValue, uint32_t unknown)
    {
        __asm
        {
            mov esi, pParamTypeList
            push unknown
            push pValue
            push pDescription
            call[pCParamTypeListConstructor]
        }
    }

    static void fCParamTypeListAddValue(CParamTypeList* pParamTypeList, const Hedgehog::Base::CSharedString* pName, uint32_t value)
    {
        __asm
        {
            mov eax, pName
            mov edi, pParamTypeList
            mov esi, value
            call[pCParamTypeListAddValue]
        }
    }

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

        void AddValue(const Hedgehog::Base::CSharedString& name, uint32_t value)
        {
            fCParamTypeListAddValue(this, &name, value);
        }

        static CParamTypeList* Create(uint32_t* pValue, const Hedgehog::Base::CSharedString& description)
        {
            CParamTypeList* pParamTypeList = (CParamTypeList*)Hedgehog::Base::fpOperatorNew(sizeof(CParamTypeList));
            fCParamTypeListConstructor(pParamTypeList, &description, pValue, 0);

            pParamTypeList->AddRef();
            return pParamTypeList;
        }
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
