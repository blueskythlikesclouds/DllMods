#include "Configuration.h"
#include "DofScaleFixer.h"

const std::array<uint32_t, 13> VALUE_ADDRESSES = {
    0x01A572E0, 0x01A572E4, 0x01A572E8, 0x01A572EC, 0x01A572F0, 0x01A572F4, 0x01A572F8, 0x01A572FC, 0x01A43594, 0x01A5730C, 0x01A57304, 0x01A57308, 0x01E5E320 };

HOOK(void*, __fastcall, ExecuteAlternativeDepthOfField, 0x1228960, YggJob* This)
{
    const uint32_t height = Configuration::height > 0 ? Configuration::height : This->scheduler->internal->subInternal->height;

    std::array<uint32_t, VALUE_ADDRESSES.size()> values{};

    for (size_t i = 0; i < VALUE_ADDRESSES.size(); i++)
    {
        values[i] = *(uint32_t*)VALUE_ADDRESSES[i];
        *(float*)VALUE_ADDRESSES[i] *= 720.0f / height;
    }

    void* result = originalExecuteAlternativeDepthOfField(This);

    for (size_t i = 0; i < VALUE_ADDRESSES.size(); i++)
        *(uint32_t*)VALUE_ADDRESSES[i] = values[i];

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
