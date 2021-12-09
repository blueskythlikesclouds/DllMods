#pragma once

#include <BlueBlur.h>
#include <Hedgehog/Base/Thread/hhHolderBase.h>

#define HH_BASE_HOLDER(x) \
    Hedgehog::Base::THolder(x)

namespace Hedgehog::Base
{
    template<typename T, bool ForceSync = false>
    class THolder : public CHolderBase
    {
    public:
        THolder(T* pObject) : CHolderBase(pObject, ForceSync) {}

        T* get() const
        {
            return static_cast<T*>(m_pSynchronizedObject);
        }

        T* operator->() const
        {
            return get();
        }

        T* operator*() const
        {
            return get();
        }
    };
}