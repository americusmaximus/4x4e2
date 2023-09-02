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

#define DIRECT3D_VERSION 0x0900
#include <d3d9.h>

#define D3DUSAGE_NONE 0

#define MAX_STATE_VARIABLE_INDEX_MASK (0xff)
#define MAX_STATE_VARIABLE_COUNT 256

#define MAX_TEXTURE_CACHE_ITEM_NAME_LENGTH 64

#define MAX_PIXEL_PALETTE_BUFFER_SIZE 256
#define MAX_PIXEL_BUFFER_SIZE (1024 * 1024 + 512 * 512 + 256 * 256 + 128 * 128 + 64 * 64 + 32 * 32 + 16 * 16 + 8 * 8 + 4 * 4 + 2 * 2 + 1 * 1)

#define MAX_SURFACE_STAGE_COUNT 8
#define MAX_SURFACE_STAGE_VALUES_COUNT 255

#define MAX_SURFACE_SAMPLER_COUNT 8
#define MAX_SURFACE_SAMPLER_VALUES_COUNT 255

#define MAX_TEXTURE_COUNT 4096
#define MAX_MIP_MAP_SURFACE_COUNT 12

#define MAX_BLEND_TEXTURE_COUNT 4

#define TEXTURE_STAGE_0_INDEX 0
#define TEXTURE_STAGE_1_INDEX 1
#define TEXTURE_STAGE_2_INDEX 2
#define TEXTURE_STAGE_3_INDEX 3

#define STREAM_SOURCE_0_INDEX 0
#define STREAM_SOURCE_1_INDEX 1

#define RENDER_TARGET_0_INDEX 0

namespace DX
{
    struct DXDevice
    {
        GUID ID;

        u32 DeviceID;
        u32 VendorID;

        char Driver[MAX_DEVICE_IDENTIFIER_STRING];
        char Description[MAX_DEVICE_IDENTIFIER_STRING];
    };

    struct DXTexture
    {
        IDirect3DTexture9* Texture;
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
            IDirect3D9* Instance;
            IDirect3DDevice9* Device;

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

                D3DCAPS9 Capabilities;
                D3DPRESENT_PARAMETERS PresentParams;
            } Current;
        } Devices;

        struct
        {
            Renderers::RendererMode Mode;
            D3DCULL Cull = D3DCULL::D3DCULL_CCW;

            struct
            {
                Renderers::RendererBlendOperation BlendOperation;
                D3DBLEND SourceBlendMode = D3DBLEND::D3DBLEND_SRCALPHA;
                D3DBLEND DestinationBlendMode = D3DBLEND::D3DBLEND_INVSRCALPHA;
            } Blend;

            struct
            {
                Renderers::RendererBlendOperation ClampOperation = Renderers::RendererBlendOperation::All;
                Renderers::RendererClampMode U[MAX_SURFACE_STAGE_COUNT];
                Renderers::RendererClampMode V[MAX_SURFACE_STAGE_COUNT];
            } Clamp;
        } Mode;

        struct
        {
            IDirect3DIndexBuffer9* Index;
            IDirect3DVertexBuffer9* RTLVX;
        } Buffers;

        struct
        {
            BOOL IsActive;
        } Lock;

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

            IDirect3DSurface9* Opaque[MAX_MIP_MAP_SURFACE_COUNT];
            IDirect3DSurface9* Transparent[MAX_MIP_MAP_SURFACE_COUNT];

            struct
            {
                IDirect3DSurface9* Surface;
                IDirect3DSurface9* DepthSurface;
            } Target;

            IDirect3DSurface9* Lock;

            struct
            {
                IDirect3DTexture9* Textures[MAX_SURFACE_STAGE_COUNT];
                DWORD Stages[MAX_SURFACE_STAGE_COUNT][MAX_SURFACE_STAGE_VALUES_COUNT];
                DWORD Sampling[MAX_SURFACE_SAMPLER_COUNT][MAX_SURFACE_SAMPLER_VALUES_COUNT];
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
                IDirect3DTexture9* Staging[MAX_SURFACE_STAGE_COUNT];
            } Surfaces;

            DXTexture Textures[MAX_TEXTURE_COUNT];
        } Textures;

        struct
        {
            struct
            {
                u32 Count;
            } Indexes;

            struct
            {
                u32 Count;
                Renderers::RTLVX Vertexes[RENDERER_TEN_THOUSAND_ELEMENT_COUNT];
            } RTLVX;
        } Data;
    };

    void DXC(const HRESULT code, const char* message);

    BOOL InitializeState(void);
    BOOL SelectLight(void);
    u32 Alpha(const f32 value);
    u32 Color(const f32 r, const f32 g, const f32 b, const f32 a);
    void Initialize(void);
    void Release(void);
    void SelectRendererMode(const Renderers::RendererMode mode);
    void SelectTextureStageValue(const u32 stage, const D3DTEXTURESTAGESTATETYPE type, const DWORD value);
    void SelectSamplerState(const u32 sampler, const D3DSAMPLERSTATETYPE type, const DWORD value);
    void SetRendererState(const D3DRENDERSTATETYPE state, const DWORD value);
    void SetShader(const DWORD vs);
}