#pragma once

#include <BlueBlur.h>

#include <Hedgehog/Base/Type/hhSharedString.h>
#include <Sonic/Tool/ParameterEditor/AbstractParameterNode.h>

namespace Sonic
{
    class CParameterCategory;
    class CParameterGroup;

    static void* const pCParameterGroupCreateParameterCategory = (void*)0xCE90B0;

    static CParameterCategory* fCParameterGroupCreateParameterCategory(
        const Hedgehog::Base::CSharedString* pName, CParameterGroup* pParameterGroup, const Hedgehog::Base::CSharedString* pDescription, uint32_t unknown)
    {
        __asm
        {
            mov eax, pName
            mov ecx, pParameterGroup
            push unknown
            push pDescription
            call [pCParameterGroupCreateParameterCategory]
        }
    }

    static FUNCTION_PTR(void, __stdcall, fpCParameterGroupFlush, 0xCE8EF0, CParameterGroup* pParameterGroup);

    class CParameterGroup : public CAbstractParameterNode
    {
    public:
        static constexpr void* ms_pVfTable = (void*)0x16E5FA4;

        CParameterCategory* CreateParameterCategory(
            const Hedgehog::Base::CSharedString& name, const Hedgehog::Base::CSharedString& description)
        {
            return fCParameterGroupCreateParameterCategory(&name, this, &description, 0xFF);
        }

        void Flush()
        {
            fpCParameterGroupFlush(this);
        }
    };

    ASSERT_SIZEOF(CParameterGroup, 0x3C);
}