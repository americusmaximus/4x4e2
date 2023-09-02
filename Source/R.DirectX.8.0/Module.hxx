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

#include "DX.hxx"
#include "Settings.hxx"
#include "Renderers.Modules.Export.hxx"

#ifdef _WIN64
#define RENDERER_MODULE_TITLE_NAME "DirectX 8.0 (x64)"
#else
#define RENDERER_MODULE_TITLE_NAME "DirectX 8.0 (x32)"
#endif

namespace Renderer::Module
{
    struct RendererModuleState
    {
        Renderers::Modules::RendererModuleInitializeArguments InitializeArguments;

        DX::DXState DX;

        struct
        {
            Settings::WindowOptionsMode WindowMode;
            Settings::FixedPipelineOptionsMode FixedPipelineMode;

            u32 MipMapLevelCount = DEFAULT_OPTIONS_MIP_LEVEL_COUNT;
            u32 MipMapTextureSize;

            struct
            {
                u32 RTLVX = RENDERER_TEN_THOUSAND_ELEMENT_COUNT;
                u32 Index = RENDERER_TEN_THOUSAND_ELEMENT_COUNT;
            } Capacities;
        } Settings;

        struct
        {
            HWND HWND;

            u32 Width = MIN_GRAPHICS_RESOLUTION_WIDTH;
            u32 Height = MIN_GRAPHICS_RESOLUTION_HEIGHT;
            u32 BitsPerPixel = GRAPHICS_BITS_PER_PIXEL_32;
        } Window;

        struct
        {
            u32** Pixels;
            u32* Values[MAX_GRAPHICS_RESOLUTION_HEIGHT];
        } Pixels;
    };

    extern RendererModuleState State;
}