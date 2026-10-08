#include "DrawState.hxx"
#include "Trace.hxx"

#include <unordered_map>

namespace Renderer::DX11::Draw
{
    namespace
    {
        using Renderers::RendererDestinationBlend;
        using Renderers::RendererSourceBlendMode;

        std::unordered_map<u32, ID3D11BlendState*> BlendStates;

        D3D11_BLEND SourceFactor(const RendererSourceBlendMode mode)
        {
            switch (mode)
            {
                case RendererSourceBlendMode::One: return D3D11_BLEND_ONE;
                case RendererSourceBlendMode::Zero: return D3D11_BLEND_ZERO;
                default: return D3D11_BLEND_SRC_ALPHA;
            }
        }

        D3D11_BLEND DestinationFactor(const RendererDestinationBlend mode)
        {
            switch (mode)
            {
                case RendererDestinationBlend::One: return D3D11_BLEND_ONE;
                case RendererDestinationBlend::Color: return D3D11_BLEND_SRC_COLOR;
                case RendererDestinationBlend::InverseColor: return D3D11_BLEND_INV_SRC_COLOR;
                case RendererDestinationBlend::Zero: return D3D11_BLEND_ZERO;
                default: return D3D11_BLEND_INV_SRC_ALPHA;
            }
        }

        u32 BlendKey(const bool enabled)
        {
            return (enabled ? 1u : 0u)
                | (static_cast<u32>(Current.SourceBlend) << 1)
                | (static_cast<u32>(Current.DestinationBlend) << 4);
        }

        ID3D11BlendState* CreateBlendState(const bool enabled)
        {
            D3D11_BLEND_DESC desc = {};
            D3D11_RENDER_TARGET_BLEND_DESC& target = desc.RenderTarget[0];

            target.BlendEnable = enabled;
            target.SrcBlend = SourceFactor(Current.SourceBlend);
            target.DestBlend = DestinationFactor(Current.DestinationBlend);
            target.BlendOp = D3D11_BLEND_OP_ADD;
            target.SrcBlendAlpha = D3D11_BLEND_ONE;
            target.DestBlendAlpha = D3D11_BLEND_ZERO;
            target.BlendOpAlpha = D3D11_BLEND_OP_ADD;
            target.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

            ID3D11BlendState* state = nullptr;

            if (FAILED(State.Device->CreateBlendState(&desc, &state)))
            {
                Trace::Call("CreateBlendState failed");
                return nullptr;
            }

            return state;
        }
    }

    ID3D11BlendState* BlendState(const bool enabled)
    {
        const u32 key = BlendKey(enabled);
        const auto found = BlendStates.find(key);

        if (found != BlendStates.end()) { return found->second; }

        ID3D11BlendState* state = CreateBlendState(enabled);
        BlendStates.emplace(key, state);

        return state;
    }

    void ReleaseBlendStates()
    {
        for (auto& entry : BlendStates)
        {
            if (entry.second != nullptr) { entry.second->Release(); }
        }

        BlendStates.clear();
    }
}
