#pragma once

#include <BlueBlur.h>
#include <Sonic/FxPipeline/Jobs/FxJob.h>

namespace Sonic
{
    class CFxCrossFade;

    static FUNCTION_PTR(void, __thiscall, fpCFxCrossFadeInitialize, 0x10C21A0, CFxCrossFade* This);
    static FUNCTION_PTR(void, __thiscall, fpCFxCrossFadeExecute, 0x10C22D0, CFxCrossFade* This);

    class CFxCrossFade : public CFxJob
    {
    public:
        INSERT_PADDING(0x20);
    };

    ASSERT_SIZEOF(CFxCrossFade, 0x68);
}