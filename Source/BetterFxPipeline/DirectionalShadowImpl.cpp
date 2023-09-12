#include "DirectionalShadowImpl.h"

#include "ParameterManager.h"

// MTFx enforces vertical shadows, so let's implement
// directional shadows manually.

static uint32_t s_setVerticalLightViewProjectionMidAsmHookReturnAddress = 0x650F37;

static void __declspec(naked) setVerticalLightViewProjectionMidAsmHook()
{
    __asm
    {
        cmp ParameterManager::isEnableVerticalShadow, 0
        jz enableDirectionalShadow

        // Enable vertical shadow
        imul ecx, 0x1F0
        jmp end

    enableDirectionalShadow:
        mov ecx, 0x3E0

    end:
        jmp[s_setVerticalLightViewProjectionMidAsmHookReturnAddress]
    }
}

static uint32_t s_setLightViewProjectionMidAsmHookReturnAddress = 0x650FB5;

static void __declspec(naked) setLightViewProjectionMidAsmHook()
{
    __asm
    {
        cmp ParameterManager::isEnableVerticalShadow, 0
        jz enableDirectionalShadow

        // Enable vertical shadow
        imul ebx, 0x1F0
        jmp end

    enableDirectionalShadow:
        mov ebx, 0x3E0

    end:
        jmp[s_setLightViewProjectionMidAsmHookReturnAddress]
    }
}

static uint32_t* s_shadowMapNoTerrainCameraType = reinterpret_cast<uint32_t*>(0x13DD29C);

HOOK(void, __fastcall, RenderSimpleExecutor, 0x654C70, void* This, void* _, void* A2, void* A3)
{
    *s_shadowMapNoTerrainCameraType = ParameterManager::isEnableVerticalShadow ? 3 : 2;
    originalRenderSimpleExecutor(This, _, A2, A3);
}

void DirectionalShadowImpl::init()
{
    WRITE_JUMP(0x650F31, setVerticalLightViewProjectionMidAsmHook);
    WRITE_JUMP(0x650FAF, setLightViewProjectionMidAsmHook);

    DWORD oldProtect;
    VirtualProtect(s_shadowMapNoTerrainCameraType, sizeof(uint32_t), PAGE_READWRITE, &oldProtect);

    INSTALL_HOOK(RenderSimpleExecutor);
}
