#include "DX.hxx"
#include "Draw.hxx"
#include "Surface.hxx"
#include "Trace.hxx"

#include <cstring>

namespace Renderers::Modules
{
    extern "C" u32 AcquireModuleName(char* name)
    {
        if (name == nullptr) { return FALSE; }

        strcpy(name, RENDERER_MODULE_TITLE_NAME);

        return TRUE;
    }

    extern "C" u32 AcquireGraphicsCardList(u32* count, char** apis, char** names, u32* vids, u32* dids)
    {
        const u32 available = Renderer::DX11::State.AdapterCount;

        *count = available;

        for (u32 x = 0; x < available; x++)
        {
            const Renderer::DX11::AdapterInformation& adapter = Renderer::DX11::State.Adapters[x];

            apis[x] = const_cast<char*>(RENDERER_MODULE_TITLE_NAME);
            names[x] = const_cast<char*>(adapter.Description);
            vids[x] = adapter.VendorId;
            dids[x] = adapter.DeviceId;
        }

        return TRUE;
    }

    extern "C" u32 Init(const HWND hwnd, const RendererModuleInitializeArguments* args)
    {
        (void)args;

        Renderer::DX11::Trace::Call("Init");
        Renderer::DX11::ReleaseAll();

        Renderer::DX11::State.Window = hwnd;

        return Renderer::DX11::EnumerateAdapters() ? TRUE : FALSE;
    }

    extern "C" u32 Kill(void)
    {
        Renderer::DX11::ReleaseAll();

        return TRUE;
    }

    extern "C" u32 SelectGraphicsCard(const u32 indx)
    {
        if (indx >= Renderer::DX11::State.AdapterCount) { return FALSE; }

        Renderer::DX11::State.SelectedAdapter = indx;

        return TRUE;
    }

    extern "C" u32 SelectVideoMode(const u32 width, const u32 height, const u32 bpp, u32** pixels)
    {
        (void)bpp;

        auto& state = Renderer::DX11::State;

        Renderer::DX11::Trace::Call("SelectVideoMode", width, height);

        if (state.Factory == nullptr) { return FALSE; }

        Renderer::DX11::ReleaseDevice();

        state.Width = width;
        state.Height = height;

        if (!Renderer::DX11::CreateDevice(width, height)) { return FALSE; }

        if (!Renderer::DX11::Draw::Create(width, height)) { return FALSE; }

        Renderer::DX11::Draw::Bind();

        Renderer::DX11::Trace::Call("LayoutStretch", width, height);

        return Renderer::DX11::Surface::Create(width, height, pixels) ? TRUE : FALSE;
    }

    extern "C" u32 RestoreVideoMode(void) { return TRUE; }

    extern "C" u32 VideoSave(void) { return TRUE; }

    extern "C" u32 VideoRestore(void) { return TRUE; }

    extern "C" u32 BeginScene(void)
    {
        auto& state = Renderer::DX11::State;

        Renderer::DX11::Trace::Call("BeginScene");

        if (state.Device == nullptr || state.SceneActive) { return FALSE; }

        state.SceneActive = true;

        return TRUE;
    }

    extern "C" u32 EndScene(void)
    {
        auto& state = Renderer::DX11::State;

        if (!state.SceneActive) { return FALSE; }

        state.SceneActive = false;

        return TRUE;
    }

    extern "C" u32 LockFrame(void) { return Renderer::DX11::Surface::Lock(); }

    extern "C" u32 UnlockFrame(void) { return Renderer::DX11::Surface::Unlock(); }

    extern "C" u32 Clear(void)
    {
        auto& state = Renderer::DX11::State;

        if (state.BackBuffer == nullptr) { return FALSE; }

        constexpr float black[4] = { 0.f, 0.f, 0.f, 1.f };

        state.Context->ClearRenderTargetView(state.BackBuffer, black);

        return TRUE;
    }

    extern "C" u32 ClearZBuffer(void)
    {
        Renderer::DX11::Trace::Call("ClearZBuffer");

        Renderer::DX11::Draw::ClearDepth();

        return TRUE;
    }

    extern "C" u32 Toggle(void)
    {
        auto& state = Renderer::DX11::State;

        Renderer::DX11::Trace::Call("Toggle");

        if (state.SwapChain == nullptr) { return FALSE; }

        const HRESULT result = state.SwapChain->Present(1, 0);

        Renderer::DX11::Draw::Bind();

        return SUCCEEDED(result) ? TRUE : FALSE;
    }
}
