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

#pragma once

#include "Basic.hxx"
#include "Graphics.Basic.hxx"
#include "Mathematics.Basic.hxx"
#include "Native.Basic.hxx"
#include "Renderers.Basic.hxx"
#include "Renderers.Basic.Textures.hxx"
#include "Renderers.Basic.Vertexes.hxx"

#include <gl/GL.h>

#define MAX_TEXTURE_COUNT 2048
#define MAX_VERTEX_COUNT 5000

#define MAX_PIXEL_PALETTE_BUFFER_SIZE 256
#define MAX_PIXEL_BUFFER_SIZE (1024 * 1024 + 512 * 512 + 256 * 256 + 128 * 128 + 64 * 64 + 32 * 32 + 16 * 16 + 8 * 8 + 4 * 4 + 2 * 2 + 1 * 1)

#define GL_TEXTURE_MAX_LEVEL 0x813D

namespace GL
{
    struct GLTexture
    {
        u32 ID;
        char Name[MAX_TEXTURE_NAME_LENGTH];
    };


    struct GLState
    {
        struct
        {
            f32 Color = 0.5f;
            f32 Alpha = 1.0f;

            f32x3 Direction;
        } Light;

        struct
        {
            Renderers::RendererMode Mode;

            u32 DestinationBlendMode = GL_ONE_MINUS_SRC_ALPHA;
            u32 SourceBlendMode = GL_SRC_ALPHA;

            struct
            {
                f32 Far, Near;
            } Clipping;
        } Mode;

        struct
        {
            f32x3 XYZ[MAX_VERTEX_COUNT];
            Graphics::f32xRGBA RGBA[MAX_VERTEX_COUNT];
        } Data;

        struct
        {
            u32* Colors;

            struct
            {
                u8* Alphas;
                u8* Colors;
                u8* Indexes;

                u32 Palette[MAX_PIXEL_PALETTE_BUFFER_SIZE];
            } Palette;

            u32 Pixels[MAX_PIXEL_BUFFER_SIZE];
        } Pixels;

        struct
        {
            u32 Dimensions;

            GLTexture Textures[MAX_TEXTURE_COUNT];
        } Textures;
    };

    void Initialize(void);
    void Release(void);
    void SelectRendererMode(const Renderers::RendererMode mode);
}