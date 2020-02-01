#include "CriticalSection.h"

FUNCTION_PTR(void*, __thiscall, enterCriticalSection, 0x65FBE0, CriticalSection* This, void** address, bool force);
FUNCTION_PTR(void, __thiscall, leaveCriticalSection, 0x65FC40, CriticalSection* This);

void CriticalSection::enter(void** address, const bool force)
{
    enterCriticalSection(this, address, force);
}

void CriticalSection::leave()
{
    leaveCriticalSection(this);
}
