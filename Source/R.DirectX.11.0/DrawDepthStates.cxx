#include "DrawState.hxx"
#include "Trace.hxx"

#include <unordered_map>

namespace Renderer::DX11::Draw
{
    namespace
    {
        using Renderers::RendererMode;

        std::unordered_map<u32, ID3D11DepthStencilState*> DepthStates;

        ID3D11DepthStencilState* CreateDepthState(const bool read, const bool write)
        {
            D3D11_DEPTH_STENCIL_DESC desc = {};

            desc.DepthEnable = read || write;
            desc.DepthWriteMask = write ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
            desc.DepthFunc = (write && !read) ? D3D11_COMPARISON_ALWAYS : D3D11_COMPARISON_LESS_EQUAL;

            ID3D11DepthStencilState* state = nullptr;

            if (FAILED(State.Device->CreateDepthStencilState(&desc, &state)))
            {
                Trace::Call("CreateDepthStencilState failed");
                return nullptr;
            }

            return state;
        }
    }

    ID3D11DepthStencilState* DepthState(const RendererMode mode)
    {
        const bool read = (mode & RendererMode::U0x40) != RendererMode::None;
        const bool write = (mode & RendererMode::U0x80) != RendererMode::None;
        const u32 key = (read ? 1u : 0u) | (write ? 2u : 0u);
        const auto found = DepthStates.find(key);

        if (found != DepthStates.end()) { return found->second; }

        ID3D11DepthStencilState* state = CreateDepthState(read, write);
        DepthStates.emplace(key, state);

        return state;
    }

    void ReleaseDepthStates()
    {
        for (auto& entry : DepthStates)
        {
            if (entry.second != nullptr) { entry.second->Release(); }
        }

        DepthStates.clear();
    }
}
