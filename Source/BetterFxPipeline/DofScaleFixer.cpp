#include "Configuration.h"
#include "DofScaleFixer.h"

namespace
{
    const std::array<uint32_t, 13> VALUE_ADDRESSES =
    {
        0x01A572E0, 0x01A572E4, 0x01A572E8, 0x01A572EC, 0x01A572F0, 0x01A572F4, 0x01A572F8, 0x01A572FC, 0x01A43594, 0x01A5730C, 0x01A57304, 0x01A57308, 0x01E5E320
    };

    template<typename T>
    void execute(const T& func)
    {
        const uint32_t height = Configuration::height > 0 ? Configuration::height : *(uint32_t*)0x1DFDDE0;

        std::array<uint32_t, VALUE_ADDRESSES.size()> values {};

        for (size_t i = 0; i < VALUE_ADDRESSES.size(); i++)
        {
            values[i] = *(uint32_t*)VALUE_ADDRESSES[i];
            *(float*)VALUE_ADDRESSES[i] *= 720.0f / height;
        }

        func();

        for (size_t i = 0; i < VALUE_ADDRESSES.size(); i++)
            *(uint32_t*)VALUE_ADDRESSES[i] = values[i];
    }
}

// FxPipeline
HOOK(void, __fastcall, ExecuteAlternativeDepthOfField, 0x1228960, YggJob* This)
{
    execute([=]() { originalExecuteAlternativeDepthOfField(This); });
}

// MTFx
HOOK(void, __stdcall, CRenderDirectorMTFxStoreSceneEffectParams, 0x579050, void* A1, void* A2)
{
    execute([=]() { originalCRenderDirectorMTFxStoreSceneEffectParams(A1, A2); });
}

bool DofScaleFixer::enabled = false;

void DofScaleFixer::applyPatches()
{
    if (enabled)
        return;

    enabled = true;

    INSTALL_HOOK(ExecuteAlternativeDepthOfField);
    INSTALL_HOOK(CRenderDirectorMTFxStoreSceneEffectParams);
}
