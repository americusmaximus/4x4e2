#pragma once

#include "DX.hxx"

namespace Renderer::DX11::Draw
{
    bool Create(u32 width, u32 height);
    void Release();
    void Bind();
    void ClearDepth();

    u32 PolyList(const Renderers::RCVX* vertexes, u32 count, const u16* indexes,
        u32 indexCount, Renderers::RendererMode mode);

    void SetSourceBlend(Renderers::RendererSourceBlendMode mode);
    void SetDestinationBlend(Renderers::RendererDestinationBlend mode);
    void SetCulling(Renderers::RendererCullMode mode);
    void SetAlphaRef(u32 value);
    void SetFogColor(f32 r, f32 g, f32 b);
    void SetClamp(u32 stage, Renderers::RendererClampMode u, Renderers::RendererClampMode v);
}
