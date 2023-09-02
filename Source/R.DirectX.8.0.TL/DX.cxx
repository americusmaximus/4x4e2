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

#include "DX.hxx"
#include "Module.hxx"
#include "Quit.hxx"

#include <math.h>

using namespace Mathematics;
using namespace Renderer::Module;
using namespace Renderers;
using namespace Settings;

namespace DX
{
    u32 Alpha(const f32 value)
    {
        return ((u32)roundf(255.0f * value) & 0xff) << 24;
    }

    u32 Color(const f32 r, const f32 g, const f32 b, const f32 a)
    {
        const auto ac = (u32)roundf(255.0f * a) & 0xff;
        const auto rc = (u32)roundf(255.0f * r) & 0xff;
        const auto gc = (u32)roundf(255.0f * g) & 0xff;
        const auto bc = (u32)roundf(255.0f * b) & 0xff;

        return (ac << 24) | (rc << 16) | (gc << 8) | (bc << 0);
    }

    void Initialize(void)
    {
        for (u32 x = 0; x < MAX_STATE_VARIABLE_COUNT; x++)
        {
            State.DX.DirectX.State.Values[x] = 0;
        }

        for (u32 x = 0; x < MAX_SURFACE_STAGE_COUNT; x++)
        {
            for (u32 xx = 0; xx < MAX_SURFACE_STAGE_VALUES_COUNT; xx++)
            {
                State.DX.Surfaces.Staging.Stages[x][xx] = 0;
            }
        }

        if (State.Settings.FixedPipelineMode == FixedPipelineOptionsMode::Off)
        {
            DXC(State.DX.DirectX.Device->CreateVertexShader(
                State.DX.Shaders.Vertex[(u32)DXVSN::DirLight].Declaration,
                State.DX.Shaders.Vertex[(u32)DXVSN::DirLight].Function,
                &State.DX.Shaders.Vertex[(u32)DXVSN::DirLight].Handle, D3DUSAGE_NONE),
                "Unable to create directional light vertex shader.");

            DXC(State.DX.DirectX.Device->CreateVertexShader(
                State.DX.Shaders.Vertex[(u32)DXVSN::TexGen].Declaration,
                State.DX.Shaders.Vertex[(u32)DXVSN::TexGen].Function,
                &State.DX.Shaders.Vertex[(u32)DXVSN::TexGen].Handle, D3DUSAGE_NONE),
                "Unable to create tex gen vertex shader.");

            DXC(State.DX.DirectX.Device->CreateVertexShader(
                State.DX.Shaders.Vertex[(u32)DXVSN::Cubic].Declaration,
                State.DX.Shaders.Vertex[(u32)DXVSN::Cubic].Function,
                &State.DX.Shaders.Vertex[(u32)DXVSN::Cubic].Handle, D3DUSAGE_NONE),
                "Unable to create cubic vertex shader.");

            DXC(State.DX.DirectX.Device->CreateVertexShader(
                State.DX.Shaders.Vertex[(u32)DXVSN::PreLit].Declaration,
                State.DX.Shaders.Vertex[(u32)DXVSN::PreLit].Function,
                &State.DX.Shaders.Vertex[(u32)DXVSN::PreLit].Handle, D3DUSAGE_NONE),
                "Unable to create pre-lit vertex shader.");

            DXC(State.DX.DirectX.Device->CreateVertexShader(
                State.DX.Shaders.Vertex[(u32)DXVSN::PreLitTexGen].Declaration,
                State.DX.Shaders.Vertex[(u32)DXVSN::PreLitTexGen].Function,
                &State.DX.Shaders.Vertex[(u32)DXVSN::PreLitTexGen].Handle, D3DUSAGE_NONE),
                "Unable to create pre-lit tex gen vertex shader.");

            DXC(State.DX.DirectX.Device->CreateVertexShader(
                State.DX.Shaders.Vertex[(u32)DXVSN::Bump].Declaration,
                State.DX.Shaders.Vertex[(u32)DXVSN::Bump].Function,
                &State.DX.Shaders.Vertex[(u32)DXVSN::Bump].Handle, D3DUSAGE_NONE),
                "Unable to create bump vertex shader.");

            DXC(State.DX.DirectX.Device->CreateVertexShader(
                State.DX.Shaders.Vertex[(u32)DXVSN::BumpSpec].Declaration,
                State.DX.Shaders.Vertex[(u32)DXVSN::BumpSpec].Function,
                &State.DX.Shaders.Vertex[(u32)DXVSN::BumpSpec].Handle, D3DUSAGE_NONE),
                "Unable to create bump specular vertex shader.");

            DXC(State.DX.DirectX.Device->CreateVertexShader(
                State.DX.Shaders.Vertex[(u32)DXVSN::BumpCubic].Declaration,
                State.DX.Shaders.Vertex[(u32)DXVSN::BumpCubic].Function,
                &State.DX.Shaders.Vertex[(u32)DXVSN::BumpCubic].Handle, D3DUSAGE_NONE),
                "Unable to create bump cubic vertex shader.");

            DXC(State.DX.DirectX.Device->CreateVertexShader(
                State.DX.Shaders.Vertex[(u32)DXVSN::BumpDiffuseTexGen].Declaration,
                State.DX.Shaders.Vertex[(u32)DXVSN::BumpDiffuseTexGen].Function,
                &State.DX.Shaders.Vertex[(u32)DXVSN::BumpDiffuseTexGen].Handle, D3DUSAGE_NONE),
                "Unable to create bump diffuse tex gen vertex shader.");

            DXC(State.DX.DirectX.Device->CreateVertexShader(
                State.DX.Shaders.Vertex[(u32)DXVSN::BumpSpecTexGen].Declaration,
                State.DX.Shaders.Vertex[(u32)DXVSN::BumpSpecTexGen].Function,
                &State.DX.Shaders.Vertex[(u32)DXVSN::BumpSpecTexGen].Handle, D3DUSAGE_NONE),
                "Unable to create bump spec tex gen vertex shader.");

            DXC(State.DX.DirectX.Device->CreateVertexShader(
                State.DX.Shaders.Vertex[(u32)DXVSN::AlphaFog].Declaration,
                State.DX.Shaders.Vertex[(u32)DXVSN::AlphaFog].Function,
                &State.DX.Shaders.Vertex[(u32)DXVSN::AlphaFog].Handle, D3DUSAGE_NONE),
                "Unable to create alpha fog vertex shader.");

            DXC(State.DX.DirectX.Device->CreateVertexShader(
                State.DX.Shaders.Vertex[(u32)DXVSN::Gloss].Declaration,
                State.DX.Shaders.Vertex[(u32)DXVSN::Gloss].Function,
                &State.DX.Shaders.Vertex[(u32)DXVSN::Gloss].Handle, D3DUSAGE_NONE),
                "Unable to create gloss vertex shader.");

            DXC(State.DX.DirectX.Device->CreateVertexShader(
                State.DX.Shaders.Vertex[(u32)DXVSN::ProjectedShadow].Declaration,
                State.DX.Shaders.Vertex[(u32)DXVSN::ProjectedShadow].Function,
                &State.DX.Shaders.Vertex[(u32)DXVSN::ProjectedShadow].Handle, D3DUSAGE_NONE),
                "Unable to create projected shadow back face culling vertex shader.");
        }
        else
        {
            State.DX.Shaders.Vertex[(u32)DXVSN::AlphaFog].FVF = D3DFVF_TEX1 | D3DFVF_NORMAL | D3DFVF_XYZ;
            State.DX.Shaders.Vertex[(u32)DXVSN::Bump].FVF = D3DFVF_TEX0;
            State.DX.Shaders.Vertex[(u32)DXVSN::BumpCubic].FVF = D3DFVF_TEX0;
            State.DX.Shaders.Vertex[(u32)DXVSN::BumpDiffuseTexGen].FVF = D3DFVF_TEX0;
            State.DX.Shaders.Vertex[(u32)DXVSN::BumpSpec].FVF = D3DFVF_TEX0;
            State.DX.Shaders.Vertex[(u32)DXVSN::BumpSpecTexGen].FVF = D3DFVF_TEX0;
            State.DX.Shaders.Vertex[(u32)DXVSN::Cubic].FVF = D3DFVF_TEX1 | D3DFVF_NORMAL | D3DFVF_XYZ;
            State.DX.Shaders.Vertex[(u32)DXVSN::DirLight].FVF = D3DFVF_TEX1 | D3DFVF_NORMAL | D3DFVF_XYZ;
            State.DX.Shaders.Vertex[(u32)DXVSN::Gloss].FVF = D3DFVF_TEX1 | D3DFVF_NORMAL | D3DFVF_XYZ;
            State.DX.Shaders.Vertex[(u32)DXVSN::PreLit].FVF = D3DFVF_TEX1 | D3DFVF_DIFFUSE | D3DFVF_XYZ;
            State.DX.Shaders.Vertex[(u32)DXVSN::PreLitTexGen].FVF = D3DFVF_TEX1 | D3DFVF_DIFFUSE | D3DFVF_XYZ;
            State.DX.Shaders.Vertex[(u32)DXVSN::ProjectedShadow].FVF = D3DFVF_TEX1 | D3DFVF_NORMAL | D3DFVF_XYZ;
            State.DX.Shaders.Vertex[(u32)DXVSN::TexGen].FVF = D3DFVF_TEX1 | D3DFVF_NORMAL | D3DFVF_XYZ;
        }

        if (State.Settings.FixedPipelineMode == FixedPipelineOptionsMode::Off)
        {
            DXC(State.DX.DirectX.Device->CreatePixelShader(State.DX.Shaders.Pixel[(u32)DXPSN::Decal].Function,
                &State.DX.Shaders.Pixel[(u32)DXPSN::Decal].Handle), "Unable to create decal pixel shader.");

            DXC(State.DX.DirectX.Device->CreatePixelShader(State.DX.Shaders.Pixel[(u32)DXPSN::DecalTexKill].Function,
                &State.DX.Shaders.Pixel[(u32)DXPSN::DecalTexKill].Handle), "Unable to create decal tex kill pixel shader.");

            DXC(State.DX.DirectX.Device->CreatePixelShader(State.DX.Shaders.Pixel[(u32)DXPSN::Bump].Function,
                &State.DX.Shaders.Pixel[(u32)DXPSN::Bump].Handle), "Unable to create bump pixel shader.");

            DXC(State.DX.DirectX.Device->CreatePixelShader(State.DX.Shaders.Pixel[(u32)DXPSN::BumpSpec].Function,
                &State.DX.Shaders.Pixel[(u32)DXPSN::BumpSpec].Handle), "Unable to create bump spec pixel shader.");

            DXC(State.DX.DirectX.Device->CreatePixelShader(State.DX.Shaders.Pixel[(u32)DXPSN::BumpCubic].Function,
                &State.DX.Shaders.Pixel[(u32)DXPSN::BumpCubic].Handle), "Unable to create bump cube pixel shader.");

            DXC(State.DX.DirectX.Device->CreatePixelShader(State.DX.Shaders.Pixel[(u32)DXPSN::BumpSpecTexGen].Function,
                &State.DX.Shaders.Pixel[(u32)DXPSN::BumpSpecTexGen].Handle), "Unable to create bump spec gen pixel shader.");

            DXC(State.DX.DirectX.Device->CreatePixelShader(State.DX.Shaders.Pixel[(u32)DXPSN::Detail].Function,
                &State.DX.Shaders.Pixel[(u32)DXPSN::Detail].Handle), "Unable to create detail pixel shader.");

            DXC(State.DX.DirectX.Device->CreatePixelShader(State.DX.Shaders.Pixel[(u32)DXPSN::Gloss].Function,
                &State.DX.Shaders.Pixel[(u32)DXPSN::Gloss].Handle), "Unable to create gloss pixel shader.");
        }
        else
        {
            State.DX.Shaders.Pixel[(u32)DXPSN::Decal].Handle = NULL;
            State.DX.Shaders.Pixel[(u32)DXPSN::DecalTexKill].Handle = NULL;
            State.DX.Shaders.Pixel[(u32)DXPSN::Bump].Handle = NULL;
            State.DX.Shaders.Pixel[(u32)DXPSN::BumpSpec].Handle = NULL;
            State.DX.Shaders.Pixel[(u32)DXPSN::BumpCubic].Handle = NULL;
            State.DX.Shaders.Pixel[(u32)DXPSN::Detail].Handle = NULL;
            State.DX.Shaders.Pixel[(u32)DXPSN::Gloss].Handle = NULL;
        }

        const D3DVIEWPORT8 viewport =
        {
            .X = 0,
            .Y = 0,
            .Width = State.Window.Width,
            .Height = State.Window.Height,
            .MinZ = 0.0f,
            .MaxZ = 1.0f
        };

        State.DX.DirectX.Device->SetViewport(&viewport);

        State.DX.Surfaces.Formats.Opaque = D3DFORMAT::D3DFMT_R5G6B5;
        State.DX.Surfaces.Formats.Transparent = State.Window.BitsPerPixel == GRAPHICS_BITS_PER_PIXEL_16
            ? D3DFORMAT::D3DFMT_A4R4G4B4 : D3DFORMAT::D3DFMT_A8R8G8B8;

        for (u32 x = 0; x < MAX_MIP_MAP_SURFACE_COUNT; x++)
        {
            u32 dims = 1 << (x & 0x1f);

            DXC(State.DX.DirectX.Device->CreateImageSurface(dims, dims,
                State.DX.Surfaces.Formats.Transparent, &State.DX.Surfaces.Transparent[x]),
                "Unable to create alpha source buffer.");

            DXC(State.DX.DirectX.Device->CreateImageSurface(dims, dims,
                State.DX.Surfaces.Formats.Opaque, &State.DX.Surfaces.Opaque[x]),
                "Unable to create 565 source buffer.");
        }

        DXC(State.DX.DirectX.Device->CreateVertexBuffer(State.Settings.Capacities.RVX * sizeof(RVX),
            D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFVF_TEX1 | D3DFVF_NORMAL | D3DFVF_XYZ,
            D3DPOOL::D3DPOOL_DEFAULT, &State.DX.Buffers.RVX), "Unable to create RVX vertex buffer");

        DXC(State.DX.DirectX.Device->CreateVertexBuffer(State.Settings.Capacities.RLVX * sizeof(RLVX),
            D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFVF_TEX1 | D3DFVF_DIFFUSE | D3DFVF_XYZ,
            D3DPOOL::D3DPOOL_DEFAULT, &State.DX.Buffers.RLVX), "Unable to create RLVX vertex buffer");

        DXC(State.DX.DirectX.Device->CreateVertexBuffer(State.Settings.Capacities.RTLVX * sizeof(RTLVX),
            D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFVF_TEX1 | D3DFVF_SPECULAR | D3DFVF_DIFFUSE | D3DFVF_XYZRHW,
            D3DPOOL::D3DPOOL_DEFAULT, &State.DX.Buffers.RTLVX), "Unable to create RTLVX vertex buffer");

        DXC(State.DX.DirectX.Device->CreateIndexBuffer(State.Settings.Capacities.Index * sizeof(u16) * GRAPHICS_TRIANGLE_VERTEX_COUNT,
            D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFORMAT::D3DFMT_INDEX16, D3DPOOL::D3DPOOL_DEFAULT,
            &State.DX.Buffers.Index), "Unable to create index buffer");

        for (u32 x = 0; x < State.Settings.TextureCount; x++)
        {
            if (State.DX.DirectX.Device->CreateTexture(State.Settings.TextureSize, State.Settings.TextureSize,
                1, D3DUSAGE_RENDERTARGET, State.DX.Devices.Current.PresentParams.BackBufferFormat,
                D3DPOOL::D3DPOOL_DEFAULT, &State.DX.Textures.Surfaces.Textures[x]) != D3D_OK)
            {
                State.Settings.TextureCount = 0; break;
            }
        }

        if (State.Settings.TextureCount != 0)
        {
            DXC(State.DX.DirectX.Device->CreateDepthStencilSurface(State.Settings.TextureSize, State.Settings.TextureSize,
                State.DX.Devices.Current.PresentParams.AutoDepthStencilFormat, D3DMULTISAMPLE_TYPE::D3DMULTISAMPLE_NONE,
                &State.DX.Surfaces.TextureDepthSurface), "Unable to create render texture depth buffer.");
        }

        if (!InitializeState()) { Quit("Unable to set DirectX into a known state."); }
    }

    BOOL InitializeState(void)
    {
        for (u32 x = 0; x < MAX_STATE_VARIABLE_COUNT; x++)
        {
            State.DX.DirectX.State.Values[x] = U32_MAX;
        }

        if (State.DX.DirectX.Device->BeginScene() != D3D_OK) { return FALSE; }

        SetRendererState(D3DRENDERSTATETYPE::D3DRS_FILLMODE, D3DFILLMODE::D3DFILL_SOLID);
        SetRendererState(D3DRENDERSTATETYPE::D3DRS_SHADEMODE, D3DSHADEMODE::D3DSHADE_GOURAUD);
        SetRendererState(D3DRENDERSTATETYPE::D3DRS_ALPHATESTENABLE, FALSE);
        SetRendererState(D3DRENDERSTATETYPE::D3DRS_LASTPIXEL, FALSE);
        SetRendererState(D3DRENDERSTATETYPE::D3DRS_SRCBLEND, D3DBLEND::D3DBLEND_ONE);
        SetRendererState(D3DRENDERSTATETYPE::D3DRS_DESTBLEND, D3DBLEND::D3DBLEND_INVSRCALPHA);
        SetRendererState(D3DRENDERSTATETYPE::D3DRS_CULLMODE, D3DCULL::D3DCULL_NONE);
        SetRendererState(D3DRENDERSTATETYPE::D3DRS_DITHERENABLE, TRUE);
        SetRendererState(D3DRENDERSTATETYPE::D3DRS_ALPHABLENDENABLE, FALSE);
        SetRendererState(D3DRENDERSTATETYPE::D3DRS_FOGENABLE, FALSE);
        SetRendererState(D3DRENDERSTATETYPE::D3DRS_FOGCOLOR, GRAPHICS_COLOR_BLACK);
        SetRendererState(D3DRENDERSTATETYPE::D3DRS_SPECULARENABLE, FALSE);
        SetRendererState(D3DRENDERSTATETYPE::D3DRS_ZVISIBLE, FALSE);
        SetRendererState(D3DRENDERSTATETYPE::D3DRS_ZENABLE, D3DZBUFFERTYPE::D3DZB_FALSE);
        SetRendererState(D3DRENDERSTATETYPE::D3DRS_ZWRITEENABLE, FALSE);
        SetRendererState(D3DRENDERSTATETYPE::D3DRS_ZFUNC, D3DCMPFUNC::D3DCMP_ALWAYS);
        SetRendererState(D3DRENDERSTATETYPE::D3DRS_ALPHAREF, 0);
        SetRendererState(D3DRENDERSTATETYPE::D3DRS_ALPHAFUNC, D3DCMPFUNC::D3DCMP_GREATER);
        SetRendererState(D3DRENDERSTATETYPE::D3DRS_ALPHATESTENABLE, FALSE);
        SetRendererState(D3DRENDERSTATETYPE::D3DRS_CLIPPING, TRUE);
        SetRendererState(D3DRENDERSTATETYPE::D3DRS_LIGHTING, FALSE);
        SetRendererState(D3DRENDERSTATETYPE::D3DRS_STENCILZFAIL, D3DSTENCILOP::D3DSTENCILOP_KEEP);
        SetRendererState(D3DRENDERSTATETYPE::D3DRS_STENCILFAIL, D3DSTENCILOP::D3DSTENCILOP_KEEP);
        SetRendererState(D3DRENDERSTATETYPE::D3DRS_STENCILREF, 1);
        SetRendererState(D3DRENDERSTATETYPE::D3DRS_STENCILMASK, U32_MAX);
        SetRendererState(D3DRENDERSTATETYPE::D3DRS_STENCILWRITEMASK, U32_MAX);

        for (u32 x = 0; x < MAX_SURFACE_STAGE_COUNT; x++)
        {
            State.DX.Surfaces.Staging.Textures[x] = NULL;
            State.DX.Textures.Surfaces.Staging[x] = NULL;
        }

        State.DX.Mode.Mode = RendererMode::None;

        return State.DX.DirectX.Device->EndScene() == D3D_OK;
    }

    void Release(void)
    {
        ReleaseOptimizedMeshes();

        if (State.Settings.FixedPipelineMode == FixedPipelineOptionsMode::Off
            && State.DX.DirectX.Device != NULL)
        {
            for (u32 x = 0; x < MAX_VERTEX_SHADER_COUNT; x++)
            {
                if (State.DX.Shaders.Vertex[x].Handle != NULL)
                {
                    State.DX.DirectX.Device->DeleteVertexShader(State.DX.Shaders.Vertex[x].Handle);
                    State.DX.Shaders.Vertex[x].Handle = NULL;
                }
            }

            for (u32 x = 0; x < MAX_PIXEL_SHADER_COUNT; x++)
            {
                if (State.DX.Shaders.Pixel[x].Handle != NULL)
                {
                    State.DX.DirectX.Device->DeletePixelShader(State.DX.Shaders.Pixel[x].Handle);
                    State.DX.Shaders.Pixel[x].Handle = NULL;
                }
            }
        }

        for (u32 x = 0; x < MAX_MIP_MAP_SURFACE_COUNT; x++)
        {
            if (State.DX.Surfaces.Transparent[x] != NULL)
            {
                State.DX.Surfaces.Transparent[x]->Release();
                State.DX.Surfaces.Transparent[x] = NULL;
            }

            if (State.DX.Surfaces.Opaque[x] != NULL)
            {
                State.DX.Surfaces.Opaque[x]->Release();
                State.DX.Surfaces.Opaque[x] = NULL;
            }
        }

        for (u32 x = 0; x < State.Settings.TextureCount; x++)
        {
            if (State.DX.Textures.Surfaces.Textures[x] != NULL)
            {
                State.DX.Textures.Surfaces.Textures[x]->Release();
                State.DX.Textures.Surfaces.Textures[x] = NULL;
            }
        }

        if (State.DX.Surfaces.TextureDepthSurface != NULL)
        {
            State.DX.Surfaces.TextureDepthSurface->Release();
            State.DX.Surfaces.TextureDepthSurface = NULL;
        }

        for (u32 x = 0; x < MAX_TEXTURE_COUNT; x++)
        {
            if (State.DX.Textures.Textures[x].Texture != NULL)
            {
                State.DX.Textures.Textures[x].Texture->Release();
                State.DX.Textures.Textures[x].Texture = NULL;
            }
        }

        if (State.DX.Buffers.RVX != NULL)
        {
            State.DX.Buffers.RVX->Release();
            State.DX.Buffers.RVX = NULL;
        }

        if (State.DX.Buffers.RLVX != NULL)
        {
            State.DX.Buffers.RLVX->Release();
            State.DX.Buffers.RLVX = NULL;
        }

        if (State.DX.Buffers.RTLVX != NULL)
        {
            State.DX.Buffers.RTLVX->Release();
            State.DX.Buffers.RTLVX = NULL;
        }

        if (State.DX.Buffers.Index != NULL)
        {
            State.DX.Buffers.Index->Release();
            State.DX.Buffers.Index = NULL;
        }

        ZeroMemory(&State.DX.Textures.Textures, MAX_TEXTURE_COUNT * sizeof(DX::DXTexture));

        for (u32 x = 0; x < MAX_MIP_MAP_SURFACE_COUNT; x++)
        {
            State.DX.Surfaces.Transparent[x] = NULL;
            State.DX.Surfaces.Opaque[x] = NULL;
        }
    }

    void ReleaseOptimizedMeshes(void)
    {
        for (u32 x = 0; x < MAX_OPTIMIZED_MESH_COUNT; x++)
        {
            if (State.DX.Meshes[x].Buffers.RVX != NULL)
            {
                State.DX.Meshes[x].Buffers.RVX->Release();
                State.DX.Meshes[x].Buffers.RVX = NULL;
            }

            if (State.DX.Meshes[x].Buffers.RBVX != NULL)
            {
                State.DX.Meshes[x].Buffers.RBVX->Release();
                State.DX.Meshes[x].Buffers.RBVX = NULL;
            }
        }
    }

    BOOL SelectLight(void)
    {
        if (State.DX.DirectX.Device == NULL) { return FALSE; }

        State.DX.Mode.IsChanged = TRUE;

        if (State.Settings.FixedPipelineMode == FixedPipelineOptionsMode::On)
        {
            State.DX.Light.Light.Type = D3DLIGHTTYPE::D3DLIGHT_DIRECTIONAL;

            State.DX.Light.Light.Specular.r = 0.0f;
            State.DX.Light.Light.Specular.g = 0.0f;
            State.DX.Light.Light.Specular.b = 0.0f;
            State.DX.Light.Light.Specular.a = 0.0f;

            State.DX.Light.Light.Position.x = 0.0f;
            State.DX.Light.Light.Position.y = 0.0f;
            State.DX.Light.Light.Position.z = 0.0f;

            State.DX.Light.Light.Range = 0.0f;
            State.DX.Light.Light.Falloff = 0.0f;

            State.DX.Light.Light.Attenuation0 = 0.0f;
            State.DX.Light.Light.Attenuation1 = 0.0f;
            State.DX.Light.Light.Attenuation2 = 0.0f;

            State.DX.Light.Light.Theta = 0.0f;
            State.DX.Light.Light.Phi = 0.0f;

            if (State.DX.Light.Light.Direction.x == 0.0f && State.DX.Light.Light.Direction.y == 0.0f && State.DX.Light.Light.Direction.z == 0.0f)
            {
                State.DX.Light.Light.Direction.x = 1.0f;
            }

            DX::DXC(State.DX.DirectX.Device->SetLight(0, &State.DX.Light.Light), "Unable to set light.");

            State.DX.DirectX.Device->LightEnable(0, TRUE);
        }

        return TRUE;
    }

    void SelectRendererMode(const RendererMode mode)
    {
        SetRendererState(D3DRENDERSTATETYPE::D3DRS_SRCBLEND, State.DX.Mode.Blend.SourceBlendMode);
        SetRendererState(D3DRENDERSTATETYPE::D3DRS_DESTBLEND, State.DX.Mode.Blend.DestinationBlendMode);

        if (((State.DX.Mode.Mode ^ mode) & RendererMode::U0x1) != RendererMode::None)
        {
            if ((mode & RendererMode::U0x1) == RendererMode::None)
            {
                SelectTextureStageValue(TEXTURE_STAGE_0_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_COLOROP, D3DTEXTUREOP::D3DTOP_DISABLE);
            }
            else
            {
                State.DX.Surfaces.Staging.Textures[0] = NULL;
            }
        }

        u32 indexes[MAX_BLEND_TEXTURE_COUNT] = { 0, U32_MAX, U32_MAX, U32_MAX };

        switch (State.DX.Mode.Blend.BlendOperation)
        {
        case RendererBlendOperation::Disable:
        {
            indexes[0] = 1;
            indexes[2] = 0;
            break;
        }
        case RendererBlendOperation::SelectArgument1:
        {
            indexes[0] = 1;

            if ((mode & RendererMode::U0x400) != RendererMode::None)
            {
                indexes[2] = 0;
            }

            break;
        }
        case RendererBlendOperation::SelectArgument2:
        {
            indexes[0] = 2;
            break;
        }
        case RendererBlendOperation::Modulate:
        {
            indexes[0] = 1;
            indexes[3] = 2;
            break;
        }
        case RendererBlendOperation::Modulate4X:
        {
            indexes[0] = 0;
            indexes[1] = 1;
            break;
        }
        case RendererBlendOperation::Add:
        {
            indexes[0] = 0;
            indexes[1] = 1;
            indexes[2] = 2;
            break;
        }
        }

        IDirect3DTexture8* tex[MAX_BLEND_TEXTURE_COUNT] = { NULL, NULL, NULL, NULL };

        for (u32 x = 0; x < MAX_BLEND_TEXTURE_COUNT; x++)
        {
            if (indexes[x] != U32_MAX)
            {
                tex[x] = State.DX.Textures.Surfaces.Staging[indexes[x]];
            }
        }

        if (tex[0] != State.DX.Surfaces.Staging.Textures[0]
            || tex[1] != State.DX.Surfaces.Staging.Textures[1]
            || tex[2] != State.DX.Surfaces.Staging.Textures[2]
            || tex[3] != State.DX.Surfaces.Staging.Textures[3]
            || State.DX.Mode.Blend.BlendOperation != State.DX.Mode.Clamp.ClampOperation
            || (((State.DX.Mode.Mode ^ mode) & RendererMode::U0x400) != RendererMode::None))
        {
            for (u32 x = 0; x < MAX_BLEND_TEXTURE_COUNT; x++)
            {
                State.DX.Surfaces.Staging.Textures[x] = tex[x];
                State.DX.DirectX.Device->SetTexture(x, State.DX.Surfaces.Staging.Textures[x]);

                if (State.DX.Surfaces.Staging.Textures[x] != NULL)
                {
                    SelectTextureStageValue(x, D3DTEXTURESTAGESTATETYPE::D3DTSS_MAGFILTER, D3DTEXTUREFILTERTYPE::D3DTEXF_LINEAR);
                    SelectTextureStageValue(x, D3DTEXTURESTAGESTATETYPE::D3DTSS_MINFILTER, D3DTEXTUREFILTERTYPE::D3DTEXF_LINEAR);
                    SelectTextureStageValue(x, D3DTEXTURESTAGESTATETYPE::D3DTSS_MIPFILTER, D3DTEXTUREFILTERTYPE::D3DTEXF_LINEAR);

                    const auto indx = indexes[x];

                    SelectTextureStageValue(x, D3DTEXTURESTAGESTATETYPE::D3DTSS_ADDRESSU,
                        State.DX.Mode.Clamp.U[indx] == RendererClampMode::Wrap
                        ? D3DTEXTUREADDRESS::D3DTADDRESS_WRAP : D3DTEXTUREADDRESS::D3DTADDRESS_CLAMP);
                    SelectTextureStageValue(x, D3DTEXTURESTAGESTATETYPE::D3DTSS_ADDRESSV,
                        State.DX.Mode.Clamp.V[indx] == RendererClampMode::Wrap
                        ? D3DTEXTUREADDRESS::D3DTADDRESS_WRAP : D3DTEXTUREADDRESS::D3DTADDRESS_CLAMP);
                }
            }

            switch (State.DX.Mode.Blend.BlendOperation)
            {
            case RendererBlendOperation::None:
            case RendererBlendOperation::Modulate2X:
            {
                SelectTextureStageValue(TEXTURE_STAGE_0_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU);
                SelectTextureStageValue(TEXTURE_STAGE_0_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_COLOROP, D3DTEXTUREOP::D3DTOP_MODULATE);
                SelectTextureStageValue(TEXTURE_STAGE_0_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_COLORARG1, D3DTA_TEXTURE);
                SelectTextureStageValue(TEXTURE_STAGE_0_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_COLORARG2, D3DTA_DIFFUSE);
                SelectTextureStageValue(TEXTURE_STAGE_0_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_ALPHAOP, D3DTEXTUREOP::D3DTOP_MODULATE);
                SelectTextureStageValue(TEXTURE_STAGE_0_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
                SelectTextureStageValue(TEXTURE_STAGE_0_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

                SelectTextureStageValue(TEXTURE_STAGE_1_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_COLOROP, D3DTEXTUREOP::D3DTOP_DISABLE);
                SelectTextureStageValue(TEXTURE_STAGE_1_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_ALPHAOP, D3DTEXTUREOP::D3DTOP_DISABLE);

                SelectTextureStageValue(TEXTURE_STAGE_2_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_COLOROP, D3DTEXTUREOP::D3DTOP_DISABLE);
                SelectTextureStageValue(TEXTURE_STAGE_2_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_ALPHAOP, D3DTEXTUREOP::D3DTOP_DISABLE);

                SelectTextureStageValue(TEXTURE_STAGE_3_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_COLOROP, D3DTEXTUREOP::D3DTOP_DISABLE);
                SelectTextureStageValue(TEXTURE_STAGE_3_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_ALPHAOP, D3DTEXTUREOP::D3DTOP_DISABLE);

                break;
            }
            case  RendererBlendOperation::Disable:
            {
                SelectTextureStageValue(TEXTURE_STAGE_0_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU);
                SelectTextureStageValue(TEXTURE_STAGE_0_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_COLOROP, D3DTEXTUREOP::D3DTOP_DOTPRODUCT3);
                SelectTextureStageValue(TEXTURE_STAGE_0_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_COLORARG1, D3DTA_TEXTURE);
                SelectTextureStageValue(TEXTURE_STAGE_0_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_COLORARG2, D3DTA_DIFFUSE);
                SelectTextureStageValue(TEXTURE_STAGE_0_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_ALPHAOP, D3DTEXTUREOP::D3DTOP_SELECTARG1);
                SelectTextureStageValue(TEXTURE_STAGE_0_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
                SelectTextureStageValue(TEXTURE_STAGE_0_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

                SelectTextureStageValue(TEXTURE_STAGE_1_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU);
                SelectTextureStageValue(TEXTURE_STAGE_1_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_COLOROP, D3DTEXTUREOP::D3DTOP_ADD);
                SelectTextureStageValue(TEXTURE_STAGE_1_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_COLORARG1, D3DTA_CURRENT);
                SelectTextureStageValue(TEXTURE_STAGE_1_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_COLORARG2, D3DTA_TFACTOR);
                SelectTextureStageValue(TEXTURE_STAGE_1_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_ALPHAOP, D3DTEXTUREOP::D3DTOP_SELECTARG1);
                SelectTextureStageValue(TEXTURE_STAGE_1_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_ALPHAARG1, D3DTA_CURRENT);
                SelectTextureStageValue(TEXTURE_STAGE_1_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_ALPHAARG2, D3DTA_CURRENT);

                SelectTextureStageValue(TEXTURE_STAGE_2_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU);
                SelectTextureStageValue(TEXTURE_STAGE_2_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_COLOROP, D3DTEXTUREOP::D3DTOP_MODULATE);
                SelectTextureStageValue(TEXTURE_STAGE_2_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_COLORARG1, D3DTA_TEXTURE);
                SelectTextureStageValue(TEXTURE_STAGE_2_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_COLORARG2, D3DTA_CURRENT);
                SelectTextureStageValue(TEXTURE_STAGE_2_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_ALPHAOP, D3DTEXTUREOP::D3DTOP_MODULATE);
                SelectTextureStageValue(TEXTURE_STAGE_2_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
                SelectTextureStageValue(TEXTURE_STAGE_2_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_ALPHAARG2, D3DTA_CURRENT);

                u32 r = 0xff;
                u32 g = 0xff;
                u32 b = 0xff;

                if (State.DX.Light.Mode == RendererLightingMode::On)
                {
                    r = (u32)(255.0f * State.DX.Light.Light.Ambient.r);
                    g = (u32)(255.0f * State.DX.Light.Light.Ambient.g);
                    b = (u32)(255.0f * State.DX.Light.Light.Ambient.b);
                }

                State.DX.DirectX.Device->SetRenderState(D3DRENDERSTATETYPE::D3DRS_TEXTUREFACTOR, 0xff000000 | (r << 16) | (g << 8) | (b << 0));

                SelectTextureStageValue(TEXTURE_STAGE_3_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_COLOROP, D3DTEXTUREOP::D3DTOP_DISABLE);
                SelectTextureStageValue(TEXTURE_STAGE_3_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_ALPHAOP, D3DTEXTUREOP::D3DTOP_DISABLE);

                break;
            }
            case RendererBlendOperation::SelectArgument1:
            {
                SelectTextureStageValue(TEXTURE_STAGE_0_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU);
                SelectTextureStageValue(TEXTURE_STAGE_0_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_COLOROP, D3DTEXTUREOP::D3DTOP_DOTPRODUCT3);
                SelectTextureStageValue(TEXTURE_STAGE_0_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_COLORARG1, D3DTA_TEXTURE);
                SelectTextureStageValue(TEXTURE_STAGE_0_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_COLORARG2, D3DTA_DIFFUSE);
                SelectTextureStageValue(TEXTURE_STAGE_0_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_ALPHAOP, D3DTEXTUREOP::D3DTOP_SELECTARG1);
                SelectTextureStageValue(TEXTURE_STAGE_0_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
                SelectTextureStageValue(TEXTURE_STAGE_0_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

                if ((mode & RendererMode::U0x400) == RendererMode::None)
                {
                    SelectTextureStageValue(TEXTURE_STAGE_1_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_COLOROP, D3DTEXTUREOP::D3DTOP_DISABLE);
                    SelectTextureStageValue(TEXTURE_STAGE_1_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_ALPHAOP, D3DTEXTUREOP::D3DTOP_DISABLE);

                    SelectTextureStageValue(TEXTURE_STAGE_2_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_COLOROP, D3DTEXTUREOP::D3DTOP_DISABLE);
                    SelectTextureStageValue(TEXTURE_STAGE_2_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_ALPHAOP, D3DTEXTUREOP::D3DTOP_DISABLE);
                }
                else
                {
                    SelectTextureStageValue(TEXTURE_STAGE_1_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU);
                    SelectTextureStageValue(TEXTURE_STAGE_1_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_COLOROP, D3DTEXTUREOP::D3DTOP_SELECTARG1);
                    SelectTextureStageValue(TEXTURE_STAGE_1_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_COLORARG1, D3DTA_CURRENT);
                    SelectTextureStageValue(TEXTURE_STAGE_1_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_COLORARG2, D3DTA_CURRENT);
                    SelectTextureStageValue(TEXTURE_STAGE_1_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_ALPHAOP, D3DTEXTUREOP::D3DTOP_SELECTARG1);
                    SelectTextureStageValue(TEXTURE_STAGE_1_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_ALPHAARG1, D3DTA_CURRENT);
                    SelectTextureStageValue(TEXTURE_STAGE_1_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_ALPHAARG2, D3DTA_CURRENT);

                    SelectTextureStageValue(TEXTURE_STAGE_2_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU);
                    SelectTextureStageValue(TEXTURE_STAGE_2_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_COLOROP, D3DTEXTUREOP::D3DTOP_MODULATE);
                    SelectTextureStageValue(TEXTURE_STAGE_2_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_COLORARG1, D3DTA_TEXTURE);
                    SelectTextureStageValue(TEXTURE_STAGE_2_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_COLORARG2, D3DTA_CURRENT);
                    SelectTextureStageValue(TEXTURE_STAGE_2_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_ALPHAOP, D3DTEXTUREOP::D3DTOP_MODULATE);
                    SelectTextureStageValue(TEXTURE_STAGE_2_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
                    SelectTextureStageValue(TEXTURE_STAGE_2_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_ALPHAARG2, D3DTA_CURRENT);
                }

                SelectTextureStageValue(TEXTURE_STAGE_3_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_COLOROP, D3DTEXTUREOP::D3DTOP_DISABLE);
                SelectTextureStageValue(TEXTURE_STAGE_3_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_ALPHAOP, D3DTEXTUREOP::D3DTOP_DISABLE);

                break;
            }
            case RendererBlendOperation::SelectArgument2:
            {
                SelectTextureStageValue(TEXTURE_STAGE_0_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_COLOROP, D3DTEXTUREOP::D3DTOP_SELECTARG1);
                SelectTextureStageValue(TEXTURE_STAGE_0_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_COLORARG1, D3DTA_TEXTURE);

                SelectTextureStageValue(TEXTURE_STAGE_1_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_COLOROP, D3DTEXTUREOP::D3DTOP_DISABLE);
                SelectTextureStageValue(TEXTURE_STAGE_1_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_ALPHAOP, D3DTEXTUREOP::D3DTOP_DISABLE);

                SelectTextureStageValue(TEXTURE_STAGE_2_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_COLOROP, D3DTEXTUREOP::D3DTOP_DISABLE);
                SelectTextureStageValue(TEXTURE_STAGE_2_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_ALPHAOP, D3DTEXTUREOP::D3DTOP_DISABLE);

                SelectTextureStageValue(TEXTURE_STAGE_3_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_COLOROP, D3DTEXTUREOP::D3DTOP_DISABLE);
                SelectTextureStageValue(TEXTURE_STAGE_3_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_ALPHAOP, D3DTEXTUREOP::D3DTOP_DISABLE);

                break;
            }
            case RendererBlendOperation::Modulate:
            {
                SelectTextureStageValue(TEXTURE_STAGE_0_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU);
                SelectTextureStageValue(TEXTURE_STAGE_0_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_COLOROP, D3DTEXTUREOP::D3DTOP_MODULATE);
                SelectTextureStageValue(TEXTURE_STAGE_0_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_COLORARG1, D3DTA_TEXTURE);

                SelectTextureStageValue(TEXTURE_STAGE_1_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_COLOROP, D3DTEXTUREOP::D3DTOP_DISABLE);
                SelectTextureStageValue(TEXTURE_STAGE_1_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_ALPHAOP, D3DTEXTUREOP::D3DTOP_DISABLE);

                SelectTextureStageValue(TEXTURE_STAGE_2_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_COLOROP, D3DTEXTUREOP::D3DTOP_DISABLE);
                SelectTextureStageValue(TEXTURE_STAGE_2_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_ALPHAOP, D3DTEXTUREOP::D3DTOP_DISABLE);

                SelectTextureStageValue(TEXTURE_STAGE_3_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_COLOROP, D3DTEXTUREOP::D3DTOP_DISABLE);
                SelectTextureStageValue(TEXTURE_STAGE_3_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_ALPHAOP, D3DTEXTUREOP::D3DTOP_DISABLE);

                break;
            }
            }

            if (State.Settings.FixedPipelineMode == FixedPipelineOptionsMode::Off
                || (mode & RendererMode::U0x400) == RendererMode::None)
            {
                if (State.DX.Mode.Blend.BlendOperation == RendererBlendOperation::SelectArgument2)
                {
                    SelectTextureStageValue(TEXTURE_STAGE_0_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_TEXTURETRANSFORMFLAGS, D3DTEXTURETRANSFORMFLAGS::D3DTTFF_COUNT3);
                    SelectTextureStageValue(TEXTURE_STAGE_0_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR);
                }
                else
                {
                    SelectTextureStageValue(TEXTURE_STAGE_0_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_TEXTURETRANSFORMFLAGS, D3DTEXTURETRANSFORMFLAGS::D3DTTFF_DISABLE);
                    SelectTextureStageValue(TEXTURE_STAGE_0_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU);
                }
            }
            else
            {
                SelectTextureStageValue(TEXTURE_STAGE_0_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_TEXTURETRANSFORMFLAGS,
                    D3DTEXTURETRANSFORMFLAGS::D3DTTFF_PROJECTED | D3DTEXTURETRANSFORMFLAGS::D3DTTFF_COUNT3);
                SelectTextureStageValue(TEXTURE_STAGE_0_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION);
            }

            State.DX.Mode.Clamp.ClampOperation = State.DX.Mode.Blend.BlendOperation;
        }

        if (((State.DX.Mode.Mode ^ mode) & RendererMode::U0x2) != RendererMode::None)
        {
            if ((mode & RendererMode::U0x2) != RendererMode::None)
            {
                SetRendererState(D3DRENDERSTATETYPE::D3DRS_ALPHABLENDENABLE, TRUE);
                SetRendererState(D3DRENDERSTATETYPE::D3DRS_ALPHATESTENABLE, TRUE);
            }
            else
            {
                SetRendererState(D3DRENDERSTATETYPE::D3DRS_ALPHABLENDENABLE, FALSE);
                SetRendererState(D3DRENDERSTATETYPE::D3DRS_ALPHATESTENABLE, FALSE);
            }
        }

        if (((State.DX.Mode.Mode ^ mode) & RendererMode::U0x8) != RendererMode::None)
        {
            if ((mode & RendererMode::U0x8) == RendererMode::None)
            {
                SetRendererState(D3DRENDERSTATETYPE::D3DRS_FOGENABLE, FALSE);

                if (State.Settings.FixedPipelineMode != FixedPipelineOptionsMode::Off)
                {
                    SetRendererState(D3DRENDERSTATETYPE::D3DRS_FOGVERTEXMODE, D3DFOGMODE::D3DFOG_NONE);
                    SetRendererState(D3DRENDERSTATETYPE::D3DRS_FOGTABLEMODE, D3DFOGMODE::D3DFOG_NONE);
                }
            }
            else
            {
                SetRendererState(D3DRENDERSTATETYPE::D3DRS_FOGENABLE, TRUE);

                if (State.Settings.FixedPipelineMode != FixedPipelineOptionsMode::Off)
                {
                    SetRendererState(D3DRENDERSTATETYPE::D3DRS_FOGVERTEXMODE, D3DFOGMODE::D3DFOG_LINEAR);
                    SetRendererState(D3DRENDERSTATETYPE::D3DRS_FOGTABLEMODE, D3DFOGMODE::D3DFOG_NONE);
                }
            }
        }

        if (State.DX.Mode.StencilMode != RendererStencilMode::Off)
        {
            State.DX.Mode.Mode = mode;

            return;
        }

        if (((State.DX.Mode.Mode ^ mode) & (RendererMode::U0x80 | RendererMode::U0x40)) == RendererMode::None)
        {
            State.DX.Mode.Mode = mode;

            return;
        }

        if ((mode & RendererMode::U0x80) != RendererMode::None && (mode & RendererMode::U0x40) != RendererMode::None)
        {
            SetRendererState(D3DRENDERSTATETYPE::D3DRS_ZENABLE, State.DX.Mode.DepthBufferMode);
            SetRendererState(D3DRENDERSTATETYPE::D3DRS_ZWRITEENABLE, TRUE);
            SetRendererState(D3DRENDERSTATETYPE::D3DRS_ZFUNC, D3DCMPFUNC::D3DCMP_LESSEQUAL);
        }
        else if ((mode & RendererMode::U0x80) != RendererMode::None)
        {
            SetRendererState(D3DRENDERSTATETYPE::D3DRS_ZENABLE, State.DX.Mode.DepthBufferMode);
            SetRendererState(D3DRENDERSTATETYPE::D3DRS_ZWRITEENABLE, TRUE);
            SetRendererState(D3DRENDERSTATETYPE::D3DRS_ZFUNC, D3DCMPFUNC::D3DCMP_ALWAYS);
        }
        else if ((mode & RendererMode::U0x40) != RendererMode::None)
        {
            SetRendererState(D3DRENDERSTATETYPE::D3DRS_ZENABLE, State.DX.Mode.DepthBufferMode);
            SetRendererState(D3DRENDERSTATETYPE::D3DRS_ZWRITEENABLE, FALSE);
            SetRendererState(D3DRENDERSTATETYPE::D3DRS_ZFUNC, D3DCMPFUNC::D3DCMP_LESSEQUAL);
        }
        else
        {
            SetRendererState(D3DRENDERSTATETYPE::D3DRS_ZENABLE, D3DZBUFFERTYPE::D3DZB_FALSE);
            SetRendererState(D3DRENDERSTATETYPE::D3DRS_ZWRITEENABLE, FALSE);
            SetRendererState(D3DRENDERSTATETYPE::D3DRS_ZFUNC, D3DCMPFUNC::D3DCMP_ALWAYS);
        }

        State.DX.Mode.Mode = mode;
    }

    void SelectTextureStageValue(const u32 stage, const D3DTEXTURESTAGESTATETYPE type, const DWORD value)
    {
        if (State.DX.Surfaces.Staging.Stages[stage][type] != value)
        {
            State.DX.DirectX.Device->SetTextureStageState(stage, type, value);
            State.DX.Surfaces.Staging.Stages[stage][type] = value;
        }
    }

    void SetIndexes(const u16* indexes, const u32 count)
    {
        if (State.Settings.Capacities.Index < count) { Quit("Too many indexes."); }

        if (State.Settings.Capacities.Index < State.DX.Data.Indexes.Count + count)
        {
            State.DX.Data.Indexes.Count = 0;
        }

        const auto options = State.DX.Data.Indexes.Count == 0
            ? D3DLOCK_DISCARD | D3DLOCK_NOSYSLOCK
            : D3DLOCK_NOOVERWRITE | D3DLOCK_DISCARD | D3DLOCK_NOSYSLOCK;

        u16* buffer;
        DXC(State.DX.Buffers.Index->Lock(State.DX.Data.Indexes.Count * sizeof(u16) * GRAPHICS_TRIANGLE_VERTEX_COUNT,
            count * GRAPHICS_TRIANGLE_VERTEX_COUNT * sizeof(u16), (BYTE**)&buffer, options), "Unable to lock index buffer.");

        CopyMemory(buffer, indexes, count * sizeof(u16) * GRAPHICS_TRIANGLE_VERTEX_COUNT);

        DXC(State.DX.Buffers.Index->Unlock(), "Unable to unlock index buffer.");
    }

    void SetIndexSource(const u32 index, IDirect3DIndexBuffer8* buffer)
    {
        DX::DXC(State.DX.DirectX.Device->SetIndices(buffer, index), "Unable to set index source.");
    }

    void SetRendererState(const D3DRENDERSTATETYPE state, const DWORD value)
    {
        if (State.DX.DirectX.State.Values[state] != value)
        {
            State.DX.DirectX.State.Values[state] = value;

            State.DX.DirectX.Device->SetRenderState(state, value);
        }
    }

    void SetShaderConstants(void)
    {
        if (State.Settings.FixedPipelineMode == FixedPipelineOptionsMode::On) { return; }

        if (State.DX.Light.Mode == RendererLightingMode::Off)
        {
            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_7_INDEX].X = 1.0f;
            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_7_INDEX].Y = 1.0f;
            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_7_INDEX].Z = 1.0f;

            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_8_INDEX].X = 0.0f;
            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_8_INDEX].Y = 0.0f;
            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_8_INDEX].Z = 0.0f;
        }
        else
        {
            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_7_INDEX].X = State.DX.Light.Light.Ambient.r;
            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_7_INDEX].Y = State.DX.Light.Light.Ambient.g;
            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_7_INDEX].Z = State.DX.Light.Light.Ambient.b;

            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_8_INDEX].X = State.DX.Light.Light.Diffuse.r;
            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_8_INDEX].Y = State.DX.Light.Light.Diffuse.g;
            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_8_INDEX].Z = State.DX.Light.Light.Diffuse.b;
        }

        State.DX.Shaders.Constants.Staging[SHADER_REGISTER_7_INDEX].W = 0.0f;

        State.DX.Shaders.Constants.Staging[SHADER_REGISTER_8_INDEX].W = 0.0f;

        if (State.DX.Mode.Blend.BlendOperation != RendererBlendOperation::None)
        {
            const f32x4 constants[MAX_PIXEL_SHADER_REGISTER_COUNT]
            {
                {
                    .X = State.DX.Shaders.Constants.Staging[SHADER_REGISTER_7_INDEX].X,
                    .Y = State.DX.Shaders.Constants.Staging[SHADER_REGISTER_7_INDEX].Y,
                    .Z = State.DX.Shaders.Constants.Staging[SHADER_REGISTER_7_INDEX].Z,
                    .W = 0.0f
                },
                {.X = 0.0f, .Y = 0.0f, .Z = 0.0f, .W = State.DX.Gloss }
            };

            DX::DXC(State.DX.DirectX.Device->SetPixelShaderConstant(SHADER_REGISTER_0_INDEX, constants,
                MAX_PIXEL_SHADER_REGISTER_COUNT), "Unable to set pixel shader constants.");
        }

        if (State.DX.Mode.IsChanged)
        {
            State.DX.Mode.IsChanged = FALSE;

            Matrix4x4 matrix;
            MultiplyMatrix(&matrix, &State.DX.Transform.ModelView, &State.DX.Transform.Projection);

            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_0_INDEX].X = matrix.M11;
            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_0_INDEX].Y = matrix.M21;
            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_0_INDEX].Z = matrix.M31;
            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_0_INDEX].W = matrix.M41;

            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_1_INDEX].X = matrix.M12;
            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_1_INDEX].Y = matrix.M22;
            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_1_INDEX].Z = matrix.M32;
            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_1_INDEX].W = matrix.M42;

            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_2_INDEX].X = matrix.M13;
            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_2_INDEX].Y = matrix.M23;
            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_2_INDEX].Z = matrix.M33;
            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_2_INDEX].W = matrix.M43;

            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_3_INDEX].X = matrix.M14;
            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_3_INDEX].Y = matrix.M24;
            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_3_INDEX].Z = matrix.M34;
            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_3_INDEX].W = matrix.M44;

            if (State.DX.Light.Mode == RendererLightingMode::Off)
            {
                State.DX.Shaders.Constants.Staging[SHADER_REGISTER_4_INDEX].X = 0.0f;
                State.DX.Shaders.Constants.Staging[SHADER_REGISTER_4_INDEX].Y = 0.0f;
                State.DX.Shaders.Constants.Staging[SHADER_REGISTER_4_INDEX].Z = 0.0f;
            }
            else
            {
                State.DX.Shaders.Constants.Staging[SHADER_REGISTER_4_INDEX].X = State.DX.Light.Direction.X;
                State.DX.Shaders.Constants.Staging[SHADER_REGISTER_4_INDEX].Y = State.DX.Light.Direction.Y;
                State.DX.Shaders.Constants.Staging[SHADER_REGISTER_4_INDEX].Z = State.DX.Light.Direction.Z;
            }

            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_4_INDEX].W = 0.0f;

            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_5_INDEX].X = 0.0f;
            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_5_INDEX].Y = 1.0f;
            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_5_INDEX].Z = 0.5f;
            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_5_INDEX].W = State.DX.Light.Alpha;

            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_6_INDEX].X = State.DX.Fog.Start;
            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_6_INDEX].Y = 1.0f / (State.DX.Fog.End - State.DX.Fog.Start);

            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_9_INDEX].X = State.DX.Light.Position.X;
            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_9_INDEX].Y = State.DX.Light.Position.Y;
            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_9_INDEX].Z = State.DX.Light.Position.Z;
            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_9_INDEX].W = 0.0f;

            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_10_INDEX].X = State.DX.Fog.Color.R;
            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_10_INDEX].Y = State.DX.Fog.Color.G;
            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_10_INDEX].Z = State.DX.Fog.Color.B;
            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_10_INDEX].W = 0.0f;

            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_12_INDEX].X = State.DX.Transform.O2W.M11;
            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_12_INDEX].Y = State.DX.Transform.O2W.M21;
            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_12_INDEX].Z = State.DX.Transform.O2W.M31;
            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_12_INDEX].W = State.DX.Transform.O2W.M41;

            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_13_INDEX].X = State.DX.Transform.O2W.M12;
            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_13_INDEX].Y = State.DX.Transform.O2W.M22;
            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_13_INDEX].Z = State.DX.Transform.O2W.M32;
            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_13_INDEX].W = State.DX.Transform.O2W.M42;

            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_14_INDEX].X = State.DX.Transform.O2W.M13;
            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_14_INDEX].Y = State.DX.Transform.O2W.M23;
            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_14_INDEX].Z = State.DX.Transform.O2W.M33;
            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_14_INDEX].W = State.DX.Transform.O2W.M43;

            if ((State.DX.Mode.Mode & RendererMode::U0x400) != RendererMode::None)
            {
                State.DX.Shaders.Constants.Staging[SHADER_REGISTER_15_INDEX].X = State.DX.Transform.W2L.M11;
                State.DX.Shaders.Constants.Staging[SHADER_REGISTER_15_INDEX].Y = State.DX.Transform.W2L.M21;
                State.DX.Shaders.Constants.Staging[SHADER_REGISTER_15_INDEX].Z = State.DX.Transform.W2L.M31;
                State.DX.Shaders.Constants.Staging[SHADER_REGISTER_15_INDEX].W = State.DX.Transform.W2L.M41;

                State.DX.Shaders.Constants.Staging[SHADER_REGISTER_16_INDEX].X = State.DX.Transform.W2L.M12;
                State.DX.Shaders.Constants.Staging[SHADER_REGISTER_16_INDEX].Y = State.DX.Transform.W2L.M22;
                State.DX.Shaders.Constants.Staging[SHADER_REGISTER_16_INDEX].Z = State.DX.Transform.W2L.M32;
                State.DX.Shaders.Constants.Staging[SHADER_REGISTER_16_INDEX].W = State.DX.Transform.W2L.M42;

                State.DX.Shaders.Constants.Staging[SHADER_REGISTER_17_INDEX].X = State.DX.Transform.W2L.M13;
                State.DX.Shaders.Constants.Staging[SHADER_REGISTER_17_INDEX].Y = State.DX.Transform.W2L.M23;
                State.DX.Shaders.Constants.Staging[SHADER_REGISTER_17_INDEX].Z = State.DX.Transform.W2L.M33;
                State.DX.Shaders.Constants.Staging[SHADER_REGISTER_17_INDEX].W = State.DX.Transform.W2L.M42;

                State.DX.Shaders.Constants.Staging[SHADER_REGISTER_18_INDEX].X = State.DX.Transform.W2L.M14;
                State.DX.Shaders.Constants.Staging[SHADER_REGISTER_18_INDEX].Y = State.DX.Transform.W2L.M24;
                State.DX.Shaders.Constants.Staging[SHADER_REGISTER_18_INDEX].Z = State.DX.Transform.W2L.M34;
                State.DX.Shaders.Constants.Staging[SHADER_REGISTER_18_INDEX].W = State.DX.Transform.W2L.M44;
            }

            const auto count = (State.DX.Mode.Mode & RendererMode::U0x400) == RendererMode::None
                ? MIN_USED_SHADER_REGISTER_COUNT : MAX_USED_SHADER_REGISTER_COUNT;

            for (u32 x = 0; x < count; x++)
            {
                auto actual = &State.DX.Shaders.Constants.Actual[x];
                const auto staging = &State.DX.Shaders.Constants.Staging[x];

                if (actual->X != staging->X || actual->Y != staging->Y || actual->Z != staging->Z || actual->W != staging->W)
                {
                    DXC(State.DX.DirectX.Device->SetVertexShaderConstant(x, staging, 1), "Unable to set vertex shader constants.");

                    actual->X = staging->X;
                    actual->Y = staging->Y;
                    actual->Z = staging->Z;
                    actual->W = staging->W;
                }
            }
        }
    }

    void SetShaders(const DWORD vs, const DWORD ps)
    {
        DXC(State.DX.DirectX.Device->SetVertexShader(vs), "Unable to set vertex shader.");

        if (State.Settings.FixedPipelineMode == FixedPipelineOptionsMode::Off)
        {
            DXC(State.DX.DirectX.Device->SetPixelShader(ps), "Unable to set pixel shader.");
        }
    }
}