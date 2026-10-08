#include "DrawState.hxx"

namespace Renderer::DX11::Draw
{
    void SetSourceBlend(const Renderers::RendererSourceBlendMode mode)
    {
        Current.SourceBlend = mode;
    }

    void SetDestinationBlend(const Renderers::RendererDestinationBlend mode)
    {
        Current.DestinationBlend = mode;
    }

    void SetCulling(const Renderers::RendererCullMode mode)
    {
        Current.Cull = mode;
    }

    void SetAlphaRef(const u32 value)
    {
        Current.AlphaRef = value;
    }

    void SetFogColor(const f32 r, const f32 g, const f32 b)
    {
        Current.FogColor[0] = r;
        Current.FogColor[1] = g;
        Current.FogColor[2] = b;
    }

    void SetClamp(const u32 stage, const Renderers::RendererClampMode u,
        const Renderers::RendererClampMode v)
    {
        if (stage >= MaxClampStageCount) { return; }

        Current.ClampU[stage] = u;
        Current.ClampV[stage] = v;
    }
}
