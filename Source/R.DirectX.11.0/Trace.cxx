#include "Trace.hxx"

#include <cstdio>

namespace Renderer::DX11::Trace
{
    namespace
    {
        constexpr const char* LogFileName = "dx11-trace.log";
    }

    void Write(const char* text)
    {
        FILE* file = fopen(LogFileName, "a");

        if (file == nullptr) { return; }

        fputs(text, file);
        fputc('\n', file);
        fclose(file);
    }

    void Call(const char* name)
    {
        Write(name);
    }

    void Call(const char* name, u32 a)
    {
        char line[192];

        snprintf(line, sizeof(line), "%s(%u)", name, a);
        Write(line);
    }

    void Call(const char* name, u32 a, u32 b)
    {
        char line[192];

        snprintf(line, sizeof(line), "%s(%u, %u)", name, a, b);
        Write(line);
    }
}
