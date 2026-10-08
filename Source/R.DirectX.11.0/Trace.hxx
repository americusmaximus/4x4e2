#pragma once

#include "Basic.hxx"

namespace Renderer::DX11::Trace
{
    void Write(const char* text);
    void Call(const char* name);
    void Call(const char* name, u32 a);
    void Call(const char* name, u32 a, u32 b);
}
