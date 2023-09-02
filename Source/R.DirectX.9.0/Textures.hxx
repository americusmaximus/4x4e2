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
#include "Renderers.Basic.hxx"

namespace DX
{
    BOOL IsTextureActive(const Renderers::RendererTexture* tex, const u32 dimensions, const BOOL opaque);
    u32 SelectTexture(const Renderers::RendererTexture* tex, const u32 dimensions, const BOOL transparent);
    u32 UpdateTexture(const Renderers::RendererTexture* tex, const u32 dimensions, const BOOL transparent);
    u32* AcquireTexturePixels(const u32 dimensions);
    void ConvertTexture(void);
    void CopyTexture16BitOpaque(u16* pixels, const u32 pitch, const u32 dimensions);
    void CopyTexture16BitTransparent(u16* pixels, const u32 pitch, const u32 dimensions);
    void CopyTexture32Bit(u32* pixels, const u32 pitch, const u32 dimensions);
    void CreateTexture(const u32 dimensions, const BOOL opaque, const Renderers::RendererTexture* tex);
    void UpdateTexture(const u32 indx);
    void UpdateTexture(DXTexture* tex);
}