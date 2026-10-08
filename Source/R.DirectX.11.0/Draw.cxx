#include "DrawState.hxx"
#include "Trace.hxx"

namespace Renderer::DX11::Draw
{
    Settings Current;
    Resources Objects;

    namespace
    {
        Vertex Staging[MaxVertexCount];
    }

    bool Create(const u32 width, const u32 height)
    {
        if (CreateShaders() && CreateBuffers() && CreateDepth(width, height)) { return true; }

        Release();

        return false;
    }

    void Release()
    {
        ReleaseStates();
        ReleaseDepth();
        ReleaseBuffers();
        ReleaseShaders();
    }

    void Bind()
    {
        ID3D11RenderTargetView* target = State.BackBuffer;

        State.Context->OMSetRenderTargets(1, &target, Objects.DepthView);
    }

    void ClearDepth()
    {
        if (Objects.DepthView == nullptr) { return; }

        State.Context->ClearDepthStencilView(Objects.DepthView, D3D11_CLEAR_DEPTH, 1.0f, 0);
    }

    u32 PolyList(const Renderers::RCVX* input, const u32 count, const u16* indexes,
        const u32 indexCount, const Renderers::RendererMode mode)
    {
        // The DX8 renderer hands indexCount to DrawIndexedPrimitiveUP as a triangle count.
        const u32 triangleCount = indexCount;
        const u32 totalIndexCount = triangleCount * 3;

        if (!State.SceneActive || count > MaxVertexCount || totalIndexCount > MaxIndexCount)
        {
            Trace::Call("PolyListTL rejected", count, triangleCount);
            return FALSE;
        }

        BuildVertices(input, count, mode, Staging);
        UploadVertices(Staging, count);
        UploadIndices(indexes, totalIndexCount);
        Apply(mode);

        State.Context->DrawIndexed(totalIndexCount, 0, 0);

        return TRUE;
    }
}
