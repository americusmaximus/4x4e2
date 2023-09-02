/*
Copyright (c) 2023 Americus Maximus

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "GL.hxx"
#include "Module.hxx"

using namespace Graphics;
using namespace Mathematics;
using namespace Renderers;

namespace Renderer::Module
{
    extern "C" u32 BeginScene(void) { return TRUE; }

    extern "C" u32 Clear(void)
    {
        glClear(GL_COLOR_BUFFER_BIT);

        return TRUE;
    }

    extern "C" u32 ClearZBuffer(void)
    {
        GL::SelectRendererMode(RendererMode::U0x80 | RendererMode::U0x40);

        glClear(GL_DEPTH_BUFFER_BIT);

        return TRUE;
    }

    extern "C" u32 EnableClipping(const RendererClippingMode) { return TRUE; }

    extern "C" u32 EnableCulling(const RendererCullMode mode)
    {
        switch (mode)
        {
        case RendererCullMode::CCW:
        {
            glFrontFace(GL_CCW);
            glCullFace(GL_FRONT);
            glEnable(GL_CULL_FACE);

            break;
        }
        case RendererCullMode::CW:
        {
            glFrontFace(GL_CCW);
            glCullFace(GL_BACK);
            glEnable(GL_CULL_FACE);

            break;
        }
        case RendererCullMode::None:
        {
            glDisable(GL_CULL_FACE);

            break;
        }
        }

        return TRUE;
    }

    extern "C" u32 EndScene(void) { return TRUE; }

    extern "C" u32 LockFrame(void) { return FALSE; }

    extern "C" u32 PolyList(const RVX * vertexes, const u32 vertexCount, const u16 * indexes, const u32 indexCount, const RendererMode mode)
    {
        if (MAX_VERTEX_COUNT < vertexCount) { return FALSE; }

        GL::SelectRendererMode(mode);

        glDisable(GL_LIGHTING);
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);

        glVertexPointer(3, GL_FLOAT, sizeof(RVX), &vertexes[0].XYZ.X);
        glColorPointer(4, GL_FLOAT, sizeof(f32xRGBA), &State.GL.Data.RGBA[0].R);
        glTexCoordPointer(2, GL_FLOAT, sizeof(RVX), &vertexes[0].UV.X);

        for (u32 x = 0; x < vertexCount; x++)
        {
            const auto input = &vertexes[x];

            auto length = input->Normal.X * State.GL.Light.Direction.X
                + input->Normal.Y * State.GL.Light.Direction.Y
                + input->Normal.Z * State.GL.Light.Direction.Z;

            auto color = State.GL.Light.Color;

            if (length < 0.0f)
            {
                color = State.GL.Light.Color - length;

                if (1.0f < color) { color = 1.0f; }
            }


            State.GL.Data.RGBA[x].A = State.GL.Light.Alpha;
            State.GL.Data.RGBA[x].R = color;
            State.GL.Data.RGBA[x].G = color;
            State.GL.Data.RGBA[x].B = color;
        }

        glDrawElements(GL_TRIANGLES, indexCount * GRAPHICS_TRIANGLE_VERTEX_COUNT, GL_UNSIGNED_SHORT, indexes);

        return TRUE;
    }

    extern "C" u32 PolyListD3DL(const RTLVX * vertexes, const u32 vertexCount, const u16 * indexes, const u32 indexCount, const RendererMode mode)
    {
        if (MAX_VERTEX_COUNT < vertexCount) { return FALSE; }

        GL::SelectRendererMode(mode);

        glDisable(GL_LIGHTING);
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);

        glVertexPointer(3, GL_FLOAT, sizeof(RTLVX), &vertexes[0].XYZ);
        glColorPointer(4, GL_FLOAT, sizeof(f32xRGBA), &State.GL.Data.RGBA[0].R);
        glTexCoordPointer(2, GL_FLOAT, sizeof(RTLVX), &vertexes[0].UV);

        for (u32 x = 0; x < vertexCount; x++)
        {
            const auto input = &vertexes[x];

            State.GL.Data.RGBA[x].A = ONE_TWO_HUNDRED_FIFTY_FIFTH * ((input->Color >> 24) & 0xff);
            State.GL.Data.RGBA[x].R = ONE_TWO_HUNDRED_FIFTY_FIFTH * ((input->Color >> 16) & 0xff);
            State.GL.Data.RGBA[x].G = ONE_TWO_HUNDRED_FIFTY_FIFTH * ((input->Color >> 8) & 0xff);
            State.GL.Data.RGBA[x].B = ONE_TWO_HUNDRED_FIFTY_FIFTH * ((input->Color >> 0) & 0xff);
        }

        glDrawElements(GL_TRIANGLES, indexCount * GRAPHICS_TRIANGLE_VERTEX_COUNT, GL_UNSIGNED_SHORT, indexes);

        return TRUE;
    }

    extern "C" u32 PolyListTL(const RCVX * vertexes, const u32 vertexCount, const u16 * indexes, const u32 indexCount, const RendererMode mode)
    {
        if (MAX_VERTEX_COUNT < vertexCount) { return FALSE; }

        GL::SelectRendererMode(mode);

        glDisable(GL_LIGHTING);
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);

        glVertexPointer(3, GL_FLOAT, sizeof(f32x3), &State.GL.Data.XYZ[0].X);
        glColorPointer(4, GL_FLOAT, sizeof(f32xRGBA), &State.GL.Data.RGBA[0].R);
        glTexCoordPointer(2, GL_FLOAT, sizeof(RCVX), &vertexes[0].Unk08);

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        glMatrixMode(GL_PROJECTION);
        glPushMatrix();

        const auto clippingNear = State.GL.Mode.Clipping.Near;
        const auto clippingFar = State.GL.Mode.Clipping.Far;

        const Matrix4x4 matrix =
        {
            1.0f, 0.0f, 0.0f,                                                       0.0f,
            0.0f, 1.0f, 0.0f,                                                       0.0f,
            0.0f, 0.0f, clippingFar / (clippingFar - clippingNear),                 1.0f,
            0.0f, 0.0f, -clippingNear * clippingFar / (clippingFar - clippingNear), 0.0f
        };

        glLoadMatrixf((f32*)&matrix);

        for (u32 x = 0; x < vertexCount; x++)
        {
            const auto input = &vertexes[x];

            State.GL.Data.XYZ[x].X = input->Unk01 - input->Unk03 / (f32)State.Window.Width;
            State.GL.Data.XYZ[x].Y = input->Unk02 - input->Unk03 / (f32)State.Window.Height;
            State.GL.Data.XYZ[x].Z = input->Unk03;


            State.GL.Data.RGBA[x].R = input->Unk12;
            State.GL.Data.RGBA[x].G = input->Unk11;
            State.GL.Data.RGBA[x].B = input->Unk10;
            State.GL.Data.RGBA[x].A = input->Unk13;
        }

        glDrawElements(GL_TRIANGLES, indexCount * GRAPHICS_TRIANGLE_VERTEX_COUNT, GL_UNSIGNED_SHORT, indexes);

        glMatrixMode(GL_PROJECTION);
        glPopMatrix();

        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();

        return TRUE;
    }

    extern "C" u32 SetAlpha(const u32 color)
    {
        glAlphaFunc(GL_GREATER, color * ONE_TWO_HUNDRED_FIFTY_FIFTH);

        return TRUE;
    }

    extern "C" u32 SetAmbientLight(const f32 color)
    {
        State.GL.Light.Color = color;

        return TRUE;
    }

    extern "C" u32 SetColorTable(u8*, u16*) { return FALSE; }

    extern "C" u32 SetDestinationBlend(const RendererDestinationBlend mode)
    {
        switch (mode)
        {
        case RendererDestinationBlend::InverseAlpha: { State.GL.Mode.DestinationBlendMode = GL_ONE_MINUS_SRC_ALPHA; break; }
        case RendererDestinationBlend::One: { State.GL.Mode.DestinationBlendMode = GL_ONE; break; }
        case RendererDestinationBlend::Color: { State.GL.Mode.DestinationBlendMode = GL_SRC_COLOR; break; }
        case RendererDestinationBlend::InverseColor: { State.GL.Mode.DestinationBlendMode = GL_ONE_MINUS_SRC_COLOR; break; }
        case RendererDestinationBlend::Zero: { State.GL.Mode.DestinationBlendMode = GL_ZERO; break; }
        }

        return TRUE;
    }

    extern "C" u32 SetFog(const f32 r, const f32 g, const f32 b, const f32 start, const f32 end)
    {
        const f32 color[] = { r, g, b, 0.0f };

        glFogi(GL_FOG_MODE, GL_LINEAR);
        glFogfv(GL_FOG_COLOR, color);
        glFogf(GL_FOG_START, start);
        glFogf(GL_FOG_END, end);

        return TRUE;
    }

    extern "C" u32 SetLightConstants(const f32, const f32, const f32, const f32 alpha)
    {
        State.GL.Light.Alpha = alpha;

        return TRUE;
    }


    extern "C" u32 SetLightVector(const f32x3*, const f32) { return TRUE; }

    extern "C" u32 SetSourceBlend(const RendererSourceBlendMode mode)
    {
        switch (mode)
        {
        case RendererSourceBlendMode::Alpha: { State.GL.Mode.SourceBlendMode = GL_SRC_ALPHA; break; }
        case RendererSourceBlendMode::One: { State.GL.Mode.SourceBlendMode = GL_ONE; break; }
        case RendererSourceBlendMode::Zero: { State.GL.Mode.SourceBlendMode = GL_ZERO; break; }
        }

        return TRUE;
    }

    extern "C" u32 SetTransform(const Matrix4x4 * transform, const f32 zNear, const f32 zFar, const f32, const f32x3 * direction, const f32x3*)
    {
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glLoadMatrixf((f32*)transform);

        const auto clippingNear = State.GL.Mode.Clipping.Near;
        const auto clippingFar = State.GL.Mode.Clipping.Far;

        const Matrix4x4 projection =
        {
            zNear, 0.0f, 0.0f,                                                        0.0f,
            0.0f,  zFar, 0.0f,                                                        0.0f,
            0.0f,  0.0f,  clippingFar / (clippingFar - clippingNear),                 1.0f,
            0.0f,  0.0f,  -clippingNear * clippingFar / (clippingFar - clippingNear), 0.0f
        };

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        glLoadMatrixf((f32*)&projection);

        State.GL.Light.Direction.X = direction->X;
        State.GL.Light.Direction.Y = direction->Y;
        State.GL.Light.Direction.Z = direction->Z;

        return TRUE;
    }

    extern "C" u32 SetViewPort(const f32 x0, const f32 y0, const f32 x1, const f32 y1, const f32 zNear, const f32 zFar)
    {
        glViewport((s32)x0, (s32)y0, (s32)(x1 - x0), (s32)(y1 - y0));

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        glFrustum(-1.0, 1.0, -1.0, 1.0, 1.0, 8192.0);

        State.GL.Mode.Clipping.Near = zNear;
        State.GL.Mode.Clipping.Far = zFar;

        return TRUE;
    }

    extern "C" u32 Toggle(void)
    {
        SwapBuffers(State.Window.DeviceContext);

        return TRUE;
    }

    extern "C" u32 UnlockFrame(void) { return FALSE; }
}