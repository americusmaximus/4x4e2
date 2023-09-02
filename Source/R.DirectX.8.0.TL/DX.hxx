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

#pragma once

#include "Basic.hxx"
#include "Native.Basic.hxx"
#include "Renderers.Basic.hxx"
#include "Renderers.Basic.Textures.hxx"
#include "Renderers.Basic.Vertexes.hxx"

#define DIRECT3D_VERSION 0x0800
#include <d3d8.h>

#define D3DUSAGE_NONE 0

#define MAX_STATE_VARIABLE_INDEX_MASK (0xff)
#define MAX_STATE_VARIABLE_COUNT 256

#define MAX_VERTEX_SHADER_COUNT 13
#define MAX_PIXEL_SHADER_COUNT 8

#define MAX_TEXTURE_CACHE_ITEM_NAME_LENGTH 64

#define MAX_PIXEL_PALETTE_BUFFER_SIZE 256
#define MAX_PIXEL_BUFFER_SIZE (1024 * 1024 + 512 * 512 + 256 * 256 + 128 * 128 + 64 * 64 + 32 * 32 + 16 * 16 + 8 * 8 + 4 * 4 + 2 * 2 + 1 * 1)

#define MAX_SURFACE_STAGE_COUNT 8
#define MAX_SURFACE_STAGE_VALUES_COUNT 255

#define MAX_TEXTURE_COUNT 4096

#define MAX_RENDER_TEXTURE_COUNT 100
#define MAX_MIP_MAP_SURFACE_COUNT 12
#define MAX_BLEND_TEXTURE_COUNT 4

#define TEXTURE_STAGE_0_INDEX 0
#define TEXTURE_STAGE_1_INDEX 1
#define TEXTURE_STAGE_2_INDEX 2
#define TEXTURE_STAGE_3_INDEX 3

#define STREAM_SOURCE_0_INDEX 0
#define STREAM_SOURCE_1_INDEX 1

#define MIN_USED_SHADER_REGISTER_COUNT 15
#define MAX_USED_SHADER_REGISTER_COUNT 19
#define MAX_SHADER_REGISTER_COUNT 96

#define SHADER_REGISTER_0_INDEX 0
#define SHADER_REGISTER_1_INDEX 1
#define SHADER_REGISTER_2_INDEX 2
#define SHADER_REGISTER_3_INDEX 3
#define SHADER_REGISTER_4_INDEX 4
#define SHADER_REGISTER_5_INDEX 5
#define SHADER_REGISTER_6_INDEX 6
#define SHADER_REGISTER_7_INDEX 7
#define SHADER_REGISTER_8_INDEX 8
#define SHADER_REGISTER_9_INDEX 9
#define SHADER_REGISTER_10_INDEX 10
#define SHADER_REGISTER_11_INDEX 11
#define SHADER_REGISTER_12_INDEX 12
#define SHADER_REGISTER_13_INDEX 13
#define SHADER_REGISTER_14_INDEX 14
#define SHADER_REGISTER_15_INDEX 15
#define SHADER_REGISTER_16_INDEX 16
#define SHADER_REGISTER_17_INDEX 17
#define SHADER_REGISTER_18_INDEX 18

#define MAX_PIXEL_SHADER_REGISTER_COUNT 2

namespace DX::Shaders::Pixel
{
    extern const DWORD DecalFunction[];
    extern const DWORD DecalTexKillFunction[];
    extern const DWORD BumpFunction[];
    extern const DWORD BumpSpecFunction[];
    extern const DWORD BumpCubicFunction[];
    extern const DWORD BumpSpecTexGenFunction[];
    extern const DWORD DetailFunction[];
    extern const DWORD GlossFunction[];
}

namespace DX::Shaders::Vertex
{
    extern const DWORD DECLARATION_XYZ_NORMAL_UV[];
    extern const DWORD DECLARATION_XYZ_DIFFUSE_UV[];
    extern const DWORD DECLARATION_XYZ_NORMAL_UV_BLENDWEIGHT_BLENDINDICES[];

    extern const DWORD DirLightFunction[];
    extern const DWORD TexGenFunction[];
    extern const DWORD CubicFunction[];
    extern const DWORD PreLitFunction[];
    extern const DWORD PreLitTexGenFunction[];
    extern const DWORD BumpFunction[];
    extern const DWORD BumpSpecFunction[];
    extern const DWORD BumpCubicFunction[];
    extern const DWORD BumpDiffuseTexGenFunction[];
    extern const DWORD BumpSpecTexGenFunction[];
    extern const DWORD AlphaFogFunction[];
    extern const DWORD GlossFunction[];
    extern const DWORD ProjectedShadowFunction[];
}

namespace DX
{
    enum class DXPSN : u32
    {
        Decal = 0,
        DecalTexKill = 1,
        Bump = 2,
        BumpSpec = 3,
        BumpCubic = 4,
        BumpSpecTexGen = 5,
        Detail = 6,
        Gloss = 7
    };

    enum class DXVSN : u32
    {
        DirLight = 0,
        TexGen = 1,
        Cubic = 2,
        PreLit = 3,
        PreLitTexGen = 4,
        Bump = 5,
        BumpSpec = 6,
        BumpCubic = 7,
        BumpDiffuseTexGen = 8,
        BumpSpecTexGen = 9,
        AlphaFog = 10,
        Gloss = 11,
        ProjectedShadow = 12
    };

    struct DXDevice
    {
        GUID ID;

        u32 DeviceID;
        u32 VendorID;

        char Driver[MAX_DEVICE_IDENTIFIER_STRING];
        char Description[MAX_DEVICE_IDENTIFIER_STRING];
    };

    struct DXOptimizedMesh
    {
        u32 VertexCount;
        u32 IndexCount;

        struct
        {
            IDirect3DVertexBuffer8* RVX;
            IDirect3DVertexBuffer8* RBVX;
        } Buffers;
    };

    struct DXPS
    {
        const DWORD* Function;
        DWORD Handle;
    };

    struct DXVS
    {
        const DWORD* Declaration;
        const DWORD* Function;
        union
        {
            DWORD Handle;
            DWORD FVF;
        };
    };

    struct DXTexture
    {
        IDirect3DTexture8* Texture;
        BOOL IsOpaque;
        u32 Dimensions;
        u32 ActualDimensions;
        D3DFORMAT Format;
        char Name[MAX_TEXTURE_CACHE_ITEM_NAME_LENGTH];
    };

    struct DXState
    {
        struct
        {
            IDirect3D8* Instance;
            IDirect3DDevice8* Device;

            struct
            {
                DWORD Values[MAX_STATE_VARIABLE_COUNT];
            } State;
        } DirectX;

        struct
        {
            struct
            {
                u32 Count;
                DXDevice Details[MAX_DEVICE_COUNT];
            } Available;

            struct
            {
                u32 Index;

                D3DCAPS8 Capabilities;
                D3DPRESENT_PARAMETERS PresentParams;
            } Current;
        } Devices;

        struct
        {
            BOOL IsChanged = TRUE;
            Renderers::RendererMode Mode;

            Renderers::RendererStencilMode StencilMode;
            D3DZBUFFERTYPE DepthBufferMode = D3DZBUFFERTYPE::D3DZB_TRUE;
            D3DCULL Cull = D3DCULL::D3DCULL_CCW;

            struct
            {
                Renderers::RendererBlendOperation BlendOperation;
                D3DBLEND SourceBlendMode = D3DBLEND::D3DBLEND_SRCALPHA;
                D3DBLEND DestinationBlendMode = D3DBLEND::D3DBLEND_INVSRCALPHA;
            } Blend;

            struct
            {
                f32 Near = DEFAULT_RENDERER_CLIPPING_NEAR;
                f32 Far = DEFAULT_RENDERER_CLIPPING_FAR;
            } Clipping;

            struct
            {
                Renderers::RendererBlendOperation ClampOperation = Renderers::RendererBlendOperation::All;
                Renderers::RendererClampMode U[MAX_SURFACE_STAGE_COUNT];
                Renderers::RendererClampMode V[MAX_SURFACE_STAGE_COUNT];
            } Clamp;
        } Mode;

        struct
        {
            IDirect3DIndexBuffer8* Index;

            IDirect3DVertexBuffer8* RVX;
            IDirect3DVertexBuffer8* RLVX;
            IDirect3DVertexBuffer8* RTLVX;
        } Buffers;

        struct
        {
            Graphics::f32xRGB Color;

            f32 Start, End;
        } Fog;

        f32 Gloss = 1.0f;

        struct
        {
            D3DLIGHT8 Light;

            f32 Alpha;

            f32x3 Direction;
            f32x3 Position;

            Graphics::f32xRGB Color = { 1.0f, 1.0f, 1.0f };
            Renderers::RendererLightingMode Mode = Renderers::RendererLightingMode::On;
        } Light;

        struct
        {
            BOOL IsActive;
        } Lock;

        DXOptimizedMesh Meshes[MAX_OPTIMIZED_MESH_COUNT];

        struct
        {
            BOOL IsActive;
        } Scene;

        struct
        {
            u32 Index;
            u32 Dimensions;

            struct
            {
                D3DFORMAT Opaque;
                D3DFORMAT Transparent;
            } Formats;

            IDirect3DSurface8* Opaque[MAX_MIP_MAP_SURFACE_COUNT];
            IDirect3DSurface8* Transparent[MAX_MIP_MAP_SURFACE_COUNT];

            IDirect3DSurface8* TextureDepthSurface;

            struct
            {
                IDirect3DSurface8* Surface;
                IDirect3DSurface8* DepthSurface;
            } Target;

            IDirect3DSurface8* Lock;
            
            struct
            {
                IDirect3DTexture8* Textures[MAX_SURFACE_STAGE_COUNT];
                DWORD Stages[MAX_SURFACE_STAGE_COUNT][MAX_SURFACE_STAGE_VALUES_COUNT];
            } Staging;
        } Surfaces;

        struct
        {
            u32* Colors;

            struct
            {
                u8* RGB;
                u16* RGBA;

                u8* Alphas;
                u8* Colors;
                u8* Indexes;

                u32 Palette[MAX_PIXEL_PALETTE_BUFFER_SIZE];
            } Palette;

            u32 Pixels[MAX_PIXEL_BUFFER_SIZE];
        } Pixels;

        struct
        {
            struct
            {
                IDirect3DTexture8* Staging[MAX_SURFACE_STAGE_COUNT];
                IDirect3DTexture8* Textures[MAX_RENDER_TEXTURE_COUNT];
            } Surfaces;

            DXTexture Textures[MAX_TEXTURE_COUNT];
        } Textures;

        struct
        {
            BOOL IsChanged = TRUE;

            f32 Near, Far;

            f32 Unknown4; // TODO

            Mathematics::Matrix4x4 ModelView;

            Mathematics::Matrix4x4 Projection;

            Mathematics::Matrix4x4 O2W;

            Mathematics::Matrix4x4 W2L;

            const Mathematics::Matrix4x4 Identity
            {
                1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f
            };
        } Transform;

        struct
        {
            struct
            {
                u32 Count;
            } Indexes;

            struct
            {
                u32 Count;
            } RLVX;

            struct
            {
                u32 Count;
                Renderers::RTLVX Vertexes[RENDERER_TEN_THOUSAND_ELEMENT_COUNT];
            } RTLVX;

            struct
            {
                u32 Count;
            } RVX;
        } Data;

        struct
        {
            struct
            {
                f32x4 Actual[MAX_SHADER_REGISTER_COUNT];
                f32x4 Staging[MAX_SHADER_REGISTER_COUNT];
            } Constants;

            DXPS Pixel[MAX_PIXEL_SHADER_COUNT] =
            {
                {.Function = DX::Shaders::Pixel::DecalFunction },
                {.Function = DX::Shaders::Pixel::DecalTexKillFunction },
                {.Function = DX::Shaders::Pixel::BumpFunction },
                {.Function = DX::Shaders::Pixel::BumpSpecFunction },
                {.Function = DX::Shaders::Pixel::BumpCubicFunction },
                {.Function = DX::Shaders::Pixel::BumpSpecTexGenFunction },
                {.Function = DX::Shaders::Pixel::DetailFunction },
                {.Function = DX::Shaders::Pixel::GlossFunction },
            };

            DXVS Vertex[MAX_VERTEX_SHADER_COUNT] =
            {
                {
                    .Declaration = DX::Shaders::Vertex::DECLARATION_XYZ_NORMAL_UV,
                    .Function = DX::Shaders::Vertex::DirLightFunction
                },
                {
                    .Declaration = DX::Shaders::Vertex::DECLARATION_XYZ_NORMAL_UV,
                    .Function = DX::Shaders::Vertex::TexGenFunction
                },
                {
                    .Declaration = DX::Shaders::Vertex::DECLARATION_XYZ_NORMAL_UV,
                    .Function = DX::Shaders::Vertex::CubicFunction
                },
                {
                    .Declaration = DX::Shaders::Vertex::DECLARATION_XYZ_DIFFUSE_UV,
                    .Function = DX::Shaders::Vertex::PreLitFunction
                },
                {
                    .Declaration = DX::Shaders::Vertex::DECLARATION_XYZ_DIFFUSE_UV,
                    .Function = DX::Shaders::Vertex::PreLitTexGenFunction
                },
                {
                    .Declaration = DX::Shaders::Vertex::DECLARATION_XYZ_NORMAL_UV_BLENDWEIGHT_BLENDINDICES,
                    .Function = DX::Shaders::Vertex::BumpFunction
                },
                {
                    .Declaration = DX::Shaders::Vertex::DECLARATION_XYZ_NORMAL_UV_BLENDWEIGHT_BLENDINDICES,
                    .Function = DX::Shaders::Vertex::BumpSpecFunction
                },
                {
                    .Declaration = DX::Shaders::Vertex::DECLARATION_XYZ_NORMAL_UV_BLENDWEIGHT_BLENDINDICES,
                    .Function = DX::Shaders::Vertex::BumpCubicFunction
                },
                {
                    .Declaration = DX::Shaders::Vertex::DECLARATION_XYZ_NORMAL_UV_BLENDWEIGHT_BLENDINDICES,
                    .Function = DX::Shaders::Vertex::BumpDiffuseTexGenFunction
                },
                {
                    .Declaration = DX::Shaders::Vertex::DECLARATION_XYZ_NORMAL_UV_BLENDWEIGHT_BLENDINDICES,
                    .Function = DX::Shaders::Vertex::BumpSpecTexGenFunction
                },
                {
                    .Declaration = DX::Shaders::Vertex::DECLARATION_XYZ_NORMAL_UV,
                    .Function = DX::Shaders::Vertex::AlphaFogFunction
                },
                {
                    .Declaration = DX::Shaders::Vertex::DECLARATION_XYZ_NORMAL_UV,
                    .Function = DX::Shaders::Vertex::GlossFunction
                },
                {
                    .Declaration = DX::Shaders::Vertex::DECLARATION_XYZ_NORMAL_UV,
                    .Function = DX::Shaders::Vertex::ProjectedShadowFunction
                }
            };
        } Shaders;
    };

    void DXC(const HRESULT code, const char* message);

    BOOL InitializeState(void);
    BOOL SelectLight(void);
    u32 Alpha(const f32 value);
    u32 Color(const f32 r, const f32 g, const f32 b, const f32 a);
    void Initialize(void);
    void Release(void);
    void ReleaseOptimizedMeshes(void);
    void SelectRendererMode(const Renderers::RendererMode mode);
    void SelectTextureStageValue(const u32 stage, const D3DTEXTURESTAGESTATETYPE type, const DWORD value);
    void SetIndexes(const u16* indexes, const u32 count);
    void SetIndexSource(const u32 index, IDirect3DIndexBuffer8* buffer);
    void SetRendererState(const D3DRENDERSTATETYPE state, const DWORD value);
    void SetShaderConstants(void);
    void SetShaders(const DWORD vs, const DWORD ps);
}