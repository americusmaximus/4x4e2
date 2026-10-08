#include "Textures.hxx"
#include "Trace.hxx"

namespace Renderers::Modules
{
    using Renderer::DX11::Textures::Bind;
    using Renderer::DX11::Textures::Select;
    using Renderer::DX11::Textures::Source;
    using Renderer::DX11::Textures::Update;

    extern "C" u32 ResetTextureCache(void)
    {
        Renderer::DX11::Trace::Call("ResetTextureCache");

        Renderer::DX11::Textures::Reset();

        return TRUE;
    }

    extern "C" u32 SelectTexture(const RendererTexture* texture, const u32 dimensions,
        u8* indexes, u8* colors, u8* alphas, const u32 indx)
    {
        const Source source = { indexes, colors, alphas, nullptr };

        Bind(indx, Select(texture, dimensions, alphas != nullptr, source));

        return TRUE;
    }

    extern "C" u32 SelectTextureBGRA(const RendererTexture* texture, const u32 dimensions,
        u32* colors, const BOOL transparent, const u32 indx)
    {
        const Source source = { nullptr, nullptr, nullptr, colors };

        Bind(indx, Select(texture, dimensions, transparent != FALSE, source));

        return TRUE;
    }

    extern "C" u32 UpdateTexture(const RendererTexture* texture, const u32 dimensions,
        u8* indexes, u8* colors, u8* alphas, const u32 indx)
    {
        const Source source = { indexes, colors, alphas, nullptr };

        Bind(indx, Update(texture, dimensions, alphas != nullptr, source));

        return TRUE;
    }

    extern "C" u32 UpdateTextureBGRA(const RendererTexture* texture, const u32 dimensions,
        u32* colors, const BOOL transparent, const u32 indx)
    {
        const Source source = { nullptr, nullptr, nullptr, colors };

        Bind(indx, Update(texture, dimensions, transparent != FALSE, source));

        return TRUE;
    }
}
