#include "DX.hxx"
#include "Trace.hxx"

// Entry points still without a DirectX 11 implementation. They report failure.
namespace Renderers::Modules
{
    extern "C" u32 AcquireTextureInformation(const u32 id)
    {
        Renderer::DX11::Trace::Call("AcquireTextureInformation", id);
        return FALSE;
    }

    extern "C" u32 AcquireVideoMemory(u32* total, u32* free, u32* used)
    {
        Renderer::DX11::Trace::Call("AcquireVideoMemory");
        (void)total; (void)free; (void)used;
        return FALSE;
    }

    extern "C" u32 BeginCubeMapRender(const u32 id, const RendererCubeMapSide side)
    {
        Renderer::DX11::Trace::Call("BeginCubeMapRender", id, static_cast<u32>(side));
        return FALSE;
    }

    extern "C" u32 BeginRenderToTexture(const u32 id)
    {
        Renderer::DX11::Trace::Call("BeginRenderToTexture", id);
        return FALSE;
    }

    extern "C" u32 DotListD3DTL(const RTLVX* vertexes, const u32 count, const RendererMode mode)
    {
        Renderer::DX11::Trace::Call("DotListD3DTL", count, static_cast<u32>(mode));
        (void)vertexes;
        return FALSE;
    }

    extern "C" u32 EndCubeMapRender(void)
    {
        Renderer::DX11::Trace::Call("EndCubeMapRender");
        return FALSE;
    }

    extern "C" u32 EndRenderToTexture(void)
    {
        Renderer::DX11::Trace::Call("EndRenderToTexture");
        return FALSE;
    }

    extern "C" u32 LineListD3DTL(const RTLVX* vertexes, const u32 count, const RendererMode mode)
    {
        Renderer::DX11::Trace::Call("LineListD3DTL", count, static_cast<u32>(mode));
        (void)vertexes;
        return FALSE;
    }

    extern "C" u32 SelectCubeTexture(const u32 id, const u32 side)
    {
        Renderer::DX11::Trace::Call("SelectCubeTexture", id, side);
        return FALSE;
    }

    extern "C" u32 SelectRenderTexture(const u32 id)
    {
        Renderer::DX11::Trace::Call("SelectRenderTexture", id);
        return FALSE;
    }

    extern "C" u32 SetColorTable(u8* table, u16* lookup)
    {
        Renderer::DX11::Trace::Call("SetColorTable");
        (void)table; (void)lookup;
        return FALSE;
    }

    extern "C" u32 SetMultiTextureBlend(const RendererBlendOperation op)
    {
        Renderer::DX11::Trace::Call("SetMultiTextureBlend", static_cast<u32>(op));
        return FALSE;
    }
}
