#include "Surface.hxx"
#include "Trace.hxx"

#include <vector>

namespace Renderer::DX11::Surface
{
    namespace
    {
        struct SurfaceState
        {
            u32** Rows = nullptr;
            std::vector<u32*> SavedRows;
            std::vector<u32> Pixels;
            ID3D11Texture2D* Texture = nullptr;
            bool Locked = false;
        };

        SurfaceState Surface;
    }

    bool Create(const u32 width, const u32 height, u32** rows)
    {
        Trace::Call("SurfaceCreate", width, height);

        ReleaseTexture();

        if (rows == nullptr) { return false; }

        D3D11_TEXTURE2D_DESC desc = {};
        desc.Width = width;
        desc.Height = height;
        desc.MipLevels = 1;
        desc.ArraySize = 1;
        desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        desc.SampleDesc.Count = 1;
        desc.Usage = D3D11_USAGE_DEFAULT;

        if (FAILED(State.Device->CreateTexture2D(&desc, nullptr, &Surface.Texture))) { return false; }

        Surface.Rows = rows;
        Surface.SavedRows.assign(rows, rows + height);
        Surface.Pixels.assign(static_cast<size_t>(width) * height, 0u);

        return true;
    }

    void ReleaseTexture()
    {
        if (Surface.Texture != nullptr) { Surface.Texture->Release(); Surface.Texture = nullptr; }

        Surface.Locked = false;
    }

    u32 Lock()
    {
        Trace::Call("LockFrame");

        if (Surface.Rows == nullptr || Surface.Texture == nullptr) { return FALSE; }

        if (!Surface.Locked)
        {
            const u32 width = State.Width;

            for (u32 y = 0; y < State.Height; y++)
            {
                Surface.Rows[y] = Surface.Pixels.data() + static_cast<size_t>(y) * width;
            }

            Surface.Locked = true;
        }

        return TRUE;
    }

    u32 Unlock()
    {
        Trace::Call("UnlockFrame");

        if (!Surface.Locked) { return FALSE; }

        Surface.Locked = false;

        for (u32 y = 0; y < State.Height; y++)
        {
            Surface.Rows[y] = Surface.SavedRows[y];
        }

        if (State.SwapChain == nullptr || State.Context == nullptr) { return FALSE; }

        ID3D11Texture2D* back = nullptr;

        if (FAILED(State.SwapChain->GetBuffer(0, IID_PPV_ARGS(&back)))) { return FALSE; }

        State.Context->UpdateSubresource(Surface.Texture, 0, nullptr,
            Surface.Pixels.data(), State.Width * sizeof(u32), 0);
        State.Context->CopyResource(back, Surface.Texture);

        back->Release();

        return TRUE;
    }
}
