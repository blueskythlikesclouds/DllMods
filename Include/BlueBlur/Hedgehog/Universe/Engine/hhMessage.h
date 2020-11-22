#pragma once

#include <BlueBlur.h>
#include <Hedgehog/Base/hhObject.h>

namespace Hedgehog::Universe
{
    class Message : public Base::CObject
    {
    public:
        INSERT_PADDING(0xC);

        virtual void Compare(const char* pIdentifier) = 0;
        virtual const char* GetIdentifier() = 0;
    };

    ASSERT_SIZEOF(Message, 0x10);
}
