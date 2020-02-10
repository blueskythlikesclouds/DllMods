#pragma once

#include "Types.h"

struct SynchronizedObject
{
    SharedPtr<CRITICAL_SECTION>* criticalSection;
    void* object;

    class Lock
    {
    protected:
        SynchronizedObject* synchronizedObject;
        bool success;

    public:
        Lock(SynchronizedObject* synchronizedObject);
        ~Lock();

        void* getObject() const;
        bool getSuccess() const;
    };
};