#pragma once

struct CsdImGui
{
    static void init();
    static void render(const ImDrawData* drawData);

    static DX_PATCH::IDirect3DBaseTexture9* makePicture(uint8_t* ddsData, size_t ddsDataSize);
};
