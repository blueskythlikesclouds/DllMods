#include "UpdateDirector.h"

FUNCTION_PTR(void*, __thiscall, fun662010, 0x662010, void* This, const Hedgehog::Base::CSharedString& value);
FUNCTION_PTR(void*, __thiscall, fun77AC20, 0x77AC20, void* This, uint32_t a2);
FUNCTION_PTR(void*, __thiscall, fun77ACC0, 0x77ACC0, void* This, uint32_t a2);

FUNCTION_PTR(void*, __thiscall, fun6F8B10, 0x6F8B10, void* This);
FUNCTION_PTR(void*, __thiscall, fun6F8FB0, 0x6F8FB0, void* This);

FUNCTION_PTR(void*, __thiscall, funD0AEE0, 0xD0AEE0, void* This);
FUNCTION_PTR(void*, __thiscall, fun6F8AF0, 0x6F8AF0, void* This);

uint32_t funD67820FunctionAddress = 0xD67820;
uint32_t funD66F90FunctionAddress = 0xD66F90;

void addUpdateCommand(void* context, const float elapsedTime, const char* name)
{
    fun662010((void*)(*(uint32_t*)((uint32_t)context + 4) + 216), name);
    *(float*)(*(uint32_t*)((uint32_t)context + 4) + 208) = elapsedTime;
    uint32_t v3 = *(uint32_t*)((uint32_t)context + 4);
    if (*(uint32_t*)(v3 + 36))
        fun77AC20(*(uint32_t**)(v3 + 36), v3 + 208);
}

void addRenderCommand(void* context, const float elapsedTime, const char* name)
{
    fun662010((void*)(*(uint32_t*)((uint32_t)context + 4) + 216), name);
    *(float*)(*(uint32_t*)((uint32_t)context + 4) + 208) = elapsedTime;
    uint32_t v3 = *(uint32_t*)((uint32_t)context + 4);
    if (*(uint32_t*)(v3 + 36))
        fun77ACC0(*(uint32_t**)(v3 + 36), v3 + 208);
}

void addUpdateAndRenderCommand(void* context, const float elapsedTime, const char* name)
{
    addUpdateCommand(context, elapsedTime, name);
    addRenderCommand(context, elapsedTime, name);
}

void callUnknownFunction(void* context, float elapsedTime)
{
    funD0AEE0(*(uint32_t**)(*(uint32_t*)((uint32_t)context + 4) + 52));

    __asm
    {
        mov eax, context
        push elapsedTime
        call [funD66F90FunctionAddress]
    }

    fun6F8AF0(*(uint32_t**)(*(uint32_t*)((uint32_t)context + 4) + 56));
}

void* finishUpdater(void* context)
{
    return fun6F8B10(*(uint32_t**)(*(uint32_t*)((uint32_t)context + 4) + 56));
}

void* finishRenderer(void* context)
{
    __asm
    {
        mov esi, context
        call [funD67820FunctionAddress]
    }

    if (*(bool*)0x1A430EC)
        return fun6F8FB0(*(uint32_t**)(*(uint32_t*)((uint32_t)context + 4) + 56));

    return nullptr;
}