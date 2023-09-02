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

using namespace Settings;
using namespace Renderers::Modules;

namespace Renderer::Module
{
    extern "C" u32 RestoreVideoMode(void)
    {
        GL::Release();

        if (State.Settings.WindowMode == WindowOptionsMode::Off)
        {
            ChangeDisplaySettingsA(NULL, 0);
        }

        SHOWCURSOR(TRUE);

        return TRUE;
    }

    extern "C" u32 SelectVideoMode(const u32 width, const u32 height, const u32 bpp, u32 * *pixels)
    {
        State.Window.Width = width;
        State.Window.Height = height;
        State.Window.BitsPerPixel = bpp;

        Settings::ReadValue(RENDERER_MODULE_OPTIONS_GRAPHICS_SECTION_NAME, RENDERER_MODULE_OPTIONS_GRAPHICS_WINDOW_MODE_NAME, (u32*)&State.Settings.WindowMode);

        DEVMODEA dm
        {
            .dmSize = sizeof(DEVMODEA),
            .dmFields = DM_PELSHEIGHT | DM_PELSWIDTH | DM_BITSPERPEL,
            .dmBitsPerPel = State.Window.BitsPerPixel,
            .dmPelsWidth = State.Window.Width,
            .dmPelsHeight = State.Window.Height
        };

        if (ChangeDisplaySettingsA(&dm, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL) { Quit("Unable to change display settings."); }

        GL::Release();

        State.Window.DeviceContext = GetDC(State.Window.HWND);

        if (State.Window.DeviceContext == NULL) { Quit("Unable to acquire device context."); }

        PIXELFORMATDESCRIPTOR pfd =
        {
            .nSize = sizeof(PIXELFORMATDESCRIPTOR),
            .nVersion = 1,
            .dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER,
            .iPixelType = PFD_TYPE_RGBA,
            .cColorBits = (u8)State.Window.BitsPerPixel,
            .cDepthBits = (u8)State.Window.BitsPerPixel
        };

        const auto pf = ChoosePixelFormat(State.Window.DeviceContext, &pfd);

        DescribePixelFormat(State.Window.DeviceContext, pf, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

        if (pfd.dwFlags & PFD_GENERIC_FORMAT) { Quit("OpenGL acceleration is not supported."); }

        if (!SetPixelFormat(State.Window.DeviceContext, pf, &pfd)) { Quit("Unable to set pixel format."); }

        State.Window.RenderContext = wglCreateContext(State.Window.DeviceContext);

        if (State.Window.RenderContext == NULL) { Quit("Unable to create rendering context."); }

        wglMakeCurrent(State.Window.DeviceContext, State.Window.RenderContext);

        SHOWCURSOR(FALSE);

        SetWindowPos(State.Window.HWND, NULL, 0, 0, State.Window.Width, State.Window.Height, SWP_FRAMECHANGED);

        GL::Initialize();

        Clear();
        ClearZBuffer();
        Toggle();

        return TRUE;
    }
}