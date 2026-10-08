#include "DrawState.hxx"
#include "Trace.hxx"

#include <unordered_map>

namespace Renderer::DX11::Draw
{
    namespace
    {
        using Renderers::RendererClampMode;
        using Renderers::RendererCullMode;

        std::unordered_map<u32, ID3D11RasterizerState*> RasterizerStates;
        std::unordered_map<u32, ID3D11SamplerState*> SamplerStates;

        D3D11_CULL_MODE CullFactor(const RendererCullMode mode)
        {
            switch (mode)
            {
                case RendererCullMode::CW: return D3D11_CULL_FRONT;
                case RendererCullMode::None: return D3D11_CULL_NONE;
                default: return D3D11_CULL_BACK;
            }
        }

        D3D11_TEXTURE_ADDRESS_MODE AddressMode(const RendererClampMode mode)
        {
            return mode == RendererClampMode::Clamp
                ? D3D11_TEXTURE_ADDRESS_CLAMP
                : D3D11_TEXTURE_ADDRESS_WRAP;
        }

        ID3D11RasterizerState* CreateRasterizerState(const RendererCullMode mode)
        {
            D3D11_RASTERIZER_DESC desc = {};

            desc.FillMode = D3D11_FILL_SOLID;
            desc.CullMode = CullFactor(mode);
            desc.DepthClipEnable = TRUE;

            ID3D11RasterizerState* state = nullptr;

            if (FAILED(State.Device->CreateRasterizerState(&desc, &state)))
            {
                Trace::Call("CreateRasterizerState failed");
                return nullptr;
            }

            return state;
        }

        ID3D11SamplerState* CreateSamplerState(const RendererClampMode u,
            const RendererClampMode v)
        {
            D3D11_SAMPLER_DESC desc = {};

            desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
            desc.AddressU = AddressMode(u);
            desc.AddressV = AddressMode(v);
            desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
            desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
            desc.MaxAnisotropy = 1;
            desc.MaxLOD = D3D11_FLOAT32_MAX;

            ID3D11SamplerState* state = nullptr;

            if (FAILED(State.Device->CreateSamplerState(&desc, &state)))
            {
                Trace::Call("CreateSamplerState failed");
                return nullptr;
            }

            return state;
        }
    }

    ID3D11RasterizerState* RasterizerState()
    {
        const u32 key = static_cast<u32>(Current.Cull);
        const auto found = RasterizerStates.find(key);

        if (found != RasterizerStates.end()) { return found->second; }

        ID3D11RasterizerState* state = CreateRasterizerState(Current.Cull);
        RasterizerStates.emplace(key, state);

        return state;
    }

    ID3D11SamplerState* SamplerState(const RendererClampMode u, const RendererClampMode v)
    {
        const u32 key = (static_cast<u32>(u) << 1) | static_cast<u32>(v);
        const auto found = SamplerStates.find(key);

        if (found != SamplerStates.end()) { return found->second; }

        ID3D11SamplerState* state = CreateSamplerState(u, v);
        SamplerStates.emplace(key, state);

        return state;
    }

    void ReleaseSamplerStates()
    {
        for (auto& entry : RasterizerStates)
        {
            if (entry.second != nullptr) { entry.second->Release(); }
        }

        RasterizerStates.clear();

        for (auto& entry : SamplerStates)
        {
            if (entry.second != nullptr) { entry.second->Release(); }
        }

        SamplerStates.clear();
    }
}
