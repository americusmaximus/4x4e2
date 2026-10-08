#include "DrawState.hxx"
#include "Trace.hxx"

#include <d3dcompiler.h>

namespace Renderer::DX11::Draw
{
    namespace
    {
        constexpr char Shaders[] = R"HLSL(
cbuffer Constants : register(b0)
{
    float2 Viewport;
    float2 Padding;
    float4 Flags;
    float4 FogColor;
};

Texture2D Diffuse : register(t0);
SamplerState Sampler : register(s0);

struct VertexInput
{
    float4 Position : POSITION;
    float4 Color : COLOR;
    float4 Specular : COLOR1;
    float2 Uv : TEXCOORD0;
};

struct PixelInput
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR;
    float4 Specular : COLOR1;
    float2 Uv : TEXCOORD0;
};

PixelInput VertexMain(VertexInput input)
{
    PixelInput output;
    const float w = 1.0f / input.Position.w;
    const float x = input.Position.x / Viewport.x * 2.0f - 1.0f;
    const float y = 1.0f - input.Position.y / Viewport.y * 2.0f;
    output.Position = float4(x * w, y * w, input.Position.z * w, w);
    output.Color = input.Color;
    output.Specular = input.Specular;
    output.Uv = input.Uv;
    return output;
}

float4 PixelMain(PixelInput input) : SV_TARGET
{
    float4 color = input.Color;
    if (Flags.x > 0.5f) { color *= Diffuse.Sample(Sampler, input.Uv); }
    if (Flags.w > 0.5f) { color.rgb = lerp(FogColor.rgb, color.rgb, input.Specular.a); }
    if (Flags.y > 0.5f && color.a * 255.0f <= Flags.z) { discard; }
    return color;
}
)HLSL";

        const D3D11_INPUT_ELEMENT_DESC Layout[] = {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
                offsetof(Vertex, Position), D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "COLOR", 0, DXGI_FORMAT_B8G8R8A8_UNORM, 0,
                offsetof(Vertex, Diffuse), D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "COLOR", 1, DXGI_FORMAT_B8G8R8A8_UNORM, 0,
                offsetof(Vertex, Specular), D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
                offsetof(Vertex, Uv), D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };

        ID3DBlob* Compile(const char* entry, const char* profile)
        {
            ID3DBlob* blob = nullptr;
            ID3DBlob* errors = nullptr;

            const HRESULT result = D3DCompile(Shaders, sizeof(Shaders) - 1, nullptr,
                nullptr, nullptr, entry, profile, 0, 0, &blob, &errors);

            if (errors != nullptr)
            {
                Trace::Write(static_cast<const char*>(errors->GetBufferPointer()));
                errors->Release();
            }

            if (FAILED(result))
            {
                if (blob != nullptr) { blob->Release(); }
                return nullptr;
            }

            return blob;
        }

        bool CreateVertexShader(ID3DBlob* blob)
        {
            return SUCCEEDED(State.Device->CreateVertexShader(blob->GetBufferPointer(),
                blob->GetBufferSize(), nullptr, &Objects.VertexShader))
                && SUCCEEDED(State.Device->CreateInputLayout(Layout, ARRAYSIZE(Layout),
                    blob->GetBufferPointer(), blob->GetBufferSize(), &Objects.InputLayout));
        }

        bool CreatePixelShader(ID3DBlob* blob)
        {
            return SUCCEEDED(State.Device->CreatePixelShader(blob->GetBufferPointer(),
                blob->GetBufferSize(), nullptr, &Objects.PixelShader));
        }
    }

    bool CreateShaders()
    {
        ID3DBlob* vertex = Compile("VertexMain", "vs_4_0");
        ID3DBlob* pixel = Compile("PixelMain", "ps_4_0");

        const bool created = vertex != nullptr && pixel != nullptr
            && CreateVertexShader(vertex) && CreatePixelShader(pixel);

        if (vertex != nullptr) { vertex->Release(); }
        if (pixel != nullptr) { pixel->Release(); }

        if (!created) { ReleaseShaders(); }

        return created;
    }

    void ReleaseShaders()
    {
        if (Objects.InputLayout != nullptr) { Objects.InputLayout->Release(); Objects.InputLayout = nullptr; }
        if (Objects.VertexShader != nullptr) { Objects.VertexShader->Release(); Objects.VertexShader = nullptr; }
        if (Objects.PixelShader != nullptr) { Objects.PixelShader->Release(); Objects.PixelShader = nullptr; }
    }
}
