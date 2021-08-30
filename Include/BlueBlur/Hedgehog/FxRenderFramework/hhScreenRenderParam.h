#pragma once

#pragma once

#include <BlueBlur.h>

namespace Hedgehog::FxRenderFramework
{
    struct SScreenRenderParam
    {
        const char* m_pName;
        void* m_pCallback; // SceneTraverse or SceneRender
        int32_t m_ShaderIndex;
        INSERT_PADDING(0x18);
    };

    ASSERT_SIZEOF(SScreenRenderParam, 0x24);
}