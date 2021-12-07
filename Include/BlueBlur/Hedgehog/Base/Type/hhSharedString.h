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
        CSharedString()
        {

        }

        CSharedString(const char* data)
        {
            m_data.Set(data, data ? strlen(data) : 0);
        }

        const char* c_str() const
        {
            return m_data.Get();
        }

        CSharedString& operator=(const CSharedString& value)
        {
            m_data.Unset();
            m_data.Set(value.m_data);
            return *this;
        }        
        
        CSharedString& operator=(const char* value)
        {
            m_data.Unset();
            m_data.Set(value, value ? strlen(value) : 0);
            return *this;
        }

        bool operator==(const CSharedString& value) const
        {
            return strcmp(c_str(), value.c_str()) == 0;
        }        
        
        bool operator!=(const CSharedString& value) const
        {
            return !(*this == value);
        }

        bool operator==(const char* value) const
        {
            return strcmp(c_str(), value) == 0;
        }       
        
        bool operator!=(const char* value) const
        {
            return !(*this == value);
        }
    };

    ASSERT_SIZEOF(CSharedString, 0x4);
}