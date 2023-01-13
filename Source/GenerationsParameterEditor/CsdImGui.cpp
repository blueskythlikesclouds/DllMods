#include "CsdImGui.h"

#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

void CsdImGui::init()
{
    ImGuiIO& io = ImGui::GetIO();
    io.BackendRendererName = "CsdImGui";
    io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;
}

void CsdImGui::render(const ImDrawData* drawData)
{
    const auto applicationDocument = Sonic::CApplicationDocument::GetInstance();

    if (!applicationDocument ||
        !applicationDocument->m_pMember->m_spRenderingInfrastructure ||
        !applicationDocument->m_pMember->m_spDatabase)
    {
        return;
    }

    hh::mr::CMirageDatabaseWrapper wrapper(applicationDocument->m_pMember->m_spDatabase.get());

    const auto csdVS = wrapper.GetVertexShaderCodeData("csd");
    const auto csdPS = wrapper.GetPixelShaderCodeData("csd");

    const auto csdNoTexVS = wrapper.GetVertexShaderCodeData("csdNoTex");
    const auto csdNoTexPS = wrapper.GetPixelShaderCodeData("csdNoTex");

    if (!csdVS || !csdVS->IsMadeAll() ||
        !csdPS || !csdPS->IsMadeAll() ||
        !csdNoTexVS || !csdNoTexVS->IsMadeAll() ||
        !csdNoTexPS || !csdNoTexPS->IsMadeAll())
    {
        return;
    }

    const auto device = applicationDocument->m_pMember->m_spRenderingInfrastructure->m_RenderingDevice.m_pD3DDevice;

    static ComPtr<DX_PATCH::IDirect3DVertexDeclaration9> vertexDeclaration;

    if (!vertexDeclaration)
    {
        static constexpr D3DVERTEXELEMENT9 VERTEX_ELEMENTS[] =
        {
            { 0, offsetof(ImDrawVert, pos), D3DDECLTYPE_FLOAT2, 0, D3DDECLUSAGE_POSITION, 0 },
            { 0, offsetof(ImDrawVert, uv), D3DDECLTYPE_FLOAT2, 0, D3DDECLUSAGE_TEXCOORD, 0 },
            { 0, offsetof(ImDrawVert, col), D3DDECLTYPE_UBYTE4N, 0, D3DDECLUSAGE_COLOR, 0 },
            D3DDECL_END()
        };

        device->CreateVertexDeclaration(VERTEX_ELEMENTS, &vertexDeclaration);
    }

    static ComPtr<DX_PATCH::IDirect3DVertexBuffer9> vtxBuffer;
    static ComPtr<DX_PATCH::IDirect3DIndexBuffer9> idxBuffer;
    static size_t vtxBufferSize;
    static size_t idxBufferSize;

    const size_t curVtxBufferSize = drawData->TotalVtxCount * sizeof(ImDrawVert);
    const size_t curIdxBufferSize = drawData->TotalIdxCount * sizeof(ImDrawIdx);

    if (curVtxBufferSize == 0 || curIdxBufferSize == 0)
        return;

    if (!vtxBuffer || vtxBufferSize < curVtxBufferSize)
        device->CreateVertexBuffer(vtxBufferSize = curVtxBufferSize, D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &vtxBuffer, nullptr);

    if (!idxBuffer || idxBufferSize < curIdxBufferSize)
        device->CreateIndexBuffer(idxBufferSize = curIdxBufferSize, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &idxBuffer, nullptr);

    ImDrawVert* vtx = nullptr;
    ImDrawIdx* idx = nullptr;

    vtxBuffer->Lock(0, curVtxBufferSize, (void**)&vtx, D3DLOCK_DISCARD);
    idxBuffer->Lock(0, curIdxBufferSize, (void**)&idx, D3DLOCK_DISCARD);

    for (int i = 0; i < drawData->CmdListsCount; i++)
    {
        auto drawList = drawData->CmdLists[i];

        const ImDrawVert* srcVtx = drawList->VtxBuffer.Data;
        for (int j = 0; j < drawList->VtxBuffer.Size; j++)
        {
            const uint8_t r = (srcVtx->col >> IM_COL32_R_SHIFT) & 0xFF;
            const uint8_t g = (srcVtx->col >> IM_COL32_G_SHIFT) & 0xFF;
            const uint8_t b = (srcVtx->col >> IM_COL32_B_SHIFT) & 0xFF;
            const uint8_t a = (srcVtx->col >> IM_COL32_A_SHIFT) & 0xFF;

            // wxyz: rgba
            // x: g
            // y: b
            // z: a
            // w: r
            vtx->col = (g << 0) | (b << 8) | (a << 16) | (r << 24);

            vtx->pos[0] = srcVtx->pos.x + 0.5f;
            vtx->pos[1] = srcVtx->pos.y + 0.5f;
            vtx->uv[0] = srcVtx->uv.x;
            vtx->uv[1] = srcVtx->uv.y;

            vtx++;
            srcVtx++;
        }

        memcpy(idx, drawList->IdxBuffer.Data, drawList->IdxBuffer.size_in_bytes());
        idx += drawList->IdxBuffer.Size;
    }

    vtxBuffer->Unlock();
    idxBuffer->Unlock();

    static ComPtr<DX_PATCH::IDirect3DBaseTexture9> font;
    if (!font)
    {
        ImGuiIO& io = ImGui::GetIO();

        uint8_t* pixels;
        int width, height, bpp;

        io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height, &bpp);

        static constexpr uint8_t DDS_HEADER[] = 
        {
            0x44, 0x44, 0x53, 0x20, 0x7C, 0x00, 0x00, 0x00, 0x0F, 0x10, 0x02, 0x00,
            0x58, 0x02, 0x00, 0x00, 0x20, 0x03, 0x00, 0x00, 0x80, 0x0C, 0x00, 0x00,
            0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x41, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00,
            0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
            0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        };

        const size_t texSize = width * height * bpp;
        const size_t ddsSize = sizeof(DDS_HEADER) + texSize;

        auto ddsData = std::make_unique<uint8_t[]>(ddsSize);

        memcpy(ddsData.get(), DDS_HEADER, sizeof(DDS_HEADER));

        for (int i = 0; i < width * height; i++)
        {
            uint8_t* srcPixel = pixels + i * bpp;
            uint8_t* dstPixel = ddsData.get() + sizeof(DDS_HEADER) + i * bpp;

            dstPixel[0] = srcPixel[2];
            dstPixel[1] = srcPixel[1];
            dstPixel[2] = srcPixel[0];
            dstPixel[3] = srcPixel[3];
        }

        *(int*)(ddsData.get() + 12) = height;
        *(int*)(ddsData.get() + 16) = width;

        font.Attach(makePicture(ddsData.get(), ddsSize));
        io.Fonts->SetTexID(font.Get());
    }

    D3DVIEWPORT9 viewport{};
    viewport.Width = (DWORD)drawData->DisplaySize.x;
    viewport.Height = (DWORD)drawData->DisplaySize.y;
    viewport.MaxZ = 1.0f;

    device->SetViewport(&viewport);
    device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
    device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    device->SetRenderState(D3DRS_ZENABLE, FALSE);
    device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
    device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    device->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE);
    device->SetRenderState(D3DRS_SRCBLENDALPHA, D3DBLEND_ONE);
    device->SetRenderState(D3DRS_DESTBLENDALPHA, D3DBLEND_INVSRCALPHA);
    device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
    device->SetVertexDeclaration(vertexDeclaration.Get());
    device->SetStreamSource(0, vtxBuffer.Get(), 0, sizeof(ImDrawVert));
    device->SetIndices(idxBuffer.Get());

    float viewportSize[] = { drawData->DisplaySize.x, drawData->DisplaySize.y, 1.0f / drawData->DisplaySize.x, 1.0f / drawData->DisplaySize.y };
    float scaleSize[] = { 1.0f, 1.0f, 0.0f, 0.0f };
    float z[] = { 0.0f, 0.0f, 0.0f, 0.0f };

    device->SetVertexShaderConstantF(180, viewportSize, 1);
    device->SetVertexShaderConstantF(246, z, 1);
    device->SetVertexShaderConstantF(247, scaleSize, 1);

    const ImVec2 clipOffset = drawData->DisplayPos;
    size_t vtxOffset = 0;
    size_t idxOffset = 0;

    for (int i = 0; i < drawData->CmdListsCount; i++)
    {
        const ImDrawList* cmdList = drawData->CmdLists[i];

        for (int j = 0; j < cmdList->CmdBuffer.Size; j++)
        {
            const ImDrawCmd& cmd = cmdList->CmdBuffer[j];

            ImVec2 clipMin(cmd.ClipRect.x - clipOffset.x, cmd.ClipRect.y - clipOffset.y);
            ImVec2 clipMax(cmd.ClipRect.z - clipOffset.x, cmd.ClipRect.w - clipOffset.y);

            if (clipMax.x <= clipMin.x || clipMax.y <= clipMin.y)
                continue;

            const RECT scissorRect = { (LONG)clipMin.x, (LONG)clipMin.y, (LONG)clipMax.x, (LONG)clipMax.y };

            const bool hasTexture = cmd.GetTexID() != nullptr;

            device->SetVertexShader(hasTexture ? csdVS->m_pD3DVertexShader : csdNoTexVS->m_pD3DVertexShader);
            device->SetPixelShader(hasTexture ? csdPS->m_pD3DPixelShader : csdNoTexPS->m_pD3DPixelShader);

            device->SetTexture(0, (DX_PATCH::IDirect3DBaseTexture9*)cmd.GetTexID());

            device->SetScissorRect(&scissorRect);
            device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, cmd.VtxOffset + vtxOffset, 0, (UINT)cmdList->VtxBuffer.Size, cmd.IdxOffset + idxOffset, cmd.ElemCount / 3);
        }

        idxOffset += cmdList->IdxBuffer.Size;
        vtxOffset += cmdList->VtxBuffer.Size;
    }
}

DX_PATCH::IDirect3DBaseTexture9* CsdImGui::makePicture(uint8_t* ddsData, size_t ddsDataSize)
{
    auto picture = boost::make_shared<hh::mr::CPictureData>();
    picture->m_Flags = 0;

    hh::mr::CPictureData::Make(picture.get(), ddsData, ddsDataSize, 
        Sonic::CApplicationDocument::GetInstance()->m_pMember->m_spRenderingInfrastructure.get());

    auto texture = picture->m_pD3DTexture;
    picture->m_pD3DTexture = nullptr;

    return texture;
}
