#include "CompilingShadersImageRenderer.h"

#include "CompilingShadersImage.h"
#include "CompilingShadersImage.wpu.h"
#include "CompilingShadersImage.wvu.h"
#include "Configuration.h"
#include "Device.h"
#include "PixelShader.h"
#include "RenderTargetSurface.h"
#include "ShaderCache.h"
#include "SwapChain.h"
#include "Texture.h"
#include "VertexShader.h"

void CompilingShadersImageRenderer::render(Device* device)
{
    if (ShaderCache::compilingShaderCount == 0 && (alpha == 0.0f || Configuration::compileShadersBeforeStarting))
        return;

    float alphaVec[4];

    if (Configuration::compileShadersBeforeStarting)
    {
        alpha += 1.0f / 16.0f;
        alphaVec[0] = sinf(alpha - 1.57079632679f) * 0.5f + 0.5f;
    }
    else
    {
        if (ShaderCache::compilingShaderCount > 0)
            alpha += 0.1f;
        else
            alpha -= 0.1f;

        alpha = std::min(1.0f, std::max(0.0f, alpha));
        alphaVec[0] = alpha;
    }

    device->SetPixelShaderConstantF(150, alphaVec, 1);

    if (!texture)
    {
        vertexShader.Attach(new VertexShader(device->get(), (void*)g_compiling_shaders_image_vs_main, sizeof(g_compiling_shaders_image_vs_main)));
        pixelShader.Attach(new PixelShader(device->get(), (void*)g_compiling_shaders_image_ps_main, sizeof(g_compiling_shaders_image_ps_main)));

        ComPtr<ID3D11Resource> tex;
        ComPtr<ID3D11ShaderResourceView> srv;

        DirectX::CreateDDSTextureFromMemoryEx(device->get(), nullptr,
            compilingShadersImage, compilingShadersImageSize, 0, D3D11_USAGE_IMMUTABLE, D3D11_BIND_SHADER_RESOURCE, 0, 0, false, tex.GetAddressOf(), srv.GetAddressOf());

        texture.Attach(new Texture(device, tex.Get(), srv.Get()));
    }

    device->SetRenderTarget(0, device->swapChain.getRenderTargetSurface());
    device->SetDepthStencilSurface(nullptr);

    if (Configuration::compileShadersBeforeStarting)
        device->Clear(0, nullptr, D3DCLEAR_TARGET, 0, 1.0f, 0);

    device->SetPixelShader(pixelShader.Get());
    device->SetTexture(0, texture.Get());
    device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
    device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
    device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

    device->SetRenderState(D3DRS_ZENABLE, FALSE);
    device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    device->flush();

    const auto lock = device->lock();
    const auto context = device->getContext();

    ID3D11Buffer* buffers[] = { nullptr };
    UINT strides[] = { 0 };
    UINT offsets[] = { 0 };

    context->VSSetShader(vertexShader->getVertexShader(), nullptr, 0);
    context->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
    context->IASetVertexBuffers(0, 1, buffers, strides, offsets);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    context->IASetInputLayout(nullptr);
    context->Draw(4, 0);
}
