#include "SynchronizedObject.h"

SynchronizedObject::Lock::Lock(SynchronizedObject* synchronizedObject)
    : synchronizedObject(synchronizedObject), success(false)
{
    if (!synchronizedObject || !synchronizedObject->criticalSection->value)
        return;

    success = true;

    EnterCriticalSection(synchronizedObject->criticalSection->value);
}

SynchronizedObject::Lock::~Lock()
{
    if (!success)
        return;

    LeaveCriticalSection(synchronizedObject->criticalSection->value);
}

void* SynchronizedObject::Lock::getObject() const
{
    return synchronizedObject->object;
}

bool SynchronizedObject::Lock::getSuccess() const
{
    return success;
}
