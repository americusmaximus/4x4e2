#include "TextureCache.hxx"

#include <vector>

namespace Renderer::DX11::Textures::Cache
{
    namespace
    {
        constexpr u32 PaletteSize = 256;
        constexpr u32 OpaqueAlpha = 0xff;

        std::vector<u32> Scratch;

        void BuildPalette(const u8* colors, u32* palette)
        {
            for (u32 x = 0; x < PaletteSize; x++)
            {
                const u32 r = colors[x * 3 + 0];
                const u32 g = colors[x * 3 + 1];
                const u32 b = colors[x * 3 + 2];

                palette[x] = (r << 16) | (g << 8) | b;
            }
        }

        u32 PixelAlpha(const Source& source, const u32 x, const u32 color)
        {
            if (source.Alphas != nullptr) { return source.Alphas[x]; }

            return color == 0 ? 0 : OpaqueAlpha;
        }

        void ConvertIndexed(const Source& source, const u32 length)
        {
            u32 palette[PaletteSize];

            BuildPalette(source.Colors, palette);

            for (u32 x = 0; x < length; x++)
            {
                const u32 color = palette[source.Indexes[x]];

                Scratch[x] = (PixelAlpha(source, x, color) << 24) | color;
            }
        }

        bool Prepare(const Source& source, const u32 length)
        {
            if (source.Pixels != nullptr)
            {
                Scratch.assign(source.Pixels, source.Pixels + length);
                return true;
            }

            if (source.Indexes == nullptr || source.Colors == nullptr) { return false; }

            Scratch.assign(length, 0u);
            ConvertIndexed(source, length);

            return true;
        }
    }

    void Upload(ID3D11Texture2D* texture, const Source& source, const u32 dimensions)
    {
        const u32 length = dimensions * dimensions;

        if (texture == nullptr || !Prepare(source, length)) { return; }

        State.Context->UpdateSubresource(texture, 0, nullptr, Scratch.data(),
            dimensions * sizeof(u32), 0);
    }
}
