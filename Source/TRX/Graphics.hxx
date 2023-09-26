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
#include "Direct.Draw.hxx"

namespace Graphics
{
    struct GraphicsContainer
    {
        struct
        {
            s32* _Width = (s32*)0x00813e4c; // TODO
            s32* _Height = (s32*)0x00813e50; // TODO
        } View;

        struct
        {
            IDirectDraw** _Instance = (IDirectDraw**)0x00d65750; // TODO

            struct
            {
                u32** _Frame = (u32**)0x00682010; // TODO type, name
                u32** _Depth = (u32**)0x00d65760; // TODO
                u32** _DepthAligned = (u32**)0x006832d4; // TODO
            } Buffers;

            struct
            {
                IDirectDrawSurface** _Main = (IDirectDrawSurface**)0x00d65754; // TODO
                IDirectDrawSurface** _Back = (IDirectDrawSurface**)0x00d65758; // TODO
            } Surfaces;

            IDirectDrawPalette** _Palette = (IDirectDrawPalette**)0x00d6575c; // TODO
        } DX;
    };

    extern GraphicsContainer GraphicsState;

    void ReleaseGraphics(void);

    typedef const void(CDECLAPI* FUN_006029D0) (void); // TODO
    static FUN_006029D0 FUN_006029d0 = (FUN_006029D0)0x006029d0; // TODO

    typedef const void(CDECLAPI* FUN_004969B0) (f32, f32, f32, f32); // TODO
    static FUN_004969B0 FUN_004969b0 = (FUN_004969B0)0x004969b0; // TODO
}