#pragma once

#include <BlueBlur.h>

namespace Hedgehog::Base
{
    class CHolderBase;
    class CSynchronizedObject;

    static FUNCTION_PTR(void, __thiscall, fpCHolderBaseCtor, 0x65FBE0, CHolderBase* This, CSynchronizedObject* pSynchronizedObject, bool forceSync);
    static FUNCTION_PTR(void, __thiscall, fpCHolderBaseDtor, 0x65FC40, CHolderBase* This);

    class CHolderBase
    {
    public:
        CSynchronizedObject* m_pSynchronizedObject;
        bool m_Success;

        CHolderBase(CSynchronizedObject* pSynchronizedObject, bool forceSync = false)
        {
            fpCHolderBaseCtor(this, pSynchronizedObject, forceSync);
        }

        ~CHolderBase()
        {
            fpCHolderBaseDtor(this);
        }
    };

    ASSERT_OFFSETOF(CHolderBase, m_pSynchronizedObject, 0x0);
    ASSERT_OFFSETOF(CHolderBase, m_Success, 0x4);
    ASSERT_SIZEOF(CHolderBase, 0x8);
}