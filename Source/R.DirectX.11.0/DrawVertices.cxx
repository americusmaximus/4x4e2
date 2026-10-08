#include "DrawState.hxx"

#include <cmath>

namespace Renderer::DX11::Draw
{
    namespace
    {
        using Renderers::RendererMode;

        u32 ToByte(const f32 value)
        {
            return static_cast<u32>(roundf(255.0f * value)) & 0xff;
        }

        u32 PackColor(const f32 r, const f32 g, const f32 b, const f32 a)
        {
            return (ToByte(a) << 24) | (ToByte(r) << 16) | (ToByte(g) << 8) | ToByte(b);
        }

        void SetPerspectiveUv(const Renderers::RCVX& input, Vertex& output)
        {
            const f32 scale = 1.0f / input.Unk15;

            output.Position[3] = input.Unk15 * input.Unk04;
            output.Uv[0] = scale * input.Unk08 * 0.5f + 0.5f;
            output.Uv[1] = scale * input.Unk09 * -0.5f + 0.5f;
        }

        // The game lays out its screen as 4:3 at the window height, so wider windows are stretched to fit.
        f32 HorizontalScale()
        {
            const f32 layoutWidth = static_cast<f32>(State.Height) * 4.0f / 3.0f;

            return static_cast<f32>(State.Width) / layoutWidth;
        }

        void BuildVertex(const Renderers::RCVX& input, const RendererMode mode,
            const f32 scale, Vertex& output)
        {
            output.Position[0] = input.Unk06 * scale;
            output.Position[1] = input.Unk07;
            output.Position[2] = 1.0f - input.Unk04;
            output.Position[3] = input.Unk04;

            output.Diffuse = PackColor(input.Unk12, input.Unk11, input.Unk10, input.Unk13);
            output.Specular = PackColor(0.0f, 0.0f, 0.0f, 1.0f - input.Unk14);

            output.Uv[0] = input.Unk08;
            output.Uv[1] = input.Unk09;

            if ((mode & RendererMode::U0x400) != RendererMode::None) { SetPerspectiveUv(input, output); }
        }
    }

    void BuildVertices(const Renderers::RCVX* input, const u32 count,
        const RendererMode mode, Vertex* output)
    {
        const f32 scale = HorizontalScale();

        for (u32 x = 0; x < count; x++) { BuildVertex(input[x], mode, scale, output[x]); }
    }
}
