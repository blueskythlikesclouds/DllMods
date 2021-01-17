#pragma once

#include <BlueBlur.h>

namespace Hedgehog::Base
{
    static inline FUNCTION_PTR(void*, __cdecl, fpOperatorNew, 0x4011D0, const size_t size);
    static inline FUNCTION_PTR(void, __cdecl, fpOperatorDelete, 0x4011E0, void* pData);
}