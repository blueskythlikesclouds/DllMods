#pragma once

#include <BlueBlur.h>
#include <Hedgehog/Yggdrasill/hhYggAbstractBuffer.h>

namespace Hedgehog::Yggdrasill
{
    class CYggSurface : public CYggAbstractBuffer
    {
    public:
        DX_PATCH::IDirect3DSurface9* m_pD3DSurface;
        INSERT_PADDING(0x48);
    };

    ASSERT_OFFSETOF(CYggSurface, m_pD3DSurface, 0x1C);
    ASSERT_SIZEOF(CYggSurface, 0x68);
}
