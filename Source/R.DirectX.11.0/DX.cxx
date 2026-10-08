#include "DX.hxx"
#include "Draw.hxx"
#include "Surface.hxx"
#include "Textures.hxx"

namespace Renderer::DX11
{
    DeviceState State;

    namespace
    {
        constexpr D3D_FEATURE_LEVEL FeatureLevels[] = {
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_10_1,
            D3D_FEATURE_LEVEL_10_0,
        };

        bool CreateBackBuffer()
        {
            ID3D11Texture2D* buffer = nullptr;

            if (FAILED(State.SwapChain->GetBuffer(0, IID_PPV_ARGS(&buffer)))) { return false; }

            const HRESULT result = State.Device->CreateRenderTargetView(buffer, nullptr, &State.BackBuffer);

            buffer->Release();

            if (FAILED(result)) { return false; }

            State.Context->OMSetRenderTargets(1, &State.BackBuffer, nullptr);

            const D3D11_VIEWPORT viewport = {
                0.f, 0.f, static_cast<float>(State.Width), static_cast<float>(State.Height), 0.f, 1.f };

            State.Context->RSSetViewports(1, &viewport);

            return true;
        }
    }

    bool EnumerateAdapters()
    {
        if (FAILED(CreateDXGIFactory1(IID_PPV_ARGS(&State.Factory)))) { return false; }

        State.AdapterCount = 0;

        for (UINT index = 0; index < MaxAdapterCount; index++)
        {
            IDXGIAdapter1* adapter = nullptr;

            if (FAILED(State.Factory->EnumAdapters1(index, &adapter))) { break; }

            DXGI_ADAPTER_DESC1 desc = {};
            adapter->GetDesc1(&desc);

            AdapterInformation& info = State.Adapters[State.AdapterCount++];

            WideCharToMultiByte(CP_ACP, 0, desc.Description, -1,
                info.Description, MaxAdapterNameLength, nullptr, nullptr);

            info.VendorId = desc.VendorId;
            info.DeviceId = desc.DeviceId;

            adapter->Release();
        }

        return true;
    }

    bool CreateDevice(u32 width, u32 height)
    {
        IDXGIAdapter1* adapter = nullptr;

        if (FAILED(State.Factory->EnumAdapters1(State.SelectedAdapter, &adapter))) { return false; }

        DXGI_SWAP_CHAIN_DESC desc = {};
        desc.BufferCount = 2;
        desc.BufferDesc.Width = width;
        desc.BufferDesc.Height = height;
        desc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        desc.OutputWindow = State.Window;
        desc.SampleDesc.Count = 1;
        desc.Windowed = TRUE;
        desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

        const HRESULT result = D3D11CreateDeviceAndSwapChain(adapter, D3D_DRIVER_TYPE_UNKNOWN, nullptr, 0,
            FeatureLevels, ARRAYSIZE(FeatureLevels), D3D11_SDK_VERSION, &desc,
            &State.SwapChain, &State.Device, nullptr, &State.Context);

        adapter->Release();

        if (FAILED(result)) { return false; }

        if (!CreateBackBuffer())
        {
            ReleaseDevice();
            return false;
        }

        return true;
    }

    void ReleaseDevice()
    {
        Draw::Release();
        Textures::Reset();
        Surface::ReleaseTexture();

        if (State.Context != nullptr) { State.Context->OMSetRenderTargets(0, nullptr, nullptr); }

        if (State.BackBuffer != nullptr) { State.BackBuffer->Release(); State.BackBuffer = nullptr; }
        if (State.SwapChain != nullptr) { State.SwapChain->Release(); State.SwapChain = nullptr; }
        if (State.Context != nullptr) { State.Context->Release(); State.Context = nullptr; }
        if (State.Device != nullptr) { State.Device->Release(); State.Device = nullptr; }

        State.SceneActive = false;
    }

    void ReleaseAll()
    {
        ReleaseDevice();

        if (State.Factory != nullptr) { State.Factory->Release(); State.Factory = nullptr; }

        State.AdapterCount = 0;
    }
}
