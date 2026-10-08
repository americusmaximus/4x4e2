#include "Textures.hxx"
#include "TextureCache.hxx"

namespace Renderer::DX11::Textures
{
    namespace
    {
        constexpr u32 MaxDimensions = 2048;

        u32 Stages[MaxStageCount] = { MaxCount, MaxCount, MaxCount, MaxCount,
            MaxCount, MaxCount, MaxCount, MaxCount };

        bool IsValid(const Renderers::RendererTexture* texture, const u32 dimensions)
        {
            return texture != nullptr
                && texture->Index < MaxCount
                && dimensions > 0
                && dimensions <= MaxDimensions;
        }

        u32 Store(const Renderers::RendererTexture* texture, const u32 dimensions,
            const bool transparent, const Source& source, const bool force)
        {
            if (!IsValid(texture, dimensions)) { return MaxCount; }

            const u32 indx = texture->Index;

            if (force || !Cache::Matches(indx, texture, dimensions, transparent))
            {
                if (!Cache::Allocate(indx, texture, dimensions, transparent)) { return MaxCount; }

                Cache::Upload(Cache::Texture(indx), source, dimensions);
            }

            return indx;
        }
    }

    u32 Select(const Renderers::RendererTexture* texture, const u32 dimensions,
        const bool transparent, const Source& source)
    {
        return Store(texture, dimensions, transparent, source, false);
    }

    u32 Update(const Renderers::RendererTexture* texture, const u32 dimensions,
        const bool transparent, const Source& source)
    {
        return Store(texture, dimensions, transparent, source, true);
    }

    void Bind(const u32 stage, const u32 indx)
    {
        if (stage < MaxStageCount) { Stages[stage] = indx; }
    }

    ID3D11ShaderResourceView* View(const u32 stage)
    {
        if (stage >= MaxStageCount || Stages[stage] >= MaxCount) { return nullptr; }

        return Cache::View(Stages[stage]);
    }

    void Reset()
    {
        Cache::ReleaseAll();

        for (u32 x = 0; x < MaxStageCount; x++) { Stages[x] = MaxCount; }
    }
}
