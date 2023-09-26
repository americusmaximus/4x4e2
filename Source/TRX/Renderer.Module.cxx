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

#include "Renderer.hxx"
#include "Renderer.Module.hxx"
#include "Native.hxx"

using namespace Renderers;
using namespace Renderers::Modules;
using namespace Native;

namespace Renderer::Module
{
    RendererModuleContainer RendererModule;

    // 0x005c6960
    BOOL InitializeRendererModule(const HWND hwnd)
    {
        BOOL invalid = FALSE;

        if (hwnd != NULL) { *RendererModule.State._HWND = hwnd; }

        if (!*RendererState.Options._IsUsingModule) { return FALSE; }

        *RendererModule.State._Handle = InitializeModule(RendererState.Options._ModuleFileName);

        if (*RendererModule.State._Handle == NULL)
        {
            *RendererState.Options._IsUsingModule = FALSE;

            return FALSE;
        }

        *RendererModule._Init = (RENDERERINIT)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_INIT_NAME);
        if (*RendererModule._Init == NULL) { invalid = TRUE; }

        *RendererModule._Kill = (RENDERERKILL)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_KILL_NAME);
        if (*RendererModule._Kill == NULL) { invalid = TRUE; }

        *RendererModule._Toggle = (RENDERERTOGGLE)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_TOGGLE_NAME);
        if (*RendererModule._Toggle == NULL) { invalid = TRUE; }

        *RendererModule._AcquireGameWindowMode = (RENDERERACQUIREGAMEWINDOWMODE)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_ACQUIRE_GAME_WINDOW_MODE_NAME);
        *RendererModule._ToggleGameWindowMode = (RENDERERTOGGLEGAMEWINDOWMODE)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_TOGGLE_GAME_WINDOW_MODE_NAME);
        *RendererModule._LockFrameForScreenShot = (RENDERERLOCKFRAMEFORSCREENSHOT)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_LOCK_FRAME_FOR_SCREENSHOT_NAME);
        RendererModule.SetDirtyRect = (RENDERERSETDIRTYRECT)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_SET_DIRTY_RECT_NAME);

        *RendererModule._SelectVideoMode = (RENDERERSELECTVIDEOMODE)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_SELECT_VIDEO_MODE_NAME);
        if (*RendererModule._SelectVideoMode == NULL) { invalid = TRUE; }

        *RendererModule._RestoreVideoMode = (RENDERERRESTOREVIDEOMODE)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_RESTORE_VIDEO_MODE_NAME);
        if (*RendererModule._RestoreVideoMode == NULL) { invalid = TRUE; }

        *RendererModule._BeginScene = (RENDERERBEGINSCENE)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_BEGIN_SCENE_NAME);
        if (*RendererModule._BeginScene == NULL) { invalid = TRUE; }

        *RendererModule._EndScene = (RENDERERENDSCENE)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_END_SCENE_NAME);
        if (*RendererModule._EndScene == NULL) { invalid = TRUE; }

        *RendererModule._LockFrame = (RENDERERLOCKFRAME)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_LOCK_FRAME_NAME);
        if (*RendererModule._LockFrame == NULL) { invalid = TRUE; }

        *RendererModule._UnlockFrame = (RENDERERUNLOCKFRAME)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_UNLOCK_FRAME_NAME);
        if (*RendererModule._UnlockFrame == NULL) { invalid = TRUE; }

        *RendererModule._SelectTexture = (RENDERERSELECTTEXTURE)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_SELECT_TEXTURE_NAME);
        if (*RendererModule._SelectTexture == NULL) { invalid = TRUE; }

        *RendererModule._UpdateTexture = (RENDERERUPDATETEXTURE)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_UPDATE_TEXTURE_NAME);
        if (*RendererModule._UpdateTexture == NULL) { invalid = TRUE; }

        *RendererModule._SelectTextureBGRA = (RENDERERSELECTTEXTUREBGRA)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_SELECT_TEXTURE_BGRA_NAME);
        *RendererModule._UpdateTextureBGRA = (RENDERERUPDATETEXTUREBGRA)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_UPDATE_TEXTURE_BGRA_NAME);

        *RendererModule._Clear = (RENDERERCLEAR)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_CLEAR_NAME);
        if (*RendererModule._Clear == NULL) { invalid = TRUE; }

        *RendererModule._ClearZBuffer = (RENDERERCLEARZBUFFER)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_CLEAR_Z_BUFFER_NAME);
        if (*RendererModule._ClearZBuffer == NULL) { invalid = TRUE; }

        *RendererModule._SetColorTable = (RENDERERSETCOLORTABLE)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_SET_COLOR_TABLE_NAME);
        if (*RendererModule._SetColorTable == NULL) { invalid = TRUE; }

        *RendererModule._AcquireVideoMemory = (RENDERERACQUIREVIDEOMEMORY)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_ACQUIRE_VIDEO_MEMORY_NAME);

        *RendererModule._SelectGraphicsCard = (RENDERERSELECTGRAPHICSCARD)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_SELECT_GRAPHICS_CARD_NAME);
        if (*RendererModule._SelectGraphicsCard == NULL) { invalid = TRUE; }

        *RendererModule._AcquireGraphicsCardList = (RENDERERACQUIREGRAPHICSCARDLIST)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_ACQUIRE_GRAPHICS_CARD_LIST_NAME);
        if (*RendererModule._AcquireGraphicsCardList == NULL) { invalid = TRUE; }

        *RendererModule._AcquireTextureInformation = (RENDERERACQUIRETEXTUREINFORMATION)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_ACQUIRE_TEXTURE_INFORMATION_NAME);
        if (*RendererModule._AcquireTextureInformation == NULL) { invalid = TRUE; }

        RendererModule.AcquireModuleName = (RENDERERACQUIREMODULENAME)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_ACQUIRE_MODULE_NAME_NAME);

        *RendererModule._SetViewPort = (RENDERERSETVIEWPORT)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_SET_VIEWPORT_NAME);

        *RendererModule._SetLightConstants = (RENDERERSETLIGHTCONSTANTS)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_SET_LIGHT_CONSTANTS_NAME);
        *RendererModule._SetLightVector = (RENDERERSETLIGHTVECTOR)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_SET_LIGHT_VECTOR_NAME);
        *RendererModule._SetAmbientLight = (RENDERERSETAMBIENTLIGHT)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_SET_AMBIENT_LIGHT_NAME);
        *RendererModule._SetLightColor = (RENDERERSETLIGHTCOLOR)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_SET_LIGHT_COLOR_NAME);

        *RendererModule._SetTransform = (RENDERERSETTRANSFORM)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_SET_TRANSFORM_NAME);
        *RendererModule._SetTransformW2L = (RENDERERSETTRANSFORMW2L)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_SET_TRANSFORM_W2L_NAME);
        *RendererModule._SetTransformO2W = (RENDERERSETTRANSFORMO2W)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_SET_TRANSFORM_O2W_NAME);

        *RendererModule._PolyList = (RENDERERPOLYLIST)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_POLY_LIST_NAME);
        *RendererModule._PolyListD3DL = (RENDERERPOLYLISTD3DL)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_POLY_LIST_D3DL_NAME);
        *RendererModule._PolyListD3DTL = (RENDERERPOLYLISTD3DTL)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_POLY_LIST_D3DTL_NAME);
        *RendererModule._PolyListTL = (RENDERERPOLYLISTTL)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_POLY_LIST_TL_NAME);

        *RendererModule._DotListD3DL = (RENDERERDOTLISTD3DL)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_DOT_LIST_D3DL_NAME);
        *RendererModule._DotListD3DTL = (RENDERERDOTLISTD3DTL)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_DOT_LIST_D3DTL_NAME);

        *RendererModule._LineListD3DL = (RENDERERLINELISTD3DL)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_LINE_LIST_D3DL_NAME);
        *RendererModule._LineListD3DTL = (RENDERERLINELISTD3DTL)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_LINE_LIST_D3DTL_NAME);

        *RendererModule._AcquireTextureHandle = (RENDERERACQUIRETEXTUREHANDLE)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_ACQUIRE_TEXTURE_HANDLE_NAME);
        *RendererModule._SelectTextureByHandle = (RENDERERSELECTTEXTUREBYHANDLE)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_SELECT_TEXTURE_BY_HANDLE);
        *RendererModule._ResetTextureCache = (RENDERERRESETTEXTURECACHE)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_RESET_TEXTURE_CACHE_NAME);

        *RendererModule._EnableCulling = (RENDERERENABLECULLING)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_ENABLE_CULLING_NAME);
        *RendererModule._EnableClipping = (RENDERERENABLECLIPPING)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_ENABLE_CLIPPING_NAME);

        *RendererModule._SetTextureClamp = (RENDERERSETTEXTURECLAMP)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_SET_TEXTURE_CLAMP_NAME);

        *RendererModule._SetFog = (RENDERERSETFOG)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_SET_FOG_NAME);

        *RendererModule._ResetDynamicLight = (RENDERERRESETDYNAMICLIGHT)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_RESET_DYNAMIC_LIGHT_NAME);
        *RendererModule._AddDynamicLight = (RENDERERADDDYNAMICLIGHT)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_ADD_DYNAMIC_LIGHT_NAME);
        *RendererModule._DynamicLightHint = (RENDERERDYNAMICLIGHTHINT)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_DYNAMIC_LIGHT_HINT_NAME);

        *RendererModule._EnableLighting = (RENDERERENABLELIGHTING)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_ENABLE_LIGHTING_NAME);

        *RendererModule._DrawPixels = (RENDERERDRAWPIXELS)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_DRAW_PIXELS_NAME);

        *RendererModule._VideoSave = (RENDERERVIDEOSAVE)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_VIDEO_SAVE_NAME);
        *RendererModule._VideoRestore = (RENDERERVIDEORESTORE)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_VIDEO_RESTORE_NAME);

        *RendererModule._SetMultiTextureBlend = (RENDERERSETMULTITEXTUREBLEND)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_SET_MULTI_TEXTURE_BLEND_NAME);

        *RendererModule._BeginCubeMapRender = (RENDERERBEGINCUBEMAPRENDER)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_BEGIN_CUBE_MAP_RENDER_NAME);
        *RendererModule._EndCubeMapRender = (RENDERERENDCUBEMAPRENDER)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_END_CUBE_MAP_RENDER_NAME);
        *RendererModule._SelectCubeTexture = (RENDERERSELECTCUBETEXTURE)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_SELECT_CUBE_TEXTURE_NAME);

        *RendererModule._BeginRenderToTexture = (RENDERERBEGINRENDERTOTEXTURE)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_BEGIN_RENDER_TO_TEXTURE_NAME);
        *RendererModule._EndRenderToTexture = (RENDERERENDRENDERTOTEXTURE)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_END_RENDER_TO_TEXTURE_NAME);
        *RendererModule._SelectRenderTexture = (RENDERERSELECTRENDERTEXTURE)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_SELECT_RENDER_TEXTURE_NAME);

        *RendererModule._AllocateOptimizedMesh = (RENDERERALLOCATEOPTIMIZEDMESH)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_ALLOCATE_OPTIMIZED_MESH_NAME); *RendererModule._FreeOptimizedMesh = (RENDERERFREEOPTIMIZEDMESH)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_FREE_OPTIMIZED_MESH_NAME);
        *RendererModule._FreeOptimizedMesh = (RENDERERFREEOPTIMIZEDMESH)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_FREE_OPTIMIZED_MESH_NAME);
        *RendererModule._RenderOptimizedMesh = (RENDERERRENDEROPTIMIZEDMESH)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_RENDER_OPTIMIZED_MESH_NAME);

        *RendererModule._SetAlpha = (RENDERERSETALPHA)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_SET_ALPHA_NAME);

        *RendererModule._SetStencilEnable = (RENDERERSETSTENCILENABLE)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_SET_STENCIL_ENABLE_NAME);
        *RendererModule._SetStencilFunc = (RENDERERSETSTENCILFUNC)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_SET_STENCIL_FUNC_NAME);
        *RendererModule._SetStencilPass = (RENDERERSETSTENCILPASS)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_SET_STENCIL_PASS_NAME);

        *RendererModule._SetSourceBlend = (RENDERERSETSOURCEBLEND)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_SET_SOURCE_BLEND_NAME);
        *RendererModule._SetDestinationBlend = (RENDERERSETDESTINATIONBLEND)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_SET_DESTINATION_BLEND_NAME);

        *RendererModule._SetGloss = (RENDERERSETGLOSS)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_SET_GLOSS_NAME);

        if (!invalid)
        {
            *RendererModule.State._IsActive = TRUE;

            const RendererModuleInitializeArguments args =
            {
                .R =
                {
                    .Display = (u32*)0x0081f764, // TODO
                    .Font = (u32*)0x0081f768, // TODO
                    .BitsPerPixel = (u32*)0x0081f76c // TODO
                },
                .G =
                {
                    .Display = (u32*)0x0081f770, // TODO
                    .Font = (u32*)0x0081f774, // TODO
                    .BitsPerPixel = (u32*)0x0081f778 // TODO
                },
                .B =
                {
                    .Display = (u32*)0x0081f77c, // TODO
                    .Font = (u32*)0x0081f780, // TODO
                    .BitsPerPixel = (u32*)0x0081f784 // TODO
                },
                .Mode = (RendererAcceleratedGraphicsPortMode*)0x00d44cb0, // TODO
                .Unknown2 = (void*)0x00d44cb8 // TODO
            };

            if ((*RendererModule._Init)(*RendererModule.State._HWND, &args))
            {
                if (RendererModule.AcquireModuleName != NULL)
                {
                    if (!RendererModule.AcquireModuleName(RendererState.Options.ModuleName))
                    {
                        strcpy(RendererState.Options.ModuleName, RendererState.Options._ModuleFileName);
                    }
                }
                else
                {
                    strcpy(RendererState.Options.ModuleName, RendererState.Options._ModuleFileName);
                }

                SelectRendererGraphicsCard(*RendererModule.State._SelectedDeviceIndex);

                return TRUE;
            }

            *RendererState.Options._IsUsingModule = FALSE;

            return FALSE;
        }

        ReleaseRendererModule();

        *RendererState.Options._IsUsingModule = FALSE;

        return FALSE;
    }

    // 0x005c5f50
    void ReleaseRendererModule(void)
    {
        *RendererState._IsActive = FALSE;
        *RendererModule.State._IsActive = FALSE;

        if (*RendererModule.State._Handle != NULL)
        {
            ReleaseModule(*RendererModule.State._Handle);

            *RendererModule.State._Handle = NULL;
        }

        RendererModule.AcquireModuleName = NULL;

        *RendererModule._Kill = NULL;
        *RendererModule._Toggle = NULL;
        *RendererModule._AcquireGameWindowMode = NULL;
        *RendererModule._ToggleGameWindowMode = NULL;
        *RendererModule._LockFrameForScreenShot = NULL;
        RendererModule.SetDirtyRect = NULL;
        *RendererModule._SelectVideoMode = NULL;
        *RendererModule._RestoreVideoMode = NULL;
        *RendererModule._BeginScene = NULL;
        *RendererModule._EndScene = NULL;
        *RendererModule._LockFrame = NULL;
        *RendererModule._UnlockFrame = NULL;
        *RendererModule._SelectTexture = NULL;
        *RendererModule._UpdateTexture = NULL;
        *RendererModule._Clear = NULL;
        *RendererModule._ClearZBuffer = NULL;
        *RendererModule._SetColorTable = NULL;
        *RendererModule._AcquireVideoMemory = NULL;
        *RendererModule._SelectGraphicsCard = NULL;
        *RendererModule._AcquireGraphicsCardList = NULL;
        *RendererModule._AcquireTextureInformation = NULL;
        *RendererModule._SetViewPort = NULL;
        *RendererModule._SetLightConstants = NULL;
        *RendererModule._SetLightVector = NULL;
        *RendererModule._SetAmbientLight = NULL;
        *RendererModule._SetLightColor = NULL;
        *RendererModule._SetTransform = NULL;
        *RendererModule._SetTransformW2L = NULL;
        *RendererModule._SetTransformO2W = NULL;
        *RendererModule._PolyList = NULL;
        *RendererModule._PolyListD3DL = NULL;
        *RendererModule._PolyListD3DTL = NULL;
        *RendererModule._PolyListTL = NULL;
        *RendererModule._DotListD3DL = NULL;
        *RendererModule._DotListD3DTL = NULL;
        *RendererModule._LineListD3DL = NULL;
        *RendererModule._LineListD3DTL = NULL;
        *RendererModule._AcquireTextureHandle = NULL;
        *RendererModule._SelectTextureByHandle = NULL;
        *RendererModule._ResetTextureCache = NULL;
        *RendererModule._EnableCulling = NULL;
        *RendererModule._EnableClipping = NULL;
        *RendererModule._SetTextureClamp = NULL;
        *RendererModule._SetFog = NULL;
        *RendererModule._ResetDynamicLight = NULL;
        *RendererModule._AddDynamicLight = NULL;
        *RendererModule._DynamicLightHint = NULL;
        *RendererModule._EnableLighting = NULL;
        *RendererModule._DrawPixels = NULL;
        *RendererModule._VideoSave = NULL;
        *RendererModule._VideoRestore = NULL;
        *RendererModule._SetMultiTextureBlend = NULL;
        *RendererModule._BeginCubeMapRender = NULL;
        *RendererModule._EndCubeMapRender = NULL;
        *RendererModule._SelectCubeTexture = NULL;
        *RendererModule._BeginRenderToTexture = NULL;
        *RendererModule._EndRenderToTexture = NULL;
        *RendererModule._SelectRenderTexture = NULL;
        *RendererModule._AllocateOptimizedMesh = NULL;
        *RendererModule._FreeOptimizedMesh = NULL;
        *RendererModule._RenderOptimizedMesh = NULL;
        *RendererModule._SetAlpha = NULL;
        *RendererModule._SetStencilEnable = NULL;
        *RendererModule._SetStencilPass = NULL;
        *RendererModule._SetStencilFunc = NULL;
        *RendererModule._SetSourceBlend = NULL;
        *RendererModule._SetDestinationBlend = NULL;
        *RendererModule._SetGloss = NULL;
        *RendererModule._Init = NULL;
    }
}