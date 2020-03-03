#include "Configuration.h"
#include "DofScaleFixer.h"

FUNCTION_PTR(uint32_t, __thiscall, fun7846D0, 0x7846D0, uint32_t This);

HOOK(uint32_t, __fastcall, ExecuteAlternativeDepthOfField, 0x1228960, uint32_t This)
{
    const uint32_t valueAddresses[] = {
        0x01A572E0, 0x01A572E4, 0x01A572E8, 0x01A572EC, 0x01A572F0, 0x01A572F4, 0x01A572F8, 0x01A572FC, 0x01A43594,
        0x01A5730C, 0x01A57304, 0x01A57308, 0x01E5E320
    };
    constexpr size_t valueCount = _countof(valueAddresses);

    const uint32_t obj = fun7846D0(This);
    const uint32_t height = Configuration::height > 0 ? Configuration::height : *(uint32_t*)(obj + 204);

    uint32_t values[valueCount];
    for (size_t i = 0; i < valueCount; i++)
    {
        values[i] = *(uint32_t*)valueAddresses[i];
        *(float*)valueAddresses[i] *= 720.0f / height;
    }

    const uint32_t result = originalExecuteAlternativeDepthOfField(This);

    for (size_t i = 0; i < valueCount; i++)
        *(uint32_t*)valueAddresses[i] = values[i];

    return result;
}

bool DofScaleFixer::enabled = false;

void DofScaleFixer::applyPatches()
{
    if (enabled)
        return;

    enabled = true;

    INSTALL_HOOK(ExecuteAlternativeDepthOfField);
}
