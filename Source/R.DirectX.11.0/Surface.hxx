#pragma once

#include "DX.hxx"

namespace Renderer::DX11::Surface
{
    bool Create(u32 width, u32 height, u32** rows);
    void ReleaseTexture();
    u32 Lock();
    u32 Unlock();
}
