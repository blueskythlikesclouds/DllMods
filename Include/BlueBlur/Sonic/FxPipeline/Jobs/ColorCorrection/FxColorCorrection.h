#pragma once

#include <BlueBlur.h>
#include <Sonic/FxPipeline/Jobs/FxJob.h>

namespace Sonic
{
    class CFxColorCorrection;

    static FUNCTION_PTR(void, __thiscall, fpCFxColorCorrectionInitialize, 0x10C2CD0, CFxColorCorrection* This);
    static FUNCTION_PTR(void, __thiscall, fpCFxColorCorrectionExecute, 0x10C2E90, CFxColorCorrection* This);

    class CFxColorCorrection : public CFxJob
    {
    public:
        INSERT_PADDING(0x7C);
    };

    ASSERT_SIZEOF(CFxColorCorrection, 0xC4);
}