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

using namespace Mathematics;
using namespace Renderers::Modules;
using namespace Renderers;
using namespace Settings;

namespace Renderer::Module
{
    extern "C" u32 RestoreVideoMode(void)
    {
        DX::Release();

        if (State.DX.DirectX.Device != NULL)
        {
            State.DX.DirectX.Device->Reset(&State.DX.Devices.Current.PresentParams);

            State.DX.DirectX.Device->Release();
            State.DX.DirectX.Device = NULL;
        }

        SHOWCURSOR(TRUE);

        return TRUE;
    }

    extern "C" u32 SelectVideoMode(const u32 width, const u32 height, const u32 bpp, u32 * *pixels)
    {
        State.Window.Width = width;
        State.Window.Height = height;
        State.Window.BitsPerPixel = bpp;

        State.DX.Mode.Mode = RendererMode::None;

        Settings::ReadValue(RENDERER_MODULE_OPTIONS_GRAPHICS_SECTION_NAME,
            RENDERER_MODULE_OPTIONS_GRAPHICS_WINDOW_MODE_NAME, (u32*)&State.Settings.WindowMode);
        Settings::ReadValue(RENDERER_MODULE_OPTIONS_GRAPHICS_SECTION_NAME,
            RENDERER_MODULE_OPTIONS_GRAPHICS_USE_FIXED_FUNCTION_PIPELINE_NAME, (u32*)&State.Settings.FixedPipelineMode);
        Settings::ReadValue(RENDERER_MODULE_OPTIONS_GRAPHICS_SECTION_NAME,
            RENDERER_MODULE_OPTIONS_GRAPHICS_MIP_LEVEL_COUNT_NAME, &State.Settings.MipMapLevelCount);

        Settings::ReadValue(RENDERER_MODULE_OPTIONS_GRAPHICS_SECTION_NAME,
            RENDERER_MODULE_OPTIONS_GRAPHICS_MAX_TRANSFORMED_LIGHT_VERTEX_COUNT_NAME, &State.Settings.Capacities.RTLVX);
        Settings::ReadValue(RENDERER_MODULE_OPTIONS_GRAPHICS_SECTION_NAME,
            RENDERER_MODULE_OPTIONS_GRAPHICS_MAX_INDEX_COUNT_NAME, &State.Settings.Capacities.Index);

        Settings::ReadValue(RENDERER_MODULE_OPTIONS_TEXTURES_SECTION_NAME,
            RENDERER_MODULE_OPTIONS_TEXTURES_MIP_MAP_MODE_NAME, &State.Settings.MipMapTextureSize);

        State.Pixels.Pixels = pixels;

        for (u32 x = 0; x < State.Window.Height; x++)
        {
            State.Pixels.Values[x] = pixels[x];
        }

        DX::Release();

        State.DX.DirectX.Instance = Direct3DCreate8(D3D_SDK_VERSION);

        if (State.DX.DirectX.Instance == NULL) { Quit("Unable to initialize DirectX."); }

        State.DX.DirectX.Instance->GetDeviceCaps(State.DX.Devices.Current.Index,
            D3DDEVTYPE::D3DDEVTYPE_HAL, &State.DX.Devices.Current.Capabilities);

        D3DDISPLAYMODE dm = { 0 };

        if (State.Settings.WindowMode == WindowOptionsMode::Off)
        {
            BOOL found = FALSE;
            const auto count = State.DX.DirectX.Instance->GetAdapterModeCount(State.DX.Devices.Current.Index);

            for (u32 x = 0; x < count; x++)
            {
                DX::DXC(State.DX.DirectX.Instance->EnumAdapterModes(State.DX.Devices.Current.Index, x, &dm),
                    "Unable to enumerate device modes.");

                if (dm.Width == State.Window.Width && dm.Height == State.Window.Height)
                {
                    if (State.Window.BitsPerPixel == GRAPHICS_BITS_PER_PIXEL_16)
                    {
                        if (dm.Format == D3DFORMAT::D3DFMT_R5G6B5) { found = TRUE; break; }
                    }
                    else if (dm.Format == D3DFORMAT::D3DFMT_R8G8B8 || dm.Format == D3DFORMAT::D3DFMT_A8R8G8B8 || dm.Format == D3DFORMAT::D3DFMT_X8R8G8B8) { found = TRUE; break; }
                }
            }

            if (!found) { Quit("Unable to find a valid video mode for %d x %d.", State.Window.Width, State.Window.Height); }
        }
        else
        {
            DX::DXC(State.DX.DirectX.Instance->GetAdapterDisplayMode(State.DX.Devices.Current.Index, &dm),
                "Unable to acquire display mode for window mode.");
        }

        auto stencilFormat = State.Window.BitsPerPixel == GRAPHICS_BITS_PER_PIXEL_16
            ? D3DFORMAT::D3DFMT_D16 : D3DFORMAT::D3DFMT_D32;

        if (State.Window.BitsPerPixel != GRAPHICS_BITS_PER_PIXEL_16)
        {
            auto result = State.DX.DirectX.Instance->CheckDeviceFormat(State.DX.Devices.Current.Index, D3DDEVTYPE::D3DDEVTYPE_HAL,
                dm.Format, D3DUSAGE_DEPTHSTENCIL, D3DRESOURCETYPE::D3DRTYPE_SURFACE, D3DFORMAT::D3DFMT_D32);

            if (result != D3D_OK)
            {
                stencilFormat = D3DFORMAT::D3DFMT_D24S8;

                result = State.DX.DirectX.Instance->CheckDeviceFormat(State.DX.Devices.Current.Index, D3DDEVTYPE::D3DDEVTYPE_HAL,
                    dm.Format, D3DUSAGE_DEPTHSTENCIL, D3DRESOURCETYPE::D3DRTYPE_SURFACE, D3DFORMAT::D3DFMT_D24S8);

                if (result != D3D_OK)
                {
                    stencilFormat = D3DFORMAT::D3DFMT_D16;

                    DX::DXC(State.DX.DirectX.Instance->CheckDeviceFormat(State.DX.Devices.Current.Index, D3DDEVTYPE::D3DDEVTYPE_HAL,
                        dm.Format, D3DUSAGE_DEPTHSTENCIL, D3DRESOURCETYPE::D3DRTYPE_SURFACE, D3DFORMAT::D3DFMT_D16),
                        "Unable to find depth buffer format.");
                }
            }
        }

        ZeroMemory(&State.DX.Devices.Current.PresentParams, sizeof(D3DPRESENT_PARAMETERS));

        State.DX.Devices.Current.PresentParams.AutoDepthStencilFormat = stencilFormat;
        State.DX.Devices.Current.PresentParams.BackBufferFormat = dm.Format;
        State.DX.Devices.Current.PresentParams.EnableAutoDepthStencil = TRUE;
        State.DX.Devices.Current.PresentParams.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
        State.DX.Devices.Current.PresentParams.hDeviceWindow = State.Window.HWND;
        State.DX.Devices.Current.PresentParams.MultiSampleType = D3DMULTISAMPLE_TYPE::D3DMULTISAMPLE_NONE;
        State.DX.Devices.Current.PresentParams.Windowed = State.Settings.WindowMode == WindowOptionsMode::On;

        if (State.Settings.WindowMode == WindowOptionsMode::Off)
        {
            State.DX.Devices.Current.PresentParams.BackBufferCount = 2;
            State.DX.Devices.Current.PresentParams.BackBufferHeight = State.Window.Height;
            State.DX.Devices.Current.PresentParams.BackBufferWidth = State.Window.Width;
            State.DX.Devices.Current.PresentParams.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
            State.DX.Devices.Current.PresentParams.FullScreen_RefreshRateInHz = dm.RefreshRate;
            State.DX.Devices.Current.PresentParams.SwapEffect = D3DSWAPEFFECT::D3DSWAPEFFECT_FLIP;
        }
        else
        {
            State.DX.Devices.Current.PresentParams.BackBufferCount = 1;
            State.DX.Devices.Current.PresentParams.BackBufferHeight = 0;
            State.DX.Devices.Current.PresentParams.BackBufferWidth = 0;
            State.DX.Devices.Current.PresentParams.FullScreen_PresentationInterval = 0;
            State.DX.Devices.Current.PresentParams.FullScreen_RefreshRateInHz = 0;
            State.DX.Devices.Current.PresentParams.SwapEffect = D3DSWAPEFFECT::D3DSWAPEFFECT_COPY;
        }

        if (State.DX.DirectX.Instance->CreateDevice(State.DX.Devices.Current.Index, D3DDEVTYPE::D3DDEVTYPE_HAL,
            State.Window.HWND, D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_FPU_PRESERVE,
            &State.DX.Devices.Current.PresentParams, &State.DX.DirectX.Device) != D3D_OK)
        {
            Quit("Unable to create DirectX software device.");
        }

        DX::Initialize();

        SetCursorPos(State.Window.Width - 1, State.Window.Height - 1);

        SHOWCURSOR(FALSE);

        Clear();
        Toggle();

        return TRUE;
    }

    extern "C" u32 VideoRestore(void)
    {
        DX::DXC(State.DX.DirectX.Device->Reset(&State.DX.Devices.Current.PresentParams), "Unable to reset device.");

        DX::Initialize();

        return TRUE;
    }

    extern "C" u32 VideoSave(void)
    {
        DX::Release();

        return TRUE;
    }
}