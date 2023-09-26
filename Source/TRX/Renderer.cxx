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

#include "Renderer.hxx"

using namespace Renderer::Module;

namespace Renderer
{
    RendererContainer RendererState;

    // 0x005c7700
    BOOL SelectRendererGraphicsCard(const u32 indx)
    {
        if (!*RendererModule.State._IsActive) { return FALSE; }

        *RendererModule.State._SelectedDeviceIndex = indx;

        return (*RendererModule._SelectGraphicsCard)(indx);
    }

    // 0x005c7260
    BOOL ReleaseRenderer(void)
    {
        if (!*RendererModule.State._IsActive) { return TRUE; }

        (*RendererModule._Kill)();

        ReleaseRendererModule();

        return TRUE;
    }

    // 0x005c7370
    BOOL SetRendererDirtyRect(const s32 x, const s32 y, const s32 width, const s32 height)
    {
        if (RendererModule.SetDirtyRect == NULL) { return TRUE; }

        RendererModule.SetDirtyRect(x, y, width, height);

        return TRUE;
    }
}