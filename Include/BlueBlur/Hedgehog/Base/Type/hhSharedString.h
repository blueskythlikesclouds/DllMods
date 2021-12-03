#pragma once

#include <BlueBlur.h>
#include <Hedgehog/Base/Type/hhCowData.h>

namespace Hedgehog::Base
{
    class CSharedString
    {
    private:
        CCowData<char> m_data;

    public:
        CSharedString(const char* data)
        {
            m_data.Set(data, data ? strlen(data) : 0);
        }

        const char* c_str() const
        {
            return m_data.Get();
        }
    };

    ASSERT_SIZEOF(CSharedString, 0x4);
}