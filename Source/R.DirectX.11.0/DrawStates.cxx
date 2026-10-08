#include "DrawState.hxx"

namespace Renderer::DX11::Draw
{
    void ReleaseStates()
    {
        ReleaseBlendStates();
        ReleaseDepthStates();
        ReleaseSamplerStates();
    }
}
