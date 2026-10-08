#include "DrawState.hxx"

namespace Renderer::DX11::Draw
{
    bool CreateDepth(const u32 width, const u32 height)
    {
        D3D11_TEXTURE2D_DESC desc = {};

        desc.Width = width;
        desc.Height = height;
        desc.MipLevels = 1;
        desc.ArraySize = 1;
        desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        desc.SampleDesc.Count = 1;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

        if (FAILED(State.Device->CreateTexture2D(&desc, nullptr, &Objects.DepthTexture)))
        {
            return false;
        }

        if (FAILED(State.Device->CreateDepthStencilView(Objects.DepthTexture, nullptr, &Objects.DepthView)))
        {
            ReleaseDepth();
            return false;
        }

        return true;
    }

    void ReleaseDepth()
    {
        if (Objects.DepthView != nullptr) { Objects.DepthView->Release(); Objects.DepthView = nullptr; }
        if (Objects.DepthTexture != nullptr) { Objects.DepthTexture->Release(); Objects.DepthTexture = nullptr; }
    }
}
