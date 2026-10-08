#pragma once

#include "Textures.hxx"

namespace Renderer::DX11::Textures::Cache
{
    bool Matches(u32 indx, const Renderers::RendererTexture* texture, u32 dimensions, bool transparent);
    bool Allocate(u32 indx, const Renderers::RendererTexture* texture, u32 dimensions, bool transparent);
    void Release(u32 indx);
    void ReleaseAll();
    ID3D11Texture2D* Texture(u32 indx);
    ID3D11ShaderResourceView* View(u32 indx);
    void Upload(ID3D11Texture2D* texture, const Source& source, u32 dimensions);
}
