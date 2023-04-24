#include "Terrain.h"

static constexpr XXH64_hash_t HASHES[] =
{
    0x011EE9141C36039E,
    0x041B78876AB1A5A6,
    0x094FB182392881E2,
    0x171F64BA60CA5A8B,
    0x1FEC932FCC006DBC,
    0x21CE5522DF9C9879,
    0x31A4BC8B0DBEFF4B,
    0x37EBD10759C1B326,
    0x3F1F441F50E5CF10,
    0x42ED01689B33C2EA,
    0x46CEF7AF7F8335BD,
    0x514E71A6B7AD7068,
    0x515D4A3F16E4911E,
    0x5FBDCABD7ED22210,
    0x695F20C68B0F6147,
    0x76D27DC3922EAEBC,
    0x785DA92B3611BFD3,
    0x885B666288F2CE97,
    0xA820A56CDE18E28F,
    0xB2106691B5843766,
    0xB4691FF31B6F3AFA,
    0xBE218A77998A25AE,
    0xC5B441FCCC6530E8,
    0xCF660335FE044414,
    0xE58B8DB461A6274D,
    0xE791B44FF3AAAB38,
    0xF5E004784621B17B
};

VTABLE_HOOK(HRESULT, WINAPI, ID3D11Device, CreatePixelShader,
    const void* pShaderBytecode,
    SIZE_T BytecodeLength,
    ID3D11ClassLinkage* pClassLinkage,
    ID3D11PixelShader** ppPixelShader
)
{
    if (std::ranges::binary_search(HASHES, XXH64(pShaderBytecode, BytecodeLength, 0)))
        return originalID3D11DeviceCreatePixelShader(This, TERRAIN_PS, sizeof(TERRAIN_PS), pClassLinkage, ppPixelShader);

    else
        return originalID3D11DeviceCreatePixelShader(This, pShaderBytecode, BytecodeLength, pClassLinkage, ppPixelShader);
}

HOOK(HRESULT, WINAPI, D3D11CreateDeviceImp, PROC_ADDRESS("d3d11.dll", "D3D11CreateDevice"),
    IDXGIAdapter* pAdapter,
    D3D_DRIVER_TYPE DriverType,
    HMODULE Software,
    UINT Flags,
    const D3D_FEATURE_LEVEL* pFeatureLevels,
    UINT FeatureLevels,
    UINT SDKVersion,
    ID3D11Device** ppDevice,
    D3D_FEATURE_LEVEL* pFeatureLevel,
    ID3D11DeviceContext** ppImmediateContext
)
{
    const HRESULT result = originalD3D11CreateDeviceImp(
        pAdapter,
        DriverType,
        Software,
        Flags,
        pFeatureLevels,
        FeatureLevels,
        SDKVersion,
        ppDevice,
        pFeatureLevel,
        ppImmediateContext);

    if (SUCCEEDED(result) && *ppDevice)
        INSTALL_VTABLE_HOOK(ID3D11Device, *ppDevice, CreatePixelShader, 15);

    return result;
}

extern "C" void __declspec(dllexport) Init()
{
    INSTALL_HOOK(D3D11CreateDeviceImp);

    WRITE_MEMORY(0x140AB5871, uint8_t, 0x10);
    WRITE_MEMORY(0x140AB4E54, uint8_t, 0x10);
    WRITE_MEMORY(0x140AB4E7A, uint8_t, 0x10);
    WRITE_MEMORY(0x140AB4FCA, uint8_t, 0x10);
    WRITE_MEMORY(0x140AB58E5, uint8_t, 0x10);
    WRITE_MEMORY(0x140AB5916, uint8_t, 0x10);

    WRITE_MEMORY(0x140AB4E58, uint32_t, 0xFFFF);
    WRITE_MEMORY(0x140AB4E7E, uint32_t, 0xFFFF);
    WRITE_MEMORY(0x140AB4FC4, uint32_t, 0xFFFF);
    WRITE_MEMORY(0x140AB58DF, uint32_t, 0xFFFF);
    WRITE_MEMORY(0x140AB5910, uint32_t, 0xFFFF);
}

BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
{
    return TRUE;
}