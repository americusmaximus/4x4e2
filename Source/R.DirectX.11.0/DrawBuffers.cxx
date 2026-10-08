#include "DrawState.hxx"

namespace Renderer::DX11::Draw
{
    namespace
    {
        ID3D11Buffer* CreateBuffer(const UINT bytes, const UINT bind)
        {
            D3D11_BUFFER_DESC desc = {};

            desc.ByteWidth = bytes;
            desc.Usage = D3D11_USAGE_DEFAULT;
            desc.BindFlags = bind;

            ID3D11Buffer* buffer = nullptr;

            if (FAILED(State.Device->CreateBuffer(&desc, nullptr, &buffer))) { return nullptr; }

            return buffer;
        }

        void UploadBytes(ID3D11Buffer* buffer, const void* data, const u32 bytes)
        {
            const D3D11_BOX box = { 0, 0, 0, bytes, 1, 1 };

            State.Context->UpdateSubresource(buffer, 0, &box, data, 0, 0);
        }
    }

    bool CreateBuffers()
    {
        Objects.VertexBuffer = CreateBuffer(MaxVertexCount * sizeof(Vertex), D3D11_BIND_VERTEX_BUFFER);
        Objects.IndexBuffer = CreateBuffer(MaxIndexCount * sizeof(u16), D3D11_BIND_INDEX_BUFFER);
        Objects.ConstantBuffer = CreateBuffer(sizeof(Constants), D3D11_BIND_CONSTANT_BUFFER);

        if (Objects.VertexBuffer == nullptr || Objects.IndexBuffer == nullptr
            || Objects.ConstantBuffer == nullptr)
        {
            ReleaseBuffers();
            return false;
        }

        return true;
    }

    void ReleaseBuffers()
    {
        if (Objects.ConstantBuffer != nullptr) { Objects.ConstantBuffer->Release(); Objects.ConstantBuffer = nullptr; }
        if (Objects.IndexBuffer != nullptr) { Objects.IndexBuffer->Release(); Objects.IndexBuffer = nullptr; }
        if (Objects.VertexBuffer != nullptr) { Objects.VertexBuffer->Release(); Objects.VertexBuffer = nullptr; }
    }

    void UploadVertices(const Vertex* vertexes, const u32 count)
    {
        UploadBytes(Objects.VertexBuffer, vertexes, count * sizeof(Vertex));
    }

    void UploadIndices(const u16* indexes, const u32 count)
    {
        UploadBytes(Objects.IndexBuffer, indexes, count * sizeof(u16));
    }

    void UploadConstants(const Constants& constants)
    {
        State.Context->UpdateSubresource(Objects.ConstantBuffer, 0, nullptr, &constants, 0, 0);
    }
}
