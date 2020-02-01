#pragma once

struct CriticalSection
{
    void* value;
    bool success;

    void enter(void** address, bool force = false);
    void leave();
};
