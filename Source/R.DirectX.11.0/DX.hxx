#pragma once

#include "Basic.hxx"
#include "Native.Basic.hxx"
#include "Renderers.Modules.Export.hxx"

#include <d3d11.h>
#include <dxgi.h>

#define RENDERER_MODULE_TITLE_NAME "DirectX 11.0 (x32)"

namespace Renderer::DX11
{
    constexpr u32 MaxAdapterCount = 16;
    constexpr u32 MaxAdapterNameLength = 128;

    struct AdapterInformation
    {
        char Description[MaxAdapterNameLength];
        u32 VendorId;
        u32 DeviceId;
    };

    struct DeviceState
    {
        HWND Window = nullptr;

        IDXGIFactory1* Factory = nullptr;
        u32 AdapterCount = 0;
        AdapterInformation Adapters[MaxAdapterCount];
        u32 SelectedAdapter = 0;

        ID3D11Device* Device = nullptr;
        ID3D11DeviceContext* Context = nullptr;
        IDXGISwapChain* SwapChain = nullptr;
        ID3D11RenderTargetView* BackBuffer = nullptr;

        u32 Width = 0;
        u32 Height = 0;
        bool SceneActive = false;
    };

    extern DeviceState State;

    bool EnumerateAdapters();
    bool CreateDevice(u32 width, u32 height);
    void ReleaseDevice();
    void ReleaseAll();
}
