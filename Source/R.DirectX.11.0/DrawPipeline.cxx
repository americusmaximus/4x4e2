#include "DrawState.hxx"
#include "Textures.hxx"

#include <cstring>

namespace Renderer::DX11::Draw
{
    namespace
    {
        using Renderers::RendererMode;

        bool HasMode(const RendererMode mode, const RendererMode flag)
        {
            return (mode & flag) != RendererMode::None;
        }

        Constants BuildConstants(const RendererMode mode)
        {
            Constants constants = {};

            constants.Viewport[0] = static_cast<f32>(State.Width);
            constants.Viewport[1] = static_cast<f32>(State.Height);
            constants.Flags[0] = HasMode(mode, RendererMode::U0x1) ? 1.0f : 0.0f;
            constants.Flags[1] = HasMode(mode, RendererMode::U0x2) ? 1.0f : 0.0f;
            constants.Flags[2] = static_cast<f32>(Current.AlphaRef);
            constants.Flags[3] = HasMode(mode, RendererMode::U0x8) ? 1.0f : 0.0f;
            std::memcpy(constants.FogColor, Current.FogColor, sizeof(constants.FogColor));

            return constants;
        }

        void BindInput()
        {
            const UINT stride = sizeof(Vertex);
            const UINT offset = 0;

            State.Context->IASetInputLayout(Objects.InputLayout);
            State.Context->IASetVertexBuffers(0, 1, &Objects.VertexBuffer, &stride, &offset);
            State.Context->IASetIndexBuffer(Objects.IndexBuffer, DXGI_FORMAT_R16_UINT, 0);
            State.Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        }

        void BindShaders()
        {
            State.Context->VSSetShader(Objects.VertexShader, nullptr, 0);
            State.Context->PSSetShader(Objects.PixelShader, nullptr, 0);
            State.Context->VSSetConstantBuffers(0, 1, &Objects.ConstantBuffer);
            State.Context->PSSetConstantBuffers(0, 1, &Objects.ConstantBuffer);
        }

        void BindTexture(const RendererMode mode)
        {
            ID3D11ShaderResourceView* view = nullptr;

            if (HasMode(mode, RendererMode::U0x1)) { view = Textures::View(0); }

            ID3D11SamplerState* sampler = SamplerState(Current.ClampU[0], Current.ClampV[0]);

            State.Context->PSSetShaderResources(0, 1, &view);
            State.Context->PSSetSamplers(0, 1, &sampler);
        }
    }

    void Apply(const RendererMode mode)
    {
        const D3D11_VIEWPORT viewport = {
            0.0f, 0.0f, static_cast<f32>(State.Width), static_cast<f32>(State.Height), 0.0f, 1.0f };

        State.Context->RSSetViewports(1, &viewport);
        State.Context->RSSetState(RasterizerState());
        State.Context->OMSetBlendState(BlendState(HasMode(mode, RendererMode::U0x2)),
            nullptr, 0xffffffffu);
        State.Context->OMSetDepthStencilState(DepthState(mode), 0);

        BindInput();
        BindShaders();
        BindTexture(mode);
        UploadConstants(BuildConstants(mode));
    }
}
