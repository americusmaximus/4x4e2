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
#include "Mathematics.Basic.hxx"
#include "Module.hxx"

using namespace Renderers::Modules;
using namespace Renderer::Module;
using namespace Renderers;

namespace GL
{
    void Initialize(void)
    {
        ResetTextureCache();

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        glEnableClientState(GL_NORMAL_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);

        glViewport(0, 0, State.Window.Width, State.Window.Height);
        glFrustum(-1.0, 1.0, -1.0, 1.0, 1.0, 1600.0);
        glDepthRange(0.0, 1.0);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glShadeModel(GL_SMOOTH);
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_ALPHA_TEST);
        glAlphaFunc(GL_GREATER, 0.0039215689f);
        glDisable(GL_FOG);
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ZERO);
        glEnable(GL_TEXTURE_2D);
        glDisable(GL_DITHER);
        glFrontFace(GL_CCW);
        glCullFace(GL_FRONT);
        glEnable(GL_CULL_FACE);
        glDisable(GL_LIGHTING);

        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    }

    void Release()
    {
        wglMakeCurrent(NULL, NULL);

        if (State.Window.RenderContext != NULL)
        {
            wglDeleteContext(State.Window.RenderContext);

            State.Window.RenderContext = NULL;
        }

        if (State.Window.DeviceContext != NULL)
        {
            ReleaseDC(State.Window.HWND, State.Window.DeviceContext);

            State.Window.DeviceContext = NULL;
        }
    }

    void SelectRendererMode(const RendererMode mode)
    {
        glEnable(GL_TEXTURE_2D);

        if ((mode & RendererMode::U0x20) == RendererMode::None)
        {
            glDisable(GL_BLEND);
            glDisable(GL_ALPHA_TEST);
        }
        else
        {
            glEnable(GL_BLEND);
            glEnable(GL_ALPHA_TEST);
        }

        glBlendFunc(State.GL.Mode.SourceBlendMode, State.GL.Mode.DestinationBlendMode);

        if (((State.GL.Mode.Mode ^ mode) & RendererMode::U0x8) != RendererMode::None)
        {
            if ((mode & RendererMode::U0x8) == RendererMode::None)
            {
                glDisable(0xb60);
            }
            else
            {
                glEnable(0xb60);
            }
        }

        if (((State.GL.Mode.Mode ^ mode) & (RendererMode::U0x80 | RendererMode::U0x40)) != RendererMode::None)
        {
            if ((mode & RendererMode::U0x80) != RendererMode::None && (mode & RendererMode::U0x40) != RendererMode::None)
            {
                glEnable(GL_DEPTH_TEST);
                glDepthMask(GL_TRUE);
                glDepthFunc(GL_LEQUAL);
            }
            else if ((mode & RendererMode::U0x80) != RendererMode::None)
            {
                glEnable(GL_DEPTH_TEST);
                glDepthMask(GL_TRUE);
                glDepthFunc(GL_ALWAYS);
            }
            else if ((mode & RendererMode::U0x40) != RendererMode::None)
            {
                glEnable(GL_DEPTH_TEST);
                glDepthMask(GL_FALSE);
                glDepthFunc(GL_LEQUAL);
            }
            else
            {
                glDisable(GL_DEPTH_TEST);
                glDepthMask(GL_FALSE);
                glDepthFunc(GL_ALWAYS);
            }
        }

        State.GL.Mode.Mode = mode;
    }
}