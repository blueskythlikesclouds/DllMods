#pragma once

#pragma once

#include <BlueBlur.h>

namespace Hedgehog::FxRenderFramework
{
    struct SScreenRenderParam
    {
        const char* m_pName;
        void* m_pCallback; // SceneTraverse or SceneRender
        uint8_t m_ShaderIndex;
        INSERT_PADDING(0x1B);
    };

    ASSERT_SIZEOF(SScreenRenderParam, 0x24);
}