#pragma once

#include "Draw.hxx"

namespace Renderer::DX11::Draw
{
    constexpr u32 MaxVertexCount = 65536;
    constexpr u32 MaxIndexCount = 65536;
    constexpr u32 MaxClampStageCount = 8;

    struct Vertex
    {
        f32 Position[4];
        u32 Diffuse;
        u32 Specular;
        f32 Uv[2];
    };

    struct Constants
    {
        f32 Viewport[2];
        f32 Padding[2];
        f32 Flags[4];
        f32 FogColor[4];
    };

    struct Settings
    {
        f32 FogColor[4] = {};
        Renderers::RendererSourceBlendMode SourceBlend = Renderers::RendererSourceBlendMode::Alpha;
        Renderers::RendererDestinationBlend DestinationBlend = Renderers::RendererDestinationBlend::InverseAlpha;
        Renderers::RendererCullMode Cull = Renderers::RendererCullMode::CCW;
        u32 AlphaRef = 0;
        Renderers::RendererClampMode ClampU[MaxClampStageCount] = {};
        Renderers::RendererClampMode ClampV[MaxClampStageCount] = {};
    };

    struct Resources
    {
        ID3D11VertexShader* VertexShader = nullptr;
        ID3D11PixelShader* PixelShader = nullptr;
        ID3D11InputLayout* InputLayout = nullptr;
        ID3D11Buffer* VertexBuffer = nullptr;
        ID3D11Buffer* IndexBuffer = nullptr;
        ID3D11Buffer* ConstantBuffer = nullptr;
        ID3D11Texture2D* DepthTexture = nullptr;
        ID3D11DepthStencilView* DepthView = nullptr;
    };

    extern Settings Current;
    extern Resources Objects;

    bool CreateShaders();
    void ReleaseShaders();
    bool CreateBuffers();
    void ReleaseBuffers();
    bool CreateDepth(u32 width, u32 height);
    void ReleaseDepth();

    void UploadVertices(const Vertex* vertexes, u32 count);
    void UploadIndices(const u16* indexes, u32 count);
    void UploadConstants(const Constants& constants);

    void BuildVertices(const Renderers::RCVX* input, u32 count,
        Renderers::RendererMode mode, Vertex* output);

    void Apply(Renderers::RendererMode mode);

    ID3D11BlendState* BlendState(bool enabled);
    ID3D11DepthStencilState* DepthState(Renderers::RendererMode mode);
    ID3D11RasterizerState* RasterizerState();
    ID3D11SamplerState* SamplerState(Renderers::RendererClampMode u,
        Renderers::RendererClampMode v);
    void ReleaseStates();
    void ReleaseBlendStates();
    void ReleaseDepthStates();
    void ReleaseSamplerStates();
}
