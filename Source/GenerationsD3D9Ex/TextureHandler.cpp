#include "TextureHandler.h"

FUNCTION_PTR(void*, __thiscall, fun736320, 0x736320, void* This, void* a1, void* a2);

// Seems like it lowers the RAM usage. I'm likely having a placebo effect, though...
HOOK(void*, __fastcall, Fun736380, 0x736380, void* This, void* Edx, void* a1, void* a2, void* a3, void* a4, void* a5)
{
    return fun736320(This, a1, a2);
}

struct DDS_PIXELFORMAT
{
    DWORD dwSize;
    DWORD dwFlags;
    DWORD dwFourCC;
    DWORD dwRGBBitCount;
    DWORD dwRBitMask;
    DWORD dwGBitMask;
    DWORD dwBBitMask;
    DWORD dwABitMask;
};

enum
{
    D3DFMT_ATI1 = MAKEFOURCC('A', 'T', 'I', '1'),
    D3DFMT_ATI2 = MAKEFOURCC('A', 'T', 'I', '2'),
    D3DFMT_BC4U = MAKEFOURCC('B', 'C', '4', 'U'),
    D3DFMT_BC5U = MAKEFOURCC('B', 'C', '5', 'U')
};

HOOK(D3DFORMAT, __cdecl, GetFormat, 0xA61AD0, DDS_PIXELFORMAT* pixelFormat)
{
    switch (pixelFormat->dwFourCC)
    {
    case D3DFMT_ATI1:
    case D3DFMT_BC4U:
        return (D3DFORMAT)D3DFMT_ATI1;

    case D3DFMT_ATI2:
    case D3DFMT_BC5U:
        return (D3DFORMAT)D3DFMT_ATI2;

    default:
        return originalGetFormat(pixelFormat);
    }
}

HOOK(uint32_t, __cdecl, GetBitsPerPixel, 0xA62190, D3DFORMAT format)
{
    switch ((uint32_t)format)
    {
    case D3DFMT_ATI1:
    case D3DFMT_BC4U:
        return 4;

    case D3DFMT_ATI2:
    case D3DFMT_BC5U:
        return 8;

    default:
        return originalGetBitsPerPixel(format);
    }
}

// dunno why this has a stdcall variant but whatever...

HOOK(uint32_t, __stdcall, GetBitsPerPixel2, 0xA60E40, D3DFORMAT format)
{
    return implOfGetBitsPerPixel(format);
}

void TextureHandler::applyPatches()
{
    INSTALL_HOOK(GetFormat);
    INSTALL_HOOK(GetBitsPerPixel);
    INSTALL_HOOK(GetBitsPerPixel2);
    INSTALL_HOOK(Fun736380);
}
