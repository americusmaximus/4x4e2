#pragma once

#include "DX.hxx"

namespace Renderer::DX11::Textures
{
    constexpr u32 MaxCount = 4096;
    constexpr u32 MaxStageCount = 8;

    struct Source
    {
        const u8* Indexes = nullptr;
        const u8* Colors = nullptr;
        const u8* Alphas = nullptr;
        const u32* Pixels = nullptr;
    };

    u32 Select(const Renderers::RendererTexture* texture, u32 dimensions, bool transparent, const Source& source);
    u32 Update(const Renderers::RendererTexture* texture, u32 dimensions, bool transparent, const Source& source);
    void Bind(u32 stage, u32 indx);
    ID3D11ShaderResourceView* View(u32 stage);
    void Reset();
}
