/*
Copyright (c) 2023 Americus Maximus

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "Mathematics.Basic.hxx"
#include "Module.hxx"
#include "Quit.hxx"
#include "Textures.hxx"

using namespace Mathematics;
using namespace Renderer::Module;
using namespace Renderers;

namespace DX
{
    u32* AcquireTexturePixels(const u32 dimensions)
    {
        u32 offset = 0;

        auto dims = State.DX.Surfaces.Dimensions;

        if (dims == dimensions) { return &State.DX.Pixels.Pixels[0]; }

        while (dimensions != dims)
        {
            offset = offset + dims * dims;

            dims = dims / 2;

            if (dims == 0) { Quit("Unable to acquire texture source size %d x %d.", dimensions, State.DX.Surfaces.Dimensions); }
        }

        return &State.DX.Pixels.Pixels[offset];
    }

    BOOL IsTextureActive(const RendererTexture* tex, const u32 dimensions, const BOOL opaque)
    {
        const auto indx = tex->Index;

        if (State.DX.Textures.Textures[indx].Dimensions != dimensions) { return FALSE; }

        if (State.DX.Textures.Textures[indx].IsOpaque != opaque) { return FALSE; }

        return strcmpi(State.DX.Textures.Textures[indx].Name, tex->Name) == 0;
    }

    void ConvertTexture(void)
    {
        const auto length = State.DX.Surfaces.Dimensions * State.DX.Surfaces.Dimensions;

        if (State.DX.Pixels.Colors == NULL)
        {
            for (u32 x = 0; x < MAX_PIXEL_PALETTE_BUFFER_SIZE; x++)
            {
                const u32 r = State.DX.Pixels.Palette.Colors[x * 3 + 0];
                const u32 g = State.DX.Pixels.Palette.Colors[x * 3 + 1];
                const u32 b = State.DX.Pixels.Palette.Colors[x * 3 + 2];

                State.DX.Pixels.Palette.Palette[x] = (r << 16) | (g << 8) | (b << 0);
            }

            if (State.DX.Pixels.Palette.Alphas == NULL)
            {
                for (u32 x = 0; x < length; x++)
                {
                    const auto indx = State.DX.Pixels.Palette.Indexes[x];

                    auto color = State.DX.Pixels.Palette.Palette[indx];

                    if (color != GRAPHICS_COLOR_BLACK) { color = 0xff000000 | color; }

                    State.DX.Pixels.Pixels[x] = color;
                }
            }
            else
            {
                for (u32 x = 0; x < length; x++)
                {
                    const auto indx = State.DX.Pixels.Palette.Indexes[x];
                    const auto alpha = ((u32)State.DX.Pixels.Palette.Alphas[x]) << 24;

                    State.DX.Pixels.Pixels[x] = alpha | State.DX.Pixels.Palette.Palette[indx];
                }
            }
        }
        else
        {
            for (u32 x = 0; x < length; x++)
            {
                State.DX.Pixels.Pixels[x] = State.DX.Pixels.Colors[x];
            }
        }

        if (*State.InitializeArguments.Mode == RendererAcceleratedGraphicsPortMode::Auto
            || (State.Settings.MipMapTextureSize & State.DX.Surfaces.Dimensions) != 0)
        {
            auto pixels = &State.DX.Pixels.Pixels[0];

            for (u32 dims = State.DX.Surfaces.Dimensions; 1 < dims; dims = dims / 2)
            {
                auto level = &pixels[dims * dims];
                const auto next = dims / 2;

                for (u32 x = 0; x < dims; x = x + 2)
                {
                    for (u32 xx = 0; xx < dims; xx = xx + 2)
                    {
                        const auto c00 = pixels[x * dims + xx];
                        const auto c01 = pixels[x * dims + (xx + 1)];
                        const auto c10 = pixels[(x + 1) * dims + xx];
                        const auto c11 = pixels[(x + 1) * dims + (xx + 1)];

                        const auto ca = ((c00 >> 24) + (c01 >> 24) + (c10 >> 24) + (c11 >> 24)) / 4;
                        const auto cr = ((c00 >> 16 & 0xff) + (c01 >> 16 & 0xff) + (c10 >> 16 & 0xff) + (c11 >> 16 & 0xff)) / 4;
                        const auto cg = ((c00 >> 8 & 0xff) + (c01 >> 8 & 0xff) + (c10 >> 8 & 0xff) + (c11 >> 8 & 0xff)) / 4;
                        const auto cb = ((c00 >> 0 & 0xff) + (c01 >> 0 & 0xff) + (c10 >> 0 & 0xff) + (c11 >> 0 & 0xff)) / 4;

                        level[((x * next) + xx) / 2] = (ca << 24) | (cr << 16) | (cg << 8) | (cb << 0);
                    }
                }

                pixels = level;
            }
        }
    }

    void CopyTexture16BitOpaque(u16* pixels, const u32 pitch, const u32 dimensions)
    {
        const auto stride = pitch / sizeof(u16);
        const auto px = AcquireTexturePixels(dimensions);

        for (u32 x = 0; x < dimensions; x++)
        {
            for (u32 xx = 0; xx < dimensions; xx++)
            {
                const auto color = px[x * dimensions + xx];

                const auto r = ((color >> 19) & 0x1f) << 11;
                const auto g = ((color >> 10) & 0x3f) << 5;
                const auto b = ((color >> 3) & 0x1f) << 0;

                pixels[x * stride + xx] = (u16)(r | g | b);
            }
        }
    }

    void CopyTexture16BitTransparent(u16* pixels, const u32 pitch, const u32 dimensions)
    {
        const auto stride = pitch / sizeof(u16);
        const auto px = AcquireTexturePixels(dimensions);

        for (u32 x = 0; x < dimensions; x++)
        {
            for (u32 xx = 0; xx < dimensions; xx++)
            {
                const auto color = px[x * dimensions + xx];

                const auto a = ((color >> 28) & 0xf) << 12;
                const auto r = ((color >> 20) & 0xf) << 8;
                const auto g = ((color >> 12) & 0xf) << 4;
                const auto b = ((color >> 4) & 0xf) << 0;

                pixels[x * stride + xx] = a | r | g | b;
            }
        }
    }

    void CopyTexture32Bit(u32* pixels, const u32 pitch, const u32 dimensions)
    {
        const auto stride = pitch / sizeof(u32);
        const auto px = AcquireTexturePixels(dimensions);

        for (u32 x = 0; x < dimensions; x++)
        {
            for (u32 xx = 0; xx < dimensions; xx++)
            {
                pixels[x * stride + xx] = px[x * dimensions + xx];
            }
        }
    }

    u32 SelectTexture(const RendererTexture* tex, const u32 dimensions, const BOOL transparent)
    {
        const auto indx = tex->Index;

        if (!IsTextureActive(tex, dimensions, !transparent))
        {
            CreateTexture(State.DX.Surfaces.Dimensions, !transparent, tex);
            UpdateTexture(indx);
        }

        return indx;
    }

    void UpdateTexture(const u32 indx)
    {
        State.DX.Textures.Surfaces.Staging[0] = State.DX.Textures.Textures[indx].Texture;

        UpdateTexture(&State.DX.Textures.Textures[indx]);
    }

    void UpdateTexture(DXTexture* tex)
    {
        if (State.DX.Pixels.Palette.Indexes == NULL && State.DX.Pixels.Colors == NULL) { return; }

        ConvertTexture();

        u32 mips = TEXTURE_MIP_MAP_COUNT_0;

        switch (State.DX.Surfaces.Dimensions)
        {
        case TEXTURE_DIMENSIONS_SIZE_32: { mips = TEXTURE_MIP_MAP_COUNT_5; break; }
        case TEXTURE_DIMENSIONS_SIZE_64: { mips = TEXTURE_MIP_MAP_COUNT_6; break; }
        case TEXTURE_DIMENSIONS_SIZE_128: { mips = TEXTURE_MIP_MAP_COUNT_7; break; }
        case TEXTURE_DIMENSIONS_SIZE_256: { mips = TEXTURE_MIP_MAP_COUNT_8; break; }
        case TEXTURE_DIMENSIONS_SIZE_512: { mips = TEXTURE_MIP_MAP_COUNT_9; break; }
        case TEXTURE_DIMENSIONS_SIZE_1024: { mips = TEXTURE_MIP_MAP_COUNT_10; break; }
        default: { Quit("Unable to acquire texture mip map count for dimensions %d.", State.DX.Surfaces.Dimensions); }
        }

        auto dims = State.DX.Surfaces.Dimensions;

        if (*State.InitializeArguments.Mode == RendererAcceleratedGraphicsPortMode::Auto)
        {
            dims = State.DX.Surfaces.Dimensions / 2;
            mips = mips - 1;
        }

        const auto lc = tex->Texture->GetLevelCount();

        for (u32 x = 0; x < lc; x++)
        {
            auto surface = tex->IsOpaque ? State.DX.Surfaces.Opaque[mips - x] : State.DX.Surfaces.Transparent[mips - x];

            D3DLOCKED_RECT rect;

            DXC(surface->LockRect(&rect, NULL, D3DLOCK_NOSYSLOCK), "Unable to lock source texture.");

            if (tex->IsOpaque)
            {
                CopyTexture16BitOpaque((u16*)rect.pBits, rect.Pitch, dims);
            }
            else
            {
                if (State.Window.BitsPerPixel == GRAPHICS_BITS_PER_PIXEL_32)
                {
                    CopyTexture32Bit((u32*)rect.pBits, rect.Pitch, dims);
                }
                else
                {
                    CopyTexture16BitTransparent((u16*)rect.pBits, rect.Pitch, dims);
                }
            }

            DXC(surface->UnlockRect(), "Unable to unlock source texture.");

            IDirect3DSurface9* level;
            DXC(tex->Texture->GetSurfaceLevel(x, &level), "Unable to acquire texture surface level.");

            DXC(State.DX.DirectX.Device->UpdateSurface(surface, NULL, level, NULL), "Unable to copy texture surface rectangle.");

            level->Release();

            dims = dims / 2;
        }
    }

    u32 UpdateTexture(const RendererTexture* tex, const u32 dimensions, const BOOL transparent)
    {
        if (!IsTextureActive(tex, dimensions, !transparent))
        {
            CreateTexture(State.DX.Surfaces.Dimensions, !transparent, tex);
        }

        const auto indx = tex->Index;

        UpdateTexture(indx);

        return indx;
    }

    void CreateTexture(const u32 dimensions, const BOOL opaque, const RendererTexture* tex)
    {
        const auto format = opaque ? State.DX.Surfaces.Formats.Opaque : State.DX.Surfaces.Formats.Transparent;
        const auto dims =
            *State.InitializeArguments.Mode == RendererAcceleratedGraphicsPortMode::Auto ? (dimensions / 2) : dimensions;

        u32 mips = TEXTURE_MIP_MAP_COUNT_1;

        if ((dimensions & State.Settings.MipMapTextureSize) != 0)
        {
            switch (dims)
            {
            case TEXTURE_DIMENSIONS_SIZE_16: { mips = Min<u32>(State.Settings.MipMapLevelCount, TEXTURE_MIP_MAP_COUNT_5); break; }
            case TEXTURE_DIMENSIONS_SIZE_32: { mips = Min<u32>(State.Settings.MipMapLevelCount, TEXTURE_MIP_MAP_COUNT_6); break; }
            case TEXTURE_DIMENSIONS_SIZE_64: { mips = Min<u32>(State.Settings.MipMapLevelCount, TEXTURE_MIP_MAP_COUNT_7); break; }
            case TEXTURE_DIMENSIONS_SIZE_128: { mips = Min<u32>(State.Settings.MipMapLevelCount, TEXTURE_MIP_MAP_COUNT_8); break; }
            case TEXTURE_DIMENSIONS_SIZE_256: { mips = Min<u32>(State.Settings.MipMapLevelCount, TEXTURE_MIP_MAP_COUNT_9); break; }
            case TEXTURE_DIMENSIONS_SIZE_512: { mips = Min<u32>(State.Settings.MipMapLevelCount, TEXTURE_MIP_MAP_COUNT_10); break; }
            case TEXTURE_DIMENSIONS_SIZE_1024: { mips = Min<u32>(State.Settings.MipMapLevelCount, TEXTURE_MIP_MAP_COUNT_11); break; }
            default: { Quit("Unable to acquire texture mip map count for dimensions %d.", State.DX.Surfaces.Dimensions); }
            }
        }

        const auto indx = tex->Index;

        if (State.DX.Textures.Textures[indx].Texture != NULL)
        {
            State.DX.Textures.Textures[indx].Texture->Release();
            State.DX.Textures.Textures[indx].Texture = NULL;
        }

        State.DX.Textures.Textures[indx].IsOpaque = opaque;
        State.DX.Textures.Textures[indx].Dimensions = dimensions;
        State.DX.Textures.Textures[indx].ActualDimensions = dims;
        State.DX.Textures.Textures[indx].Format = format;

        strcpy(State.DX.Textures.Textures[indx].Name, tex->Name);

        DXC(State.DX.DirectX.Device->CreateTexture(dims, dims, mips, D3DUSAGE_NONE, format,
            D3DPOOL::D3DPOOL_DEFAULT, &State.DX.Textures.Textures[indx].Texture, NULL),
            "Unable to create texture, please lower the texture detail settings, display resolution, or the bit depth.");
    }
}

namespace Renderer::Module
{
    extern "C" u32 AcquireTextureInformation(const u32 dimensions)
    {
        u32 count = 0;

        for (u32 x = 0; x < MAX_TEXTURE_COUNT; x++)
        {
            if (State.DX.Textures.Textures[x].Dimensions == dimensions) { count = count + 1; }
        }

        return count;
    }

    extern "C" u32 BeginCubeMapRender(const u32, const RendererCubeMapSide) { return FALSE; } // NOTE: Not being called by the application.

    extern "C" u32 BeginRenderToTexture(const u32) { return FALSE; }

    extern "C" u32 EndCubeMapRender(void) { return FALSE; } // NOTE: Not being called by the application.

    extern "C" u32 EndRenderToTexture(void) { return FALSE; }

    extern "C" u32 SelectCubeTexture(const u32, const u32) { return FALSE; } // NOTE: Not being called by the application.

    extern "C" u32 SelectRenderTexture(const u32 indx) { return FALSE; }

    extern "C" u32 SelectTexture(const RendererTexture * tex, const u32 dimensions, u8 * indexes, u8 * colors, u8 * alphas, const u32 indx)
    {
        State.DX.Pixels.Palette.Alphas = alphas;
        State.DX.Pixels.Palette.Colors = colors;
        State.DX.Pixels.Palette.Indexes = indexes;

        State.DX.Pixels.Colors = NULL;

        State.DX.Surfaces.Dimensions = dimensions;

        State.DX.Surfaces.Index = DX::SelectTexture(tex, dimensions, alphas != NULL);

        State.DX.Textures.Surfaces.Staging[indx] = State.DX.Textures.Textures[State.DX.Surfaces.Index].Texture;

        return TRUE;
    }

    extern "C" u32 SelectTextureBGRA(const RendererTexture * tex, const u32 dimensions, u32 * colors, const BOOL transparent, const u32 indx)
    {
        State.DX.Pixels.Palette.Alphas = NULL;
        State.DX.Pixels.Palette.Indexes = NULL;
        State.DX.Pixels.Palette.Colors = NULL;

        State.DX.Pixels.Colors = colors;

        State.DX.Surfaces.Dimensions = dimensions;

        State.DX.Surfaces.Index = DX::SelectTexture(tex, dimensions, transparent);

        State.DX.Textures.Surfaces.Staging[indx] = State.DX.Textures.Textures[State.DX.Surfaces.Index].Texture;

        return TRUE;
    }

    extern "C" u32 ResetTextureCache(void)
    {
        for (u32 x = 0; x < MAX_SURFACE_STAGE_COUNT; x++)
        {
            State.DX.DirectX.Device->SetTexture(x, NULL);
        }

        for (u32 x = 0; x < MAX_TEXTURE_COUNT; x++)
        {
            if (State.DX.Textures.Textures[x].Texture != NULL)
            {
                State.DX.Textures.Textures[x].Texture->Release();
                State.DX.Textures.Textures[x].Texture = NULL;
            }
        }

        ZeroMemory(&State.DX.Textures.Textures, MAX_TEXTURE_COUNT * sizeof(DX::DXTexture));

        return TRUE;
    }

    extern "C" u32 UpdateTexture(const RendererTexture * tex, const u32 dimensions, u8 * indexes, u8 * colors, u8 * alphas, const u32 indx)
    {
        State.DX.Pixels.Palette.Alphas = alphas;
        State.DX.Pixels.Palette.Colors = colors;
        State.DX.Pixels.Palette.Indexes = indexes;

        State.DX.Pixels.Colors = NULL;

        State.DX.Surfaces.Dimensions = dimensions;

        State.DX.Surfaces.Index = DX::UpdateTexture(tex, dimensions, alphas != NULL);

        State.DX.Textures.Surfaces.Staging[indx] = State.DX.Textures.Textures[State.DX.Surfaces.Index].Texture;

        return TRUE;
    }

    extern "C" u32 UpdateTextureBGRA(const RendererTexture * tex, const u32 dimensions, u32 * colors, const BOOL transparent, const u32 indx)
    {
        State.DX.Pixels.Palette.Alphas = NULL;
        State.DX.Pixels.Palette.Indexes = NULL;
        State.DX.Pixels.Palette.Colors = NULL;

        State.DX.Pixels.Colors = colors;

        State.DX.Surfaces.Dimensions = dimensions;

        State.DX.Surfaces.Index = DX::UpdateTexture(tex, dimensions, transparent);

        State.DX.Textures.Surfaces.Staging[indx] = State.DX.Textures.Textures[State.DX.Surfaces.Index].Texture;

        return TRUE;
    }
}