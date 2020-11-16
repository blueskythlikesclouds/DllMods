#pragma once

#include <BlueBlur.h>
#include <Sonic/FxPipeline/Jobs/FxJob.h>

namespace Sonic
{
    class CFxAlternativeDepthOfField;

    static FUNCTION_PTR(void, __thiscall, fpCFxAlternativeDepthOfFieldInitialize, 0x1228620, CFxAlternativeDepthOfField* This);
    static FUNCTION_PTR(void, __thiscall, fpCFxAlternativeDepthOfFieldExecute, 0x1228960, CFxAlternativeDepthOfField* This);

    class CFxAlternativeDepthOfField : public CFxJob
    {
    public:
        INSERT_PADDING(0x70);
    };

    ASSERT_SIZEOF(CFxAlternativeDepthOfField, 0xB8);
}