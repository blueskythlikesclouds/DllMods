#pragma once

#include <BlueBlur.h>

#include <Hedgehog/Base/Type/hhSharedString.h>
#include <Sonic/Tool/ParameterEditor/AbstractParameterNode.h>

namespace Sonic
{
    class CParamBool;

    template<typename T>
    class CParamValue;

    class CParameterCategory;

    template<typename T>
    struct SParamValueCreationParams
    {
        T* m_pValue;
        T m_DefaultValue;
        alignas(4) INSERT_PADDING(0x24);
    };

    static void* const pCParameterCategoryCreateParamBool = (void*)0xCEF4F0;
    static void* const pCParameterCategoryCreateParamUnsignedLong = (void*)0x591300;
    static void* const pCParameterCategoryCreateParamLong = (void*)0x590FB0;
    static void* const pCParameterCategoryCreateParamInt = (void*)0x590DA0;
    static void* const pCParameterCategoryCreateParamFloat = (void*)0x590770;

    static CParamBool* fCParameterCategoryCreateParamBool(
        const SParamValueCreationParams<bool>* pCreationParams, CParameterCategory* pParameterCategory, const Hedgehog::Base::CSharedString* pName)
    {
        __asm
        {
            mov eax, pCreationParams
            push pName
            push pParameterCategory
            call [pCParameterCategoryCreateParamBool]
        }
    }

    static CParamValue<unsigned long>* fCParameterCategoryCreateParamUnsignedLong(
        const SParamValueCreationParams<unsigned long>* pCreationParams, CParameterCategory* pParameterCategory, const Hedgehog::Base::CSharedString* pName)
    {
        __asm
        {
            mov eax, pCreationParams
            push pName
            push pParameterCategory
            call [pCParameterCategoryCreateParamUnsignedLong]
        }
    }

    static CParamValue<long>* fCParameterCategoryCreateParamLong(
        const SParamValueCreationParams<long>* pCreationParams, CParameterCategory* pParameterCategory, const Hedgehog::Base::CSharedString* pName)
    {
        __asm
        {
            mov eax, pCreationParams
            push pName
            push pParameterCategory
            call [pCParameterCategoryCreateParamLong]
        }
    }

    static CParamValue<int>* fCParameterCategoryCreateParamInt(
        const SParamValueCreationParams<int>* pCreationParams, CParameterCategory* pParameterCategory, const Hedgehog::Base::CSharedString* pName)
    {
        __asm
        {
            mov eax, pCreationParams
            push pName
            push pParameterCategory
            call [pCParameterCategoryCreateParamInt]
        }
    }

    static CParamValue<float>* fCParameterCategoryCreateParamFloat(
        const SParamValueCreationParams<float>* pCreationParams, CParameterCategory* pParameterCategory, const Hedgehog::Base::CSharedString* pName)
    {
        __asm
        {
            mov eax, pCreationParams
            push pName
            push pParameterCategory
            call [pCParameterCategoryCreateParamFloat]
        }
    }

    class CParameterCategory : public CAbstractParameterNode
    {
    public:
        static constexpr void* ms_pVfTable = (void*)0x16E5FFC;

        INSERT_PADDING(0xC);

        CParamBool* CreateParamBool(const SParamValueCreationParams<bool>& creationParams, const Hedgehog::Base::CSharedString& name)
        {
            return fCParameterCategoryCreateParamBool(&creationParams, this, &name);
        }

        CParamBool* CreateParamBool(bool* pValue, const Hedgehog::Base::CSharedString& name)
        {
            return CreateParamBool({ pValue, *pValue }, name);
        }

        CParamValue<unsigned long>* CreateParamUnsignedLong(const SParamValueCreationParams<unsigned long>& creationParams, const Hedgehog::Base::CSharedString& name)
        {
            return fCParameterCategoryCreateParamUnsignedLong(&creationParams, this, &name);
        }

        CParamValue<unsigned long>* CreateParamUnsignedLong(unsigned long* pValue, const Hedgehog::Base::CSharedString& name)
        {
            return CreateParamUnsignedLong({ pValue, *pValue }, name);
        }

        CParamValue<long>* CreateParamLong(const SParamValueCreationParams<long>& creationParams, const Hedgehog::Base::CSharedString& name)
        {
            return fCParameterCategoryCreateParamLong(&creationParams, this, &name);
        }

        CParamValue<long>* CreateParamLong(long* pValue, const Hedgehog::Base::CSharedString& name)
        {
            return CreateParamLong({ pValue, *pValue }, name);
        }

        CParamValue<int>* CreateParamInt(const SParamValueCreationParams<int>& creationParams, const Hedgehog::Base::CSharedString& name)
        {
            return fCParameterCategoryCreateParamInt(&creationParams, this, &name);
        }

        CParamValue<int>* CreateParamInt(int* pValue, const Hedgehog::Base::CSharedString& name)
        {
            return CreateParamInt({ pValue, *pValue }, name);
        }

        CParamValue<float>* CreateParamFloat(const SParamValueCreationParams<float>& creationParams, const Hedgehog::Base::CSharedString& name)
        {
            return fCParameterCategoryCreateParamFloat(&creationParams, this, &name);
        }

        CParamValue<float>* CreateParamFloat(float* pValue, const Hedgehog::Base::CSharedString& name)
        {
            return CreateParamFloat({ pValue, *pValue }, name);
        }
    };

    ASSERT_SIZEOF(CParameterCategory, 0x48);
}