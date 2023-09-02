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

#include "Module.hxx"
#include "Quit.hxx"
#include "Textures.hxx"

#include <stdlib.h>

using namespace Renderer::Module;
using namespace Renderers;

namespace GL
{
    u32* AcquireTexturePixels(const u32 dimensions)
    {
        u32 offset = 0;

        auto dims = State.GL.Textures.Dimensions;

        if (dims == dimensions) { return &State.GL.Pixels.Pixels[0]; }

        while (dimensions != dims)
        {
            offset = offset + dims * dims;

            dims = dims / 2;

            if (dims == 0)
            {
                Quit("Unable to acquire texture source size %d x %d.", dimensions, State.GL.Textures.Dimensions);
            }
        }

        return &State.GL.Pixels.Pixels[offset];
    }

    BOOL BindTexture(const Renderers::RendererTexture* tex)
    {
        if (strcmpi(tex->Name, State.GL.Textures.Textures[tex->Index].Name) == 0)
        {
            glBindTexture(GL_TEXTURE_2D, State.GL.Textures.Textures[tex->Index].ID);

            return TRUE;
        }

        return FALSE;
    }

    void ConvertTexture(void)
    {
        const auto length = State.GL.Textures.Dimensions * State.GL.Textures.Dimensions;

        if (State.GL.Pixels.Colors == NULL)
        {
            for (u32 x = 0; x < MAX_PIXEL_PALETTE_BUFFER_SIZE; x++)
            {
                State.GL.Pixels.Palette.Palette[x] =
                    ((u32)State.GL.Pixels.Palette.Colors[x * 3 + 0] << 16)
                    | ((u32)State.GL.Pixels.Palette.Colors[x * 3 + 1] << 8)
                    | ((u32)State.GL.Pixels.Palette.Colors[x * 3 + 2] << 0);
            }

            if (State.GL.Pixels.Palette.Alphas == NULL)
            {
                for (u32 x = 0; x < length; x++)
                {
                    const auto indx = State.GL.Pixels.Palette.Indexes[x];

                    auto color = State.GL.Pixels.Palette.Palette[indx];

                    if (color != GRAPHICS_COLOR_BLACK) { color = color | 0xff000000; }

                    State.GL.Pixels.Pixels[x] = color;
                }
            }
            else
            {
                for (u32 x = 0; x < length; x++)
                {
                    const auto indx = State.GL.Pixels.Palette.Indexes[x];
                    const auto alpha = ((u32)State.GL.Pixels.Palette.Alphas[x]) << 24;

                    State.GL.Pixels.Pixels[x] = alpha | State.GL.Pixels.Palette.Palette[indx];
                }
            }
        }
        else
        {
            for (u32 x = 0; x < length; x++)
            {
                State.GL.Pixels.Pixels[x] = State.GL.Pixels.Colors[x];
            }
        }

        // Generate down-sized texture pixels.
        if ((State.GL.Textures.Dimensions & State.Settings.MipMapTextureSize) != 0
            || *State.InitializeArguments.Mode == RendererAcceleratedGraphicsPortMode::Auto)
        {
            auto pixels = &State.GL.Pixels.Pixels[0];

            for (u32 dims = State.GL.Textures.Dimensions; 1 < dims; dims = dims / 2)
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

    void UpdateTexture(const RendererTexture* tex)
    {
        strcpy(State.GL.Textures.Textures[tex->Index].Name, tex->Name);

        ConvertTexture();

        glGenTextures(1, &State.GL.Textures.Textures[tex->Index].ID);
        glBindTexture(GL_TEXTURE_2D, State.GL.Textures.Textures[tex->Index].ID);

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

        auto dims = *State.InitializeArguments.Mode == RendererAcceleratedGraphicsPortMode::Auto
            ? (State.GL.Textures.Dimensions / 2) : State.GL.Textures.Dimensions;

        const u32 levels = *State.InitializeArguments.Mode == RendererAcceleratedGraphicsPortMode::Auto
            ? TEXTURE_MIP_MAP_COUNT_1 : TEXTURE_MIP_MAP_COUNT_2;

        for (u32 x = 0; x < (levels + 1); x++)
        {
            if (dims == 0) { break; }

            const auto pixels = AcquireTexturePixels(dims);

            glTexImage2D(GL_TEXTURE_2D, x, GL_RGBA8, dims, dims, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, pixels);

            dims = dims / 2;
        }

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, levels);
    }
}

namespace Renderer::Module
{
    extern "C" u32 AcquireTextureInformation(const u32) { return DEFAULT_TEXTURE_CACHE_CAPACITY; }

    extern "C" u32 AcquireTextureHandle(void) { return DEFAULT_TEXTURE_HANDLE; } // NOTE: Not being called by the application.

    extern "C" u32 SelectTexture(const RendererTexture * tex, const u32 dimensions, u8 * indexes, u8 * colors, u8 * alphas, const u32 indx)
    {
        State.GL.Pixels.Palette.Alphas = alphas;
        State.GL.Pixels.Palette.Colors = colors;
        State.GL.Pixels.Palette.Indexes = indexes;

        State.GL.Pixels.Colors = NULL;

        State.GL.Textures.Dimensions = dimensions;

        if (!GL::BindTexture(tex)) { GL::UpdateTexture(tex); }

        return TRUE;
    }

    extern "C" u32 SelectTextureBGRA(const RendererTexture * tex, const u32 dimensions, u32 * colors, const BOOL transparent, const u32 indx)
    {
        State.GL.Pixels.Palette.Alphas = NULL;
        State.GL.Pixels.Palette.Indexes = NULL;
        State.GL.Pixels.Palette.Colors = NULL;

        State.GL.Pixels.Colors = colors;

        State.GL.Textures.Dimensions = dimensions;

        if (!GL::BindTexture(tex)) { GL::UpdateTexture(tex); }

        return TRUE;
    }

    extern "C" u32 ResetTextureCache(void)
    {
        for (u32 x = 0; x < MAX_TEXTURE_COUNT; x++)
        {
            if (State.GL.Textures.Textures[x].ID != INVALID_TEXTURE_ID)
            {
                glDeleteTextures(1, &State.GL.Textures.Textures[x].ID);
            }
        }

        ZeroMemory(&State.GL.Textures, MAX_TEXTURE_COUNT * sizeof(GL::GLTexture));

        return TRUE;
    }

    extern "C" u32 UpdateTexture(const RendererTexture * tex, const u32 dimensions, u8 * indexes, u8 * colors, u8 * alphas, const u32 indx)
    {
        State.GL.Pixels.Palette.Alphas = alphas;
        State.GL.Pixels.Palette.Colors = colors;
        State.GL.Pixels.Palette.Indexes = indexes;

        State.GL.Pixels.Colors = NULL;

        State.GL.Textures.Dimensions = dimensions;

        GL::UpdateTexture(tex);

        return TRUE;
    }

    extern "C" u32 UpdateTextureBGRA(const RendererTexture * tex, const u32 dimensions, u32 * colors, const BOOL transparent, const u32 indx)
    {
        State.GL.Pixels.Palette.Alphas = NULL;
        State.GL.Pixels.Palette.Indexes = NULL;
        State.GL.Pixels.Palette.Colors = NULL;

        State.GL.Pixels.Colors = colors;

        State.GL.Textures.Dimensions = dimensions;

        GL::UpdateTexture(tex);

        return TRUE;
    }
}