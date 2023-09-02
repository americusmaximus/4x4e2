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
#include "Mathematics.Basic.hxx"
#include "Module.hxx"
#include "Quit.hxx"

#include <math.h>

using namespace Mathematics;
using namespace Renderers;
using namespace Renderers::Modules;
using namespace Settings;

namespace Renderer::Module
{
    extern "C" u32 AddDynamicLight(void) { return FALSE; }

    extern "C" u32 AllocateOptimizedMesh(const u32 vertexCount, const RendererMeshState state, const u32 indexCount, const u32 rbvxi)
    {
        if (state != RendererMeshState::Unk1) { return INVALID_OPTIMIZED_MESH_INDEX; }

        u32 indx = MIN_OPTIMIZED_MESH_INDEX;

        for (u32 x = MIN_OPTIMIZED_MESH_INDEX; x < MAX_OPTIMIZED_MESH_COUNT; x++)
        {
            if (State.DX.Meshes[x].Buffers.RVX == NULL) { break; }

            indx = indx + 1;
        }

        State.DX.Meshes[indx].VertexCount = vertexCount;
        State.DX.Meshes[indx].IndexCount = indexCount;

        DX::DXC(State.DX.DirectX.Device->CreateVertexBuffer(vertexCount * sizeof(RVX),
            D3DUSAGE_WRITEONLY, D3DFVF_TEX1 | D3DFVF_NORMAL | D3DFVF_XYZ,
            D3DPOOL::D3DPOOL_DEFAULT, &State.DX.Meshes[indx].Buffers.RVX),
            "Unable to allocate RVX vertex buffer for optimized mesh.");

        if (rbvxi != INVALID_OPTIMIZED_MESH_INDEX
            && State.Settings.FixedPipelineMode == FixedPipelineOptionsMode::Off)
        {
            DX::DXC(State.DX.DirectX.Device->CreateVertexBuffer(vertexCount * sizeof(RBVX),
                D3DUSAGE_WRITEONLY, D3DFVF_TEX0,
                D3DPOOL::D3DPOOL_DEFAULT, &State.DX.Meshes[indx].Buffers.RBVX),
                "Unable to allocate RBVX vertex buffer for optimized mesh.");
        }

        return indx;
    }

    extern "C" u32 BeginScene(void)
    {
        if (State.DX.Scene.IsActive) { return FALSE; }

        if (State.DX.DirectX.Device->BeginScene() != D3D_OK) { return FALSE; }

        State.DX.Scene.IsActive = TRUE;

        return TRUE;
    }

    extern "C" u32 Clear(void)
    {
        if (State.DX.DirectX.Device != NULL)
        {
            DX::DXC(State.DX.DirectX.Device->Clear(0, NULL, D3DCLEAR_TARGET, GRAPHICS_COLOR_BLACK, 0.0f, 0), "Unable to clear.");
        }

        return TRUE;
    }

    extern "C" u32 ClearZBuffer(void)
    {
        if (State.DX.DirectX.Device != NULL)
        {
            DX::DXC(State.DX.DirectX.Device->Clear(0, NULL, D3DCLEAR_ZBUFFER, GRAPHICS_COLOR_BLACK, 1.0f, 0), "Unable to clear depth buffer.");
        }

        return TRUE;
    }

    u32 DotLineListD3DTL(const RTLVX* vertexes, const u32 vertexCount, const RendererMode mode, const D3DPRIMITIVETYPE primitive, const s32 primitiveCount)
    {
        DX::SelectRendererMode(mode);

        {
            RTLVX* buffer;
            DX::DXC(State.DX.Buffers.RTLVX->Lock(0, vertexCount * sizeof(RTLVX),
                (BYTE**)&buffer, D3DLOCK_DISCARD | D3DLOCK_NOSYSLOCK), "Unable to lock RTLVX vertex buffer.");

            for (u32 x = 0; x < vertexCount; x++)
            {
                const auto input = vertexes[x];
                auto vertex = buffer[x];

                vertex.XYZ.X = input.XYZ.X;
                vertex.XYZ.Y = input.XYZ.Y;
                vertex.XYZ.Z = 1.0f - input.RHW;

                vertex.RHW = input.RHW;

                vertex.Color = input.Color;
                vertex.Specular = input.Specular;

                vertex.UV.X = input.UV.X;
                vertex.UV.Y = input.UV.Y;
            }

            DX::DXC(State.DX.Buffers.RTLVX->Unlock(), "Unable to unlock RTLVX vertex buffer.");
        }

        {
            u16* buffer;
            DX::DXC(State.DX.Buffers.Index->Lock(0, vertexCount * GRAPHICS_LINE_VERTEX_COUNT,
                (BYTE**)&buffer, D3DLOCK_DISCARD | D3DLOCK_NOSYSLOCK), "Unable to lock index buffer.");

            for (u32 x = 0; x < vertexCount; x++)
            {
                buffer[x] = x;
            }

            DX::DXC(State.DX.Buffers.Index->Unlock(), "Unable to unlock index buffer.");
        }

        DX::DXC(State.DX.DirectX.Device->SetStreamSource(STREAM_SOURCE_0_INDEX, State.DX.Buffers.RTLVX, sizeof(RTLVX)),
            "Unable to set RTLVX stream source.");

        DX::DXC(State.DX.DirectX.Device->SetIndices(State.DX.Buffers.Index, 0), "Unable to set index source.");

        DX::SetShaders(D3DFVF_TEX1 | D3DFVF_SPECULAR | D3DFVF_DIFFUSE | D3DFVF_XYZRHW,
            State.DX.Shaders.Pixel[(u32)DX::DXPSN::Decal].Handle);

        DX::DXC(State.DX.DirectX.Device->DrawIndexedPrimitive(primitive, 0, vertexCount, 0, primitiveCount),
            "Unable to draw indexed primitive.");

        return TRUE;
    }

    extern "C" u32 DotListD3DTL(const RTLVX * vertexes, const u32 vertexCount, const RendererMode mode)
    {
        return DotLineListD3DTL(vertexes, vertexCount, mode, D3DPRIMITIVETYPE::D3DPT_POINTLIST, vertexCount);
    }

    extern "C" u32 DrawPixels(void) { return FALSE; } // NOTE: Not being called by the application.

    extern "C" u32 DynamicLightHint(void) { return FALSE; } // NOTE: Not being called by the application.

    extern "C" u32 EnableClipping(const RendererClippingMode) { return TRUE; }

    extern "C" u32 EnableCulling(const RendererCullMode mode)
    {
        switch (mode)
        {
        case RendererCullMode::CCW: { State.DX.Mode.Cull = D3DCULL::D3DCULL_CCW; break; }
        case RendererCullMode::CW: { State.DX.Mode.Cull = D3DCULL::D3DCULL_CW; break; }
        default: { State.DX.Mode.Cull = D3DCULL::D3DCULL_NONE; break; }
        }

        return TRUE;
    }

    extern "C" u32 EnableLighting(const RendererLightingMode mode)
    {
        State.DX.Light.Mode = mode;

        return TRUE;
    }

    extern "C" u32 EndScene(void)
    {
        if (!State.DX.Scene.IsActive) { return FALSE; }

        State.DX.Scene.IsActive = FALSE;

        return State.DX.DirectX.Device->EndScene() == D3D_OK;
    }

    extern "C" u32 FreeOptimizedMesh(const u32 indx)
    {
        if (MAX_OPTIMIZED_MESH_COUNT <= indx) { Quit("Unable to release optimized mesh, invalid index %d.", indx); }

        if (indx == INVALID_OPTIMIZED_MESH_INDEX) { return FALSE; }

        if (State.DX.Meshes[indx].Buffers.RVX != NULL)
        {
            State.DX.Meshes[indx].Buffers.RVX->Release();
            State.DX.Meshes[indx].Buffers.RVX = NULL;
        }

        if (State.DX.Meshes[indx].Buffers.RBVX != NULL)
        {
            State.DX.Meshes[indx].Buffers.RBVX->Release();
            State.DX.Meshes[indx].Buffers.RBVX = NULL;
        }

        return TRUE;
    }

    extern "C" u32 LineListD3DTL(const Renderers::RTLVX * vertexes, const u32 vertexCount, const Renderers::RendererMode mode)
    {
        return DotLineListD3DTL(vertexes, vertexCount, mode, D3DPRIMITIVETYPE::D3DPT_LINELIST, vertexCount - 1);
    }

    extern "C" u32 LockFrame(void)
    {
        if (State.DX.Scene.IsActive) { EndScene(); }

        if (!State.DX.Lock.IsActive)
        {
            State.DX.DirectX.Device->GetRenderTarget(&State.DX.Surfaces.Lock);

            D3DLOCKED_RECT rect;

            if (State.DX.Surfaces.Lock->LockRect(&rect, NULL, D3DLOCK_NOSYSLOCK) == D3D_OK)
            {
                u32 offset = 0;

                for (u32 x = 0; x < State.Window.Height; x++)
                {
                    State.Pixels.Pixels[x] = (u32*)((addr)rect.pBits + offset);

                    offset = offset + rect.Pitch;
                }

                State.DX.Lock.IsActive = TRUE;

                return TRUE;
            }

            return FALSE;
        }

        return TRUE;
    }

    extern "C" u32 PolyList(const RVX * vertexes, const u32 vertexCount, const u16 * indexes, const u32 indexCount, const RendererMode mode)
    {
        if (!State.DX.Scene.IsActive) { return FALSE; }

        DX::SelectRendererMode(mode);
        DX::SetRendererState(D3DRENDERSTATETYPE::D3DRS_CULLMODE, State.DX.Mode.Cull);

        if (State.Settings.FixedPipelineMode != FixedPipelineOptionsMode::Off)
        {
            DX::SetRendererState(D3DRENDERSTATETYPE::D3DRS_LIGHTING, (DWORD)State.DX.Light.Mode);
        }

        if (State.Settings.Capacities.RVX < vertexCount) { Quit("Too many vertexes."); }
        if (State.Settings.Capacities.Index < indexCount) { Quit("Too many indexes."); }

        if (State.Settings.Capacities.RVX < State.DX.Data.RVX.Count + vertexCount)
        {
            State.DX.Data.RVX.Count = 0;
        }

        {
            const auto options = State.DX.Data.RVX.Count == 0
                ? D3DLOCK_DISCARD | D3DLOCK_NOSYSLOCK
                : D3DLOCK_NOOVERWRITE | D3DLOCK_DISCARD | D3DLOCK_NOSYSLOCK;

            RVX* buffer;
            DX::DXC(State.DX.Buffers.RVX->Lock(State.DX.Data.RVX.Count * sizeof(RVX),
                vertexCount * sizeof(RVX), (BYTE**)&buffer, options), "Unable to lock RVX vertex buffer.");

            CopyMemory(buffer, vertexes, vertexCount * sizeof(RVX));

            DX::DXC(State.DX.Buffers.RVX->Unlock(), "Unable to unlock RVX vertex buffer.");
        }

        DX::SetIndexes(indexes, indexCount);

        DX::DXC(State.DX.DirectX.Device->SetStreamSource(STREAM_SOURCE_0_INDEX, State.DX.Buffers.RVX, sizeof(RVX)),
            "Unable to set RVX stream source.");

        DX::SetIndexSource(State.DX.Data.RVX.Count, State.DX.Buffers.Index);

        switch (State.DX.Mode.Blend.BlendOperation)
        {
        case RendererBlendOperation::None:
        case RendererBlendOperation::Disable:
        case RendererBlendOperation::SelectArgument1:
        case RendererBlendOperation::Modulate:
        case RendererBlendOperation::Add:
        {
            DX::SetShaders((mode & RendererMode::U0x400) == RendererMode::None
                ? State.DX.Shaders.Vertex[(u32)DX::DXVSN::DirLight].Handle
                : State.DX.Shaders.Vertex[(u32)DX::DXVSN::TexGen].Handle,
                State.DX.Shaders.Pixel[(u32)DX::DXPSN::Decal].Handle);

            break;
        }
        case RendererBlendOperation::SelectArgument2:
        {
            DX::SetShaders(State.DX.Shaders.Vertex[(u32)DX::DXVSN::Cubic].Handle,
                State.DX.Shaders.Pixel[(u32)DX::DXPSN::Decal].Handle);

            break;
        }
        case RendererBlendOperation::Modulate2X:
        {
            DX::SetShaders(State.DX.Shaders.Vertex[(u32)DX::DXVSN::AlphaFog].Handle,
                State.DX.Shaders.Pixel[(u32)DX::DXPSN::Decal].Handle);

            break;
        }
        case RendererBlendOperation::Modulate4X:
        {
            DX::SetShaders((mode & RendererMode::U0x400) == RendererMode::None
                ? State.DX.Shaders.Vertex[(u32)DX::DXVSN::DirLight].Handle
                : State.DX.Shaders.Vertex[(u32)DX::DXVSN::TexGen].Handle,
                State.DX.Shaders.Pixel[(u32)DX::DXPSN::Detail].Handle);

            break;
        }
        case RendererBlendOperation::AddSigned:
        {
            DX::SetShaders(State.DX.Shaders.Vertex[(u32)DX::DXVSN::ProjectedShadow].Handle,
                State.DX.Shaders.Pixel[(u32)DX::DXPSN::DecalTexKill].Handle);

            break;
        }
        }

        DX::SetShaderConstants();

        DX::DXC(State.DX.DirectX.Device->DrawIndexedPrimitive(D3DPRIMITIVETYPE::D3DPT_TRIANGLELIST,
            State.Settings.VertexIndexFormat == Settings::VertexIndexFormatMode::Count ? State.DX.Data.RVX.Count : 0,
            vertexCount, State.DX.Data.Indexes.Count * GRAPHICS_TRIANGLE_VERTEX_COUNT, indexCount),
            "Unable to draw indexed primitive.");

        State.DX.Data.Indexes.Count = State.DX.Data.Indexes.Count + indexCount;
        State.DX.Data.RVX.Count = State.DX.Data.RVX.Count + vertexCount;

        return TRUE;
    }

    extern "C" u32 PolyListD3DL(const RTLVX * vertexes, const u32 vertexCount, const u16 * indexes, const u32 indexCount, const RendererMode mode)
    {
        if (!State.DX.Scene.IsActive) { return FALSE; }

        DX::SelectRendererMode(mode);
        DX::SetRendererState(D3DRENDERSTATETYPE::D3DRS_CULLMODE, State.DX.Mode.Cull);
        DX::SetRendererState(D3DRENDERSTATETYPE::D3DRS_LIGHTING, FALSE);

        if (State.Settings.Capacities.RLVX < vertexCount) { Quit("Too many vertexes."); }
        if (State.Settings.Capacities.Index < indexCount) { Quit("Too many indexes."); }

        if (State.Settings.Capacities.RLVX < State.DX.Data.RLVX.Count + 1 + vertexCount)
        {
            State.DX.Data.RLVX.Count = 0;
        }

        {
            const auto options = State.DX.Data.RLVX.Count == 0
                ? D3DLOCK_DISCARD | D3DLOCK_NOSYSLOCK
                : D3DLOCK_NOOVERWRITE | D3DLOCK_DISCARD | D3DLOCK_NOSYSLOCK;

            RLVX* buffer;
            DX::DXC(State.DX.Buffers.RLVX->Lock(State.DX.Data.RLVX.Count * sizeof(RLVX),
                (vertexCount + 1) * sizeof(RLVX), (BYTE**)&buffer, options), "Unable to lock RLVX vertex buffer.");

            for (u32 x = 0; x < vertexCount; x++)
            {
                const auto input = &vertexes[x];
                auto vertex = &buffer[x + 1];

                vertex->XYZ.X = input->XYZ.X;
                vertex->XYZ.Y = input->XYZ.Y;
                vertex->XYZ.Z = input->XYZ.Z;

                vertex->Color = input->Color;

                vertex->UV.X = input->UV.X;
                vertex->UV.Y = input->UV.Y;
            }

            DX::DXC(State.DX.Buffers.RLVX->Unlock(), "Unable to unlock RLVX vertex buffer.");
        }

        DX::SetIndexes(indexes, indexCount);

        DX::DXC(State.DX.DirectX.Device->SetStreamSource(STREAM_SOURCE_0_INDEX, State.DX.Buffers.RLVX, sizeof(RLVX)),
            "Unable to set RLVX stream source.");

        DX::SetIndexSource(State.DX.Data.RLVX.Count + 1, State.DX.Buffers.Index);

        switch (State.DX.Mode.Blend.BlendOperation)
        {
        case RendererBlendOperation::None:
        case RendererBlendOperation::SelectArgument2:
        case RendererBlendOperation::Modulate:
        case RendererBlendOperation::Modulate2X:
        case RendererBlendOperation::Modulate4X:
        case RendererBlendOperation::Add:
        {
            if ((mode & RendererMode::U0x400) != RendererMode::None)
            {
                DX::SetShaders(State.DX.Shaders.Vertex[(u32)DX::DXVSN::PreLitTexGen].Handle,
                    State.DX.Shaders.Pixel[(u32)DX::DXPSN::DecalTexKill].Handle);
            }
            else
            {
                DX::SetShaders(State.DX.Shaders.Vertex[(u32)DX::DXVSN::PreLit].Handle,
                    State.DX.Shaders.Pixel[(u32)DX::DXPSN::Decal].Handle);
            }

            break;
        }
        case RendererBlendOperation::Disable:
        {
            DX::SetShaders(State.DX.Shaders.Vertex[(u32)DX::DXVSN::PreLit].Handle,
                State.DX.Shaders.Pixel[(u32)DX::DXPSN::Bump].Handle);

            break;
        }
        case RendererBlendOperation::SelectArgument1:
        {
            DX::SetShaders(State.DX.Shaders.Vertex[(u32)DX::DXVSN::PreLit].Handle,
                State.DX.Shaders.Pixel[(u32)DX::DXPSN::BumpSpec].Handle);

            break;
        }
        case RendererBlendOperation::AddSigned:
        {
            DX::SetShaders(State.DX.Shaders.Vertex[(u32)DX::DXVSN::ProjectedShadow].Handle,
                State.DX.Shaders.Pixel[(u32)DX::DXPSN::DecalTexKill].Handle);

            break;
        }
        }

        DX::SetShaderConstants();

        DX::DXC(State.DX.DirectX.Device->DrawIndexedPrimitive(D3DPRIMITIVETYPE::D3DPT_TRIANGLELIST,
            State.Settings.VertexIndexFormat == Settings::VertexIndexFormatMode::Count ? State.DX.Data.RLVX.Count : 0,
            vertexCount, State.DX.Data.Indexes.Count * GRAPHICS_TRIANGLE_VERTEX_COUNT, indexCount),
            "Unable to draw indexed primitive.");

        State.DX.Data.Indexes.Count = State.DX.Data.Indexes.Count + indexCount;
        State.DX.Data.RLVX.Count = State.DX.Data.RLVX.Count + vertexCount + 1;

        return TRUE;
    }

    extern "C" u32 PolyListTL(const RCVX * vertexes, const u32 vertexCount, const u16 * indexes, const u32 indexCount, const RendererMode mode)
    {
        if (!State.DX.Scene.IsActive) { return FALSE; }

        if (State.DX.Mode.Blend.BlendOperation != RendererBlendOperation::None) { return FALSE; }

        DX::SelectRendererMode(mode);

        DX::SetRendererState(D3DRENDERSTATETYPE::D3DRS_CULLMODE, State.DX.Mode.Cull);
        DX::SetRendererState(D3DRENDERSTATETYPE::D3DRS_LIGHTING, FALSE);

        for (u32 x = 0; x < vertexCount; x++)
        {
            const auto input = &vertexes[x];
            auto vertex = &State.DX.Data.RTLVX.Vertexes[x];

            vertex->XYZ.X = input->Unk06;
            vertex->XYZ.Y = input->Unk07;
            vertex->XYZ.Z = 1.0f - input->Unk04;

            vertex->RHW = input->Unk04;

            vertex->Color = DX::Color(input->Unk12, input->Unk11, input->Unk10, input->Unk13);
            vertex->Specular = DX::Alpha(1.0f - input->Unk14);

            if ((mode & RendererMode::U0x400) == RendererMode::None)
            {
                vertex->UV.X = input->Unk08;
                vertex->UV.Y = input->Unk09;
            }
            else
            {
                vertex->RHW = vertexes->Unk15 * vertexes->Unk04;

                vertex->UV.X = (1.0f / vertexes->Unk15) * vertexes->Unk08 * 0.5f + 0.5f;
                vertex->UV.Y = (1.0f / vertexes->Unk15) * vertexes->Unk09 * -0.5f + 0.5f;
            }
        }

        DX::SetShaders(D3DFVF_TEX1 | D3DFVF_SPECULAR | D3DFVF_DIFFUSE | D3DFVF_XYZRHW,
            State.DX.Shaders.Pixel[(u32)DX::DXPSN::Decal].Handle);

        DX::DXC(State.DX.DirectX.Device->DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE::D3DPT_TRIANGLELIST,
            0, vertexCount, indexCount, indexes, D3DFORMAT::D3DFMT_INDEX16,
            State.DX.Data.RTLVX.Vertexes, sizeof(RTLVX)), "Unable to draw indexed primitive.");

        return TRUE;
    }

    extern "C" u32 ResetDynamicLight(void) { return FALSE; }

    extern "C" u32 RenderOptimizedMesh(const u32 * rvxi, RendererMeshState * state, const RVX * vertexes, const u32 vertexCount, const u16 * indexes, const u32 indexCount, const u32 rbvxi, const RendererMode mode)
    {
        const auto indx = *rvxi;

        if (indx == INVALID_OPTIMIZED_MESH_INDEX)
        {
            return PolyList(vertexes, vertexCount, indexes, indexCount, mode);
        }

        if (State.DX.Meshes[indx].Buffers.RVX == NULL)
        {
            DX::DXC(State.DX.DirectX.Device->CreateVertexBuffer(vertexCount * sizeof(RVX),
                D3DUSAGE_WRITEONLY, D3DFVF_TEX1 | D3DFVF_NORMAL | D3DFVF_XYZ,
                D3DPOOL::D3DPOOL_MANAGED, &State.DX.Meshes[indx].Buffers.RVX),
                "Unable to allocate RVX vertex buffer for optimized mesh.");

            if (rbvxi != INVALID_OPTIMIZED_MESH_INDEX
                && State.Settings.FixedPipelineMode == FixedPipelineOptionsMode::Off)
            {
                DX::DXC(State.DX.DirectX.Device->CreateVertexBuffer(vertexCount * sizeof(RBVX),
                    D3DUSAGE_WRITEONLY, D3DFVF_TEX0,
                    D3DPOOL::D3DPOOL_DEFAULT, &State.DX.Meshes[indx].Buffers.RBVX),
                    "Unable to allocate RBVX vertex buffer for optimized mesh.");
            }

            *state = RendererMeshState::None;
        }

        if ((*state & RendererMeshState::Unk1) == RendererMeshState::None)
        {
            RVX* buffer;
            DX::DXC(State.DX.Meshes[indx].Buffers.RVX->Lock(0, vertexCount * sizeof(RVX),
                (BYTE**)&buffer, D3DLOCK_NOSYSLOCK), "Unable to lock optimized RVX mesh buffer.");

            CopyMemory(buffer, vertexes, vertexCount * sizeof(RVX));

            DX::DXC(State.DX.Meshes[indx].Buffers.RVX->Unlock(), "Unable to unlock optimized RVX mesh buffer.");

            *state = *state | RendererMeshState::Unk1;
        }

        if (rbvxi != INVALID_OPTIMIZED_MESH_INDEX
            && (*state & RendererMeshState::Unk4) == RendererMeshState::None
            && State.DX.Meshes[indx].Buffers.RBVX != NULL)
        {
            RBVX* buffer;
            DX::DXC(State.DX.Meshes[indx].Buffers.RBVX->Lock(0, vertexCount * sizeof(RBVX),
                (BYTE**)&buffer, D3DLOCK_NOSYSLOCK), "Unable to lock optimized RBVX mesh buffer.");

            CopyMemory(buffer, vertexes, vertexCount * sizeof(RBVX));

            DX::DXC(State.DX.Meshes[indx].Buffers.RVX->Unlock(), "Unable to unlock optimized RBVX mesh buffer.");

            *state = *state | RendererMeshState::Unk4;
        }

        if (!State.DX.Scene.IsActive) { return TRUE; }

        DX::SelectRendererMode(mode);
        DX::SetRendererState(D3DRENDERSTATETYPE::D3DRS_CULLMODE, State.DX.Mode.Cull);

        if (State.Settings.FixedPipelineMode != FixedPipelineOptionsMode::Off)
        {
            DX::SetRendererState(D3DRENDERSTATETYPE::D3DRS_LIGHTING, (DWORD)State.DX.Light.Mode);
        }

        DX::SetIndexes(indexes, indexCount);

        DX::DXC(State.DX.DirectX.Device->SetStreamSource(STREAM_SOURCE_0_INDEX,
            State.DX.Meshes[indx].Buffers.RVX, sizeof(RVX)), "Unable to set RVX stream source.");

        if (State.DX.Mode.Blend.BlendOperation == RendererBlendOperation::Disable
            || State.DX.Mode.Blend.BlendOperation == RendererBlendOperation::SelectArgument1
            || State.DX.Mode.Blend.BlendOperation == RendererBlendOperation::Modulate)
        {
            if (State.DX.Meshes[indx].Buffers.RBVX == NULL)
            {
                State.DX.Mode.Blend.BlendOperation = RendererBlendOperation::None;
            }
            else
            {
                DX::DXC(State.DX.DirectX.Device->SetStreamSource(STREAM_SOURCE_1_INDEX,
                    State.DX.Meshes[indx].Buffers.RBVX, sizeof(RBVX)), "Unable to set RBVX stream source.");
            }
        }

        DX::SetIndexSource(0, State.DX.Buffers.Index);

        switch (State.DX.Mode.Blend.BlendOperation)
        {
        case RendererBlendOperation::None:
        {
            if ((mode & RendererMode::U0x400) != RendererMode::None)
            {
                DX::SetShaders(State.DX.Shaders.Vertex[(u32)DX::DXVSN::TexGen].Handle,
                    State.DX.Shaders.Pixel[(u32)DX::DXPSN::DecalTexKill].Handle);
            }
            else
            {
                DX::SetShaders(State.DX.Shaders.Vertex[(u32)DX::DXVSN::DirLight].Handle,
                    State.DX.Shaders.Pixel[(u32)DX::DXPSN::Decal].Handle);
            }

            break;
        }
        case RendererBlendOperation::Disable:
        {
            DX::SetShaders((mode & RendererMode::U0x400) == RendererMode::None
                ? State.DX.Shaders.Vertex[(u32)DX::DXVSN::Bump].Handle
                : State.DX.Shaders.Vertex[(u32)DX::DXVSN::BumpDiffuseTexGen].Handle,
                State.DX.Shaders.Pixel[(u32)DX::DXPSN::Bump].Handle);

            break;
        }
        case RendererBlendOperation::SelectArgument1:
        {
            if ((mode & RendererMode::U0x400) != RendererMode::None)
            {
                DX::SetShaders(State.DX.Shaders.Vertex[(u32)DX::DXVSN::BumpSpecTexGen].Handle,
                    State.DX.Shaders.Pixel[(u32)DX::DXPSN::BumpSpecTexGen].Handle);
            }
            else
            {
                DX::SetShaders(State.DX.Shaders.Vertex[(u32)DX::DXVSN::BumpSpec].Handle,
                    State.DX.Shaders.Pixel[(u32)DX::DXPSN::BumpSpec].Handle);
            }

            break;
        }
        case RendererBlendOperation::SelectArgument2:
        {
            DX::SetShaders(State.DX.Shaders.Vertex[(u32)DX::DXVSN::Cubic].Handle,
                State.DX.Shaders.Pixel[(u32)DX::DXPSN::Decal].Handle);

            break;
        }
        case RendererBlendOperation::Modulate:
        {
            DX::SetShaders(State.DX.Shaders.Vertex[(u32)DX::DXVSN::BumpCubic].Handle,
                State.DX.Shaders.Pixel[(u32)DX::DXPSN::BumpCubic].Handle);

            break;
        }
        case RendererBlendOperation::Modulate2X:
        {
            DX::SetShaders(State.DX.Shaders.Vertex[(u32)DX::DXVSN::AlphaFog].Handle,
                State.DX.Shaders.Pixel[(u32)DX::DXPSN::Decal].Handle);

            break;
        }
        case RendererBlendOperation::Modulate4X:
        {
            DX::SetShaders(State.DX.Shaders.Vertex[(u32)DX::DXVSN::DirLight].Handle,
                State.DX.Shaders.Pixel[(u32)DX::DXPSN::Detail].Handle);

            break;
        }
        case RendererBlendOperation::Add:
        {
            DX::SetShaders(State.DX.Shaders.Vertex[(u32)DX::DXVSN::Gloss].Handle,
                State.DX.Shaders.Pixel[(u32)DX::DXPSN::Gloss].Handle);

            break;
        }
        case RendererBlendOperation::AddSigned:
        {
            DX::SetShaders(State.DX.Shaders.Vertex[(u32)DX::DXVSN::ProjectedShadow].Handle,
                State.DX.Shaders.Pixel[(u32)DX::DXPSN::DecalTexKill].Handle);

            break;
        }
        }

        DX::SetShaderConstants();

        DX::DXC(State.DX.DirectX.Device->DrawIndexedPrimitive(D3DPRIMITIVETYPE::D3DPT_TRIANGLELIST,
            0, vertexCount, State.DX.Data.Indexes.Count * GRAPHICS_TRIANGLE_VERTEX_COUNT, indexCount),
            "Unable to draw indexed primitive.");

        State.DX.Data.Indexes.Count = State.DX.Data.Indexes.Count + indexCount;

        return TRUE;
    }

    extern "C" u32 SetAlpha(const u32 value)
    {
        DX::SetRendererState(D3DRENDERSTATETYPE::D3DRS_ALPHAREF, value);

        return TRUE;
    }

    extern "C" u32 SetAmbientLight(const f32 color)
    {
        State.DX.Light.Light.Ambient.r = color;
        State.DX.Light.Light.Ambient.g = color;
        State.DX.Light.Light.Ambient.b = color;
        State.DX.Light.Light.Ambient.a = color;

        return DX::SelectLight();
    }

    extern "C" u32 SetColorTable(u8 * rgb, u16 * rgba)
    {
        if (State.Window.BitsPerPixel == GRAPHICS_BITS_PER_PIXEL_16)
        {
            *State.InitializeArguments.R.Display = 11;
            *State.InitializeArguments.R.Font = 8;
            *State.InitializeArguments.R.BitsPerPixel = 3;

            *State.InitializeArguments.G.Display = 5;
            *State.InitializeArguments.G.Font = 4;
            *State.InitializeArguments.G.BitsPerPixel = 2;

            *State.InitializeArguments.B.Display = 0;
            *State.InitializeArguments.B.Font = 8;
            *State.InitializeArguments.B.BitsPerPixel = 3;
        }
        else
        {
            *State.InitializeArguments.R.Display = 16;
            *State.InitializeArguments.R.Font = 1;
            *State.InitializeArguments.R.BitsPerPixel = 0;

            *State.InitializeArguments.G.Display = 8;
            *State.InitializeArguments.G.Font = 1;
            *State.InitializeArguments.G.BitsPerPixel = 0;

            *State.InitializeArguments.B.Display = 0;
            *State.InitializeArguments.B.Font = 1;
            *State.InitializeArguments.B.BitsPerPixel = 0;
        }

        {
            const auto rf = *State.InitializeArguments.R.Font;
            const auto rd = *State.InitializeArguments.R.Display;

            const auto gf = *State.InitializeArguments.G.Font;
            const auto gd = *State.InitializeArguments.G.Display;

            const auto bf = *State.InitializeArguments.B.Font;
            const auto bd = *State.InitializeArguments.B.Display;

            for (u32 x = 0; x < MAX_COLOR_TABLE_BUFFER_SIZE; x++)
            {
                auto r = rgb[x * 3 + 0] / rf << (rd & 0x1f);
                auto g = rgb[x * 3 + 1] / gf << (gd & 0x1f);
                auto b = rgb[x * 3 + 2] / bf << (bd & 0x1f);

                rgba[x] = (u16)(r | g | b);
            }
        }

        return TRUE;
    }

    extern "C" u32 SetDestinationBlend(const RendererDestinationBlend mode)
    {
        switch (mode)
        {
        case RendererDestinationBlend::InverseAlpha: { State.DX.Mode.Blend.DestinationBlendMode = D3DBLEND::D3DBLEND_INVSRCALPHA; break; }
        case RendererDestinationBlend::One: { State.DX.Mode.Blend.DestinationBlendMode = D3DBLEND::D3DBLEND_ONE; break; }
        case RendererDestinationBlend::Color: { State.DX.Mode.Blend.DestinationBlendMode = D3DBLEND::D3DBLEND_SRCCOLOR; break; }
        case RendererDestinationBlend::InverseColor: { State.DX.Mode.Blend.DestinationBlendMode = D3DBLEND::D3DBLEND_INVSRCCOLOR; break; }
        case RendererDestinationBlend::Zero: { State.DX.Mode.Blend.DestinationBlendMode = D3DBLEND::D3DBLEND_ZERO; break; }
        }

        return TRUE;
    }

    extern "C" u32 SetFog(const f32 r, const f32 g, const f32 b, const f32 start, const f32 end)
    {
        const u32 rv = Clamp<u32>((u32)Max<f32>(0.0f, (r * 255.0f)), 0, 255);
        const u32 gv = Clamp<u32>((u32)Max<f32>(0.0f, (g * 255.0f)), 0, 255);
        const u32 bv = Clamp<u32>((u32)Max<f32>(0.0f, (b * 255.0f)), 0, 255);

        const u32 color = (rv << 16) | (gv << 8) | bv;

        DX::SetRendererState(D3DRENDERSTATETYPE::D3DRS_FOGCOLOR, color);

        // Depth at which pixel or vertex fog effects begin for linear fog mode.The default value is 0.0f.
        // Depth is specified in world space for vertex fog and either device space[0.0, 1.0] or world space for pixel fog.
        // For pixel fog, these values are in device space when the system uses z for fog calculations and world - world space when the system is using eye - relative fog(w - fog).
        DX::SetRendererState(D3DRENDERSTATETYPE::D3DRS_FOGSTART, *((DWORD*)(&start)));

        // Depth at which pixel or vertex fog effects end for linear fog mode. The default value is 1.0f.
        // Depth is specified in world space for vertex fog and either device space [0.0, 1.0] or world space for pixel fog.
        // For pixel fog, these values are in device space when the system uses z for fog calculations and in world space when the system is using eye-relative fog (w-fog).
        DX::SetRendererState(D3DRENDERSTATETYPE::D3DRS_FOGEND, *((DWORD*)(&end)));

        // Fog density for pixel or vertex fog used in the exponential fog modes (D3DFOG_EXP and D3DFOG_EXP2). Valid density values range from 0.0 through 1.0.
        DX::SetRendererState(D3DRENDERSTATETYPE::D3DRS_FOGDENSITY, 0);

        State.DX.Mode.IsChanged = TRUE;

        State.DX.Fog.Start = start;
        State.DX.Fog.End = end;

        State.DX.Fog.Color.R = r;
        State.DX.Fog.Color.G = g;
        State.DX.Fog.Color.B = b;

        return TRUE;
    }

    extern "C" u32 SetGloss(const f32 value)
    {
        State.DX.Gloss = value;

        return TRUE;
    }
    
    extern "C" u32 SetLightColor(const f32 r, const f32 g, const f32 b)
    {
        State.DX.Light.Color.R = r;
        State.DX.Light.Color.G = g;
        State.DX.Light.Color.B = b;

        return TRUE;
    }
    
    extern "C" u32 SetLightConstants(const f32 diffuse, const f32 specular, const f32 power, const f32 alpha)
    {
        if (State.Settings.FixedPipelineMode == FixedPipelineOptionsMode::On)
        {
            const D3DMATERIAL8 material =
            {
                .Diffuse =
                {
                    .r = diffuse,
                    .g = diffuse,
                    .b = diffuse,
                    .a = alpha
                },
                .Ambient =
                {
                    .r = 1.0f,
                    .g = 1.0f,
                    .b = 1.0f,
                    .a = alpha
                },
                .Specular =
                {
                    .r = specular,
                    .g = specular,
                    .b = specular,
                    .a = alpha
                },
                .Emissive =
                {
                    .r = 0.0f,
                    .g = 0.0f,
                    .b = 0.0f,
                    .a = 0.0f
                },
                .Power = power
            };

            DX::DXC(State.DX.DirectX.Device->SetMaterial(&material), "Unable to set material.");
        }

        if (State.DX.Light.Alpha != alpha)
        {
            State.DX.Mode.IsChanged = TRUE;
        }

        State.DX.Light.Alpha = alpha;

        return TRUE;
    }

    extern "C" u32 SetLightVector(const f32x3*, const f32 value)
    {
        State.DX.Light.Light.Diffuse.a = 1.0f;
        State.DX.Light.Light.Diffuse.r = State.DX.Light.Color.R * value;
        State.DX.Light.Light.Diffuse.g = State.DX.Light.Color.G * value;
        State.DX.Light.Light.Diffuse.b = State.DX.Light.Color.B * value;

        return TRUE;
    }

    extern "C" u32 SetMultiTextureBlend(const RendererBlendOperation operation)
    {
        State.DX.Mode.Blend.BlendOperation = operation;

        if (operation == RendererBlendOperation::None || operation == RendererBlendOperation::SelectArgument2) { return TRUE; }

        return State.DX.Devices.Current.Capabilities.TextureOpCaps & D3DTEXOPCAPS_DOTPRODUCT3;
    }

    extern "C" u32 SetSourceBlend(const RendererSourceBlendMode mode)
    {
        switch (mode)
        {
        case RendererSourceBlendMode::Alpha: { State.DX.Mode.Blend.SourceBlendMode = D3DBLEND::D3DBLEND_SRCALPHA; break; }
        case RendererSourceBlendMode::One: { State.DX.Mode.Blend.SourceBlendMode = D3DBLEND::D3DBLEND_ONE; break; }
        case RendererSourceBlendMode::Zero: { State.DX.Mode.Blend.SourceBlendMode = D3DBLEND::D3DBLEND_ZERO; break; }
        }

        return TRUE;
    }

    extern "C" u32 SetStencilEnable(const RendererStencilMode mode)
    {
        State.DX.Mode.StencilMode = mode;

        DX::SetRendererState(D3DRENDERSTATETYPE::D3DRS_ZENABLE, TRUE);
        DX::SetRendererState(D3DRENDERSTATETYPE::D3DRS_STENCILENABLE, mode == RendererStencilMode::On);

        if (mode == RendererStencilMode::On)
        {
            DX::SetRendererState(D3DRENDERSTATETYPE::D3DRS_ZWRITEENABLE, FALSE);
        }
        else
        {
            DX::SetRendererState(D3DRENDERSTATETYPE::D3DRS_ZWRITEENABLE, FALSE);
            DX::SetRendererState(D3DRENDERSTATETYPE::D3DRS_ZFUNC, D3DCMPFUNC::D3DCMP_LESSEQUAL);
        }

        return TRUE;
    }

    extern "C" u32 SetStencilFunc(const RendererStencilFunction func)
    {
        DX::SetRendererState(D3DRENDERSTATETYPE::D3DRS_STENCILFUNC,
            func == RendererStencilFunction::LessEqual ? D3DCMPFUNC::D3DCMP_LESSEQUAL : D3DCMPFUNC::D3DCMP_ALWAYS);

        return TRUE;
    }

    extern "C" u32 SetStencilPass(const RendererStencilPass pass)
    {
        switch (pass)
        {
        case RendererStencilPass::LessEqualDecrement:
        {
            DX::SetRendererState(D3DRENDERSTATETYPE::D3DRS_STENCILPASS, D3DSTENCILOP::D3DSTENCILOP_DECR);
            DX::SetRendererState(D3DRENDERSTATETYPE::D3DRS_ZFUNC, D3DCMPFUNC::D3DCMP_LESSEQUAL);

            break;
        }
        case RendererStencilPass::LessEqualIncrement:
        {
            DX::SetRendererState(D3DRENDERSTATETYPE::D3DRS_STENCILPASS, D3DSTENCILOP::D3DSTENCILOP_INCR);
            DX::SetRendererState(D3DRENDERSTATETYPE::D3DRS_ZFUNC, D3DCMPFUNC::D3DCMP_LESSEQUAL);

            break;
        }
        case RendererStencilPass::GreaterEqualIncrement:
        {
            DX::SetRendererState(D3DRENDERSTATETYPE::D3DRS_STENCILPASS, D3DSTENCILOP::D3DSTENCILOP_INCR);
            DX::SetRendererState(D3DRENDERSTATETYPE::D3DRS_ZFUNC, D3DCMPFUNC::D3DCMP_GREATEREQUAL);

            break;
        }
        case RendererStencilPass::GreaterEqualDecrement:
        {
            DX::SetRendererState(D3DRENDERSTATETYPE::D3DRS_STENCILPASS, D3DSTENCILOP::D3DSTENCILOP_DECR);
            DX::SetRendererState(D3DRENDERSTATETYPE::D3DRS_ZFUNC, D3DCMPFUNC::D3DCMP_GREATEREQUAL);

            break;
        }
        }

        return TRUE;
    }

    extern "C" u32 SetTextureClamp(const RendererClampMode u, const RendererClampMode v, const u32 indx)
    {
        if (State.DX.Mode.Clamp.U[indx] != u || State.DX.Mode.Clamp.V[indx] != v)
        {
            State.DX.Mode.Clamp.U[indx] = u;
            State.DX.Mode.Clamp.V[indx] = v;

            State.DX.Mode.Clamp.ClampOperation = RendererBlendOperation::All;
        }

        return TRUE;
    }

    extern "C" u32 SetTransform(const Matrix4x4 * transform, const f32 zNear, const f32 zFar, const f32 todo4, const f32x3 * direction, const f32x3 * position)
    {
        if (State.Settings.FixedPipelineMode == FixedPipelineOptionsMode::On)
        {
            State.DX.DirectX.Device->SetTransform(D3DTS_WORLD, (D3DMATRIX*)transform);

            State.DX.DirectX.Device->SetTransform(D3DTRANSFORMSTATETYPE::D3DTS_VIEW,
                (D3DMATRIX*)&State.DX.Transform.Identity);
        }

        if (State.DX.Transform.Near != zNear || State.DX.Transform.Far != zFar)
        {
            State.DX.Transform.IsChanged = TRUE;

            State.DX.Transform.Near = zNear;
            State.DX.Transform.Far = zFar;
        }

        if (State.DX.Transform.Unknown4 == todo4) // TODO
        {
            if (State.DX.Transform.IsChanged)
            {
                State.DX.Transform.IsChanged = FALSE;

                const auto clippingNear = State.DX.Mode.Clipping.Near;
                const auto clippingFar = State.DX.Mode.Clipping.Far;

                if (todo4 == 0.0f) // TODO, name, type
                {
                    State.DX.Transform.Projection.M21 = todo4;
                    State.DX.Transform.Projection.M34 = 1.0f;
                    State.DX.Transform.Projection.M44 = 0.0f;
                }
                else
                {
                    State.DX.Transform.Projection.M21 = 0.0f;
                    State.DX.Transform.Projection.M34 = 0.0f;
                    State.DX.Transform.Projection.M44 = 1.0f;
                }

                State.DX.Transform.Projection.M11 = zNear;
                State.DX.Transform.Projection.M12 = State.DX.Transform.Projection.M21;
                State.DX.Transform.Projection.M13 = State.DX.Transform.Projection.M21;
                State.DX.Transform.Projection.M14 = State.DX.Transform.Projection.M21;

                State.DX.Transform.Projection.M22 = zFar;
                State.DX.Transform.Projection.M23 = 0.0f;
                State.DX.Transform.Projection.M24 = 0.0f;

                State.DX.Transform.Projection.M31 = 0.0f;
                State.DX.Transform.Projection.M32 = 0.0f;
                State.DX.Transform.Projection.M33 = clippingFar / (clippingFar - clippingNear);

                State.DX.Transform.Projection.M41 = 0.0f;
                State.DX.Transform.Projection.M42 = 0.0f;
                State.DX.Transform.Projection.M43 = -(clippingNear * clippingFar / (clippingFar - clippingNear));

                State.DX.DirectX.Device->SetTransform(D3DTRANSFORMSTATETYPE::D3DTS_PROJECTION, (D3DMATRIX*)&State.DX.Transform.Projection);
            }
        }
        else
        {
            State.DX.Transform.Unknown4 = todo4; // TODO
            State.DX.Transform.IsChanged = FALSE;
        }

        CopyMemory(&State.DX.Transform.ModelView, transform, sizeof(Matrix4x4));

        {
            State.DX.Light.Direction.X = -direction->X;
            State.DX.Light.Direction.Y = -direction->Y;
            State.DX.Light.Direction.Z = -direction->Z;

            const auto length = sqrtf(State.DX.Light.Direction.X * State.DX.Light.Direction.X
                + State.DX.Light.Direction.Y * State.DX.Light.Direction.Y
                + State.DX.Light.Direction.Z * State.DX.Light.Direction.Z);

            if (0.0f < length)
            {
                State.DX.Light.Direction.X = State.DX.Light.Direction.X / length;
                State.DX.Light.Direction.Y = State.DX.Light.Direction.Y / length;
                State.DX.Light.Direction.Z = State.DX.Light.Direction.Z / length;
            }
        }

        State.DX.Mode.IsChanged = TRUE;

        {
            State.DX.Light.Position.X = -position->X;
            State.DX.Light.Position.Y = -position->Y;
            State.DX.Light.Position.Z = -position->Z;

            if (State.Settings.FixedPipelineMode == FixedPipelineOptionsMode::On)
            {
                State.DX.Light.Light.Direction.x =
                    -State.DX.Light.Direction.X * transform->M11
                    - State.DX.Light.Direction.Y * transform->M21
                    - State.DX.Light.Direction.Z * transform->M31;

                State.DX.Light.Light.Direction.y =
                    -State.DX.Light.Direction.X * transform->M12
                    - State.DX.Light.Direction.Y * transform->M22
                    - State.DX.Light.Direction.Z * transform->M32;

                State.DX.Light.Light.Direction.z =
                    -State.DX.Light.Direction.X * transform->M13
                    - State.DX.Light.Direction.Y * transform->M23
                    - State.DX.Light.Direction.Z * transform->M33;

                DX::SelectLight();
            }
        }

        return TRUE;
    }

    extern "C" u32 SetTransformO2W(const Matrix4x4 * transform)
    {
        CopyMemory(&State.DX.Transform.O2W, transform, sizeof(Matrix4x4));

        State.DX.Mode.IsChanged = TRUE;

        return TRUE;
    }

    extern "C" u32 SetTransformW2L(const Matrix4x4 * transform, const f32 zNear, const f32 zFar)
    {
        const Matrix4x4 matrix =
        {
            0.5f, 0.0f,  0.0f,                           0.0f,
            0.0f, -0.5f, 0.0f,                           0.0f,
            0.5f, 0.5f,  zFar / (zFar - zNear),          1.0f,
            0.0f, 0.0f,  -zNear * zFar / (zFar - zNear), 1.0f
        };

        Matrix4x4 result;

        MultiplyMatrix(&result, transform, &matrix);

        if (State.Settings.FixedPipelineMode == FixedPipelineOptionsMode::On)
        {
            Matrix4x4 inverted;

            InvertMatrix(&inverted, &State.DX.Transform.ModelView);

            Matrix4x4 mt;

            MultiplyMatrix(&mt, &inverted, &result);

            State.DX.DirectX.Device->SetTransform(D3DTRANSFORMSTATETYPE::D3DTS_TEXTURE0, (D3DMATRIX*)&mt);

            return TRUE;
        }

        CopyMemory(&State.DX.Transform.W2L, &result, sizeof(Matrix4x4));

        State.DX.Mode.IsChanged = TRUE;

        return TRUE;
    }

    extern "C" u32 SetViewPort(const f32 x0, const f32 y0, const f32 x1, const f32 y1, const f32 zNear, const f32 zFar)
    {
        const D3DVIEWPORT8 viewport =
        {
            .X = (u32)(x0),
            .Y = (u32)(y0),
            .Width = (u32)(x1 - x0),
            .Height = (u32)(y1 - y0),
            .MinZ = 0.0f,
            .MaxZ = 1.0f
        };

        if (viewport.Width < 0 || viewport.Height < 0) { return FALSE; }

        State.DX.DirectX.Device->SetViewport(&viewport);

        State.DX.Mode.Clipping.Near = zNear;
        State.DX.Mode.Clipping.Far = zFar;

        State.DX.Transform.IsChanged = TRUE;

        return TRUE;
    }

    extern "C" u32 Toggle(void)
    {
        if (State.DX.DirectX.Device != NULL)
        {
            DX::DXC(State.DX.DirectX.Device->Present(NULL, NULL, NULL, NULL), "Unable to present back buffer.");
        }

        return TRUE;
    }

    extern "C" u32 UnlockFrame(void)
    {
        if (!State.DX.Lock.IsActive) { return FALSE; }

        State.DX.Lock.IsActive = FALSE;

        for (u32 x = 0; x < State.Window.Height; x++)
        {
            State.Pixels.Pixels[x] = State.Pixels.Values[x];
        }

        DX::DXC(State.DX.Surfaces.Lock->UnlockRect(), "Unable to unlock back buffer.");

        State.DX.Surfaces.Lock->Release();

        return TRUE;
    }
}