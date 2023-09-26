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

#include "Graphics.hxx"
#include "Memory.hxx"
#include "Renderer.hxx"

using namespace Memory;
using namespace Renderer;

namespace Graphics
{
    GraphicsContainer GraphicsState;

    // 0x00601b70
    void ReleaseGraphics(void)
    {
        if (*GraphicsState.DX.Buffers._Frame != NULL)
        {
            ReleaseMemory1(*GraphicsState.DX.Buffers._Frame);
            *GraphicsState.DX.Buffers._Frame = NULL;
        }

        if (*GraphicsState.DX.Buffers._Depth != NULL)
        {
            ReleaseMemory1(*GraphicsState.DX.Buffers._Depth);
            *GraphicsState.DX.Buffers._DepthAligned = NULL;
        }

        ReleaseRenderer();

        if (*GraphicsState.DX._Palette != NULL)
        {
            (*GraphicsState.DX._Palette)->Release();
            *GraphicsState.DX._Palette = NULL;
        }

        if (*GraphicsState.DX.Surfaces._Back != NULL)
        {
            (*GraphicsState.DX.Surfaces._Back)->Release();
            *GraphicsState.DX.Surfaces._Back = NULL;
        }

        if (*GraphicsState.DX.Surfaces._Main != NULL)
        {
            (*GraphicsState.DX.Surfaces._Main)->Release();
            *GraphicsState.DX.Surfaces._Main = NULL;
        }

        if (*GraphicsState.DX._Instance != NULL)
        {
            (*GraphicsState.DX._Instance)->RestoreDisplayMode();
            (*GraphicsState.DX._Instance)->Release();

            *GraphicsState.DX._Instance = NULL;
        }
    }
}