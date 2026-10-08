#include "TextureCache.hxx"
#include "Trace.hxx"

#include <cstring>

namespace Renderer::DX11::Textures::Cache
{
    namespace
    {
        struct Slot
        {
            ID3D11Texture2D* Texture = nullptr;
            ID3D11ShaderResourceView* View = nullptr;
            u32 Dimensions = 0;
            bool Transparent = false;
            char Name[MAX_TEXTURE_NAME_LENGTH] = {};
        };

        Slot Slots[MaxCount];

        void FreeSlot(Slot& slot)
        {
            if (slot.View != nullptr) { slot.View->Release(); slot.View = nullptr; }
            if (slot.Texture != nullptr) { slot.Texture->Release(); slot.Texture = nullptr; }

            slot.Dimensions = 0;
            slot.Transparent = false;
        }

        D3D11_TEXTURE2D_DESC Describe(const u32 dimensions)
        {
            D3D11_TEXTURE2D_DESC desc = {};

            desc.Width = dimensions;
            desc.Height = dimensions;
            desc.MipLevels = 1;
            desc.ArraySize = 1;
            desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
            desc.SampleDesc.Count = 1;
            desc.Usage = D3D11_USAGE_DEFAULT;
            desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

            return desc;
        }

        void CopyName(Slot& slot, const Renderers::RendererTexture* texture)
        {
            strncpy(slot.Name, texture->Name, sizeof(slot.Name) - 1);
            slot.Name[sizeof(slot.Name) - 1] = '\0';
        }
    }

    bool Matches(const u32 indx, const Renderers::RendererTexture* texture,
        const u32 dimensions, const bool transparent)
    {
        const Slot& slot = Slots[indx];

        return slot.Texture != nullptr
            && slot.Dimensions == dimensions
            && slot.Transparent == transparent
            && _stricmp(slot.Name, texture->Name) == 0;
    }

    bool Allocate(const u32 indx, const Renderers::RendererTexture* texture,
        const u32 dimensions, const bool transparent)
    {
        Slot& slot = Slots[indx];

        FreeSlot(slot);

        const D3D11_TEXTURE2D_DESC desc = Describe(dimensions);

        if (FAILED(State.Device->CreateTexture2D(&desc, nullptr, &slot.Texture)))
        {
            Trace::Call("CreateTexture2D failed", dimensions);
            return false;
        }

        if (FAILED(State.Device->CreateShaderResourceView(slot.Texture, nullptr, &slot.View)))
        {
            FreeSlot(slot);
            return false;
        }

        slot.Dimensions = dimensions;
        slot.Transparent = transparent;
        CopyName(slot, texture);

        return true;
    }

    void Release(const u32 indx)
    {
        FreeSlot(Slots[indx]);
    }

    void ReleaseAll()
    {
        for (u32 x = 0; x < MaxCount; x++) { FreeSlot(Slots[x]); }
    }

    ID3D11Texture2D* Texture(const u32 indx)
    {
        return Slots[indx].Texture;
    }

    ID3D11ShaderResourceView* View(const u32 indx)
    {
        return Slots[indx].View;
    }
}
