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

        DXC(State.DX.DirectX.Device->CreateVertexBuffer(State.Settings.Capacities.RTLVX * sizeof(RTLVX),
            D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFVF_TEX1 | D3DFVF_SPECULAR | D3DFVF_DIFFUSE | D3DFVF_XYZRHW,
            D3DPOOL::D3DPOOL_DEFAULT, &State.DX.Buffers.RTLVX), "Unable to create RTLVX vertex buffer");

        DXC(State.DX.DirectX.Device->CreateIndexBuffer(State.Settings.Capacities.Index * sizeof(u16) * GRAPHICS_TRIANGLE_VERTEX_COUNT,
            D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFORMAT::D3DFMT_INDEX16, D3DPOOL::D3DPOOL_DEFAULT,
            &State.DX.Buffers.Index), "Unable to create index buffer");

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

        for (u32 x = 0; x < MAX_TEXTURE_COUNT; x++)
        {
            if (State.DX.Textures.Textures[x].Texture != NULL)
            {
                State.DX.Textures.Textures[x].Texture->Release();
                State.DX.Textures.Textures[x].Texture = NULL;
            }
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

                State.DX.DirectX.Device->SetRenderState(D3DRENDERSTATETYPE::D3DRS_TEXTUREFACTOR, 0xff000000);

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

        if (((State.DX.Mode.Mode ^ mode) & (RendererMode::U0x80 | RendererMode::U0x40)) != RendererMode::None)
        {
            if ((mode & RendererMode::U0x80) != RendererMode::None && (mode & RendererMode::U0x40) != RendererMode::None)
            {
                SetRendererState(D3DRENDERSTATETYPE::D3DRS_ZENABLE, D3DZBUFFERTYPE::D3DZB_TRUE);
                SetRendererState(D3DRENDERSTATETYPE::D3DRS_ZWRITEENABLE, TRUE);
                SetRendererState(D3DRENDERSTATETYPE::D3DRS_ZFUNC, D3DCMPFUNC::D3DCMP_LESSEQUAL);
            }
            else if ((mode & RendererMode::U0x80) != RendererMode::None)
            {
                SetRendererState(D3DRENDERSTATETYPE::D3DRS_ZENABLE, D3DZBUFFERTYPE::D3DZB_TRUE);
                SetRendererState(D3DRENDERSTATETYPE::D3DRS_ZWRITEENABLE, TRUE);
                SetRendererState(D3DRENDERSTATETYPE::D3DRS_ZFUNC, D3DCMPFUNC::D3DCMP_ALWAYS);
            }
            else if ((mode & RendererMode::U0x40) != RendererMode::None)
            {
                SetRendererState(D3DRENDERSTATETYPE::D3DRS_ZENABLE, D3DZBUFFERTYPE::D3DZB_TRUE);
                SetRendererState(D3DRENDERSTATETYPE::D3DRS_ZWRITEENABLE, FALSE);
                SetRendererState(D3DRENDERSTATETYPE::D3DRS_ZFUNC, D3DCMPFUNC::D3DCMP_LESSEQUAL);
            }
            else
            {
                SetRendererState(D3DRENDERSTATETYPE::D3DRS_ZENABLE, D3DZBUFFERTYPE::D3DZB_FALSE);
                SetRendererState(D3DRENDERSTATETYPE::D3DRS_ZWRITEENABLE, FALSE);
                SetRendererState(D3DRENDERSTATETYPE::D3DRS_ZFUNC, D3DCMPFUNC::D3DCMP_ALWAYS);
            }
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

    void SetRendererState(const D3DRENDERSTATETYPE state, const DWORD value)
    {
        if (State.DX.DirectX.State.Values[state] != value)
        {
            State.DX.DirectX.State.Values[state] = value;

            State.DX.DirectX.Device->SetRenderState(state, value);
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