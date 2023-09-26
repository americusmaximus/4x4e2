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

#include "Renderers.Modules.Import.hxx"

#define MAX_RENDERER_MODULE_FILE_NAME_LENGTH 256
#define MAX_RENDERER_MODULE_NAME_LENGTH 256

namespace Renderer::Module
{
    struct RendererModuleContainer
    {
        struct
        {
            HWND* _HWND = (HWND*)0x00d44df0; // TODO
            BOOL* _IsActive = (BOOL*)0x00d44ddc; // TODO
            HMODULE* _Handle = (HMODULE*)0x00d44de0; // TODO

            u32* _SelectedDeviceIndex = (u32*)0x00d44cbc; // TODO
        } State;

        Renderers::Modules::RENDERERACQUIREMODULENAME AcquireModuleName;

        Renderers::Modules::RENDERERINIT* _Init = (Renderers::Modules::RENDERERINIT*)0x00d44cc0; // TODO
        Renderers::Modules::RENDERERKILL* _Kill = (Renderers::Modules::RENDERERKILL*)0x00d44cc4; // TODO
        Renderers::Modules::RENDERERTOGGLE* _Toggle = (Renderers::Modules::RENDERERTOGGLE*)0x00d44cc8; // TODO
        Renderers::Modules::RENDERERACQUIREGAMEWINDOWMODE* _AcquireGameWindowMode = (Renderers::Modules::RENDERERACQUIREGAMEWINDOWMODE*)0x00d44ccc; // TODO
        Renderers::Modules::RENDERERTOGGLEGAMEWINDOWMODE* _ToggleGameWindowMode = (Renderers::Modules::RENDERERTOGGLEGAMEWINDOWMODE*)0x00d44cd0; // TODO
        Renderers::Modules::RENDERERLOCKFRAMEFORSCREENSHOT* _LockFrameForScreenShot = (Renderers::Modules::RENDERERLOCKFRAMEFORSCREENSHOT*)0x00d44cd4; // TODO
        Renderers::Modules::RENDERERSETDIRTYRECT SetDirtyRect; // 0x00d44cd8
        Renderers::Modules::RENDERERSELECTVIDEOMODE* _SelectVideoMode = (Renderers::Modules::RENDERERSELECTVIDEOMODE*)0x00d44cdc; // TODO
        Renderers::Modules::RENDERERRESTOREVIDEOMODE* _RestoreVideoMode = (Renderers::Modules::RENDERERRESTOREVIDEOMODE*)0x00d44ce0; // TODO
        Renderers::Modules::RENDERERBEGINSCENE* _BeginScene = (Renderers::Modules::RENDERERBEGINSCENE*)0x00d44ce4; // TODO
        Renderers::Modules::RENDERERENDSCENE* _EndScene = (Renderers::Modules::RENDERERENDSCENE*)0x00d44ce8; // TODO
        Renderers::Modules::RENDERERLOCKFRAME* _LockFrame = (Renderers::Modules::RENDERERLOCKFRAME*)0x00d44cec; // TODO
        Renderers::Modules::RENDERERUNLOCKFRAME* _UnlockFrame = (Renderers::Modules::RENDERERUNLOCKFRAME*)0x00d44cf0; // TODO
        Renderers::Modules::RENDERERSELECTTEXTURE* _SelectTexture = (Renderers::Modules::RENDERERSELECTTEXTURE*)0x00d44cf4; // TODO
        Renderers::Modules::RENDERERUPDATETEXTURE* _UpdateTexture = (Renderers::Modules::RENDERERUPDATETEXTURE*)0x00d44cf8; // TODO
        Renderers::Modules::RENDERERSELECTTEXTUREBGRA* _SelectTextureBGRA = (Renderers::Modules::RENDERERSELECTTEXTUREBGRA*)0x00d44cfc; // TODO
        Renderers::Modules::RENDERERUPDATETEXTUREBGRA* _UpdateTextureBGRA = (Renderers::Modules::RENDERERUPDATETEXTUREBGRA*)0x00d44d00; // TODO
        Renderers::Modules::RENDERERCLEAR* _Clear = (Renderers::Modules::RENDERERCLEAR*)0x00d44d04; // TODO
        Renderers::Modules::RENDERERCLEARZBUFFER* _ClearZBuffer = (Renderers::Modules::RENDERERCLEARZBUFFER*)0x00d44d08; // TODO
        Renderers::Modules::RENDERERSETCOLORTABLE* _SetColorTable = (Renderers::Modules::RENDERERSETCOLORTABLE*)0x00d44d0c; // TODO
        Renderers::Modules::RENDERERACQUIREVIDEOMEMORY* _AcquireVideoMemory = (Renderers::Modules::RENDERERACQUIREVIDEOMEMORY*)0x00d44d10; // TODO
        Renderers::Modules::RENDERERSELECTGRAPHICSCARD* _SelectGraphicsCard = (Renderers::Modules::RENDERERSELECTGRAPHICSCARD*)0x00d44d14; // TODO
        Renderers::Modules::RENDERERACQUIREGRAPHICSCARDLIST* _AcquireGraphicsCardList = (Renderers::Modules::RENDERERACQUIREGRAPHICSCARDLIST*)0x00d44d18; // TODO
        Renderers::Modules::RENDERERACQUIRETEXTUREINFORMATION* _AcquireTextureInformation = (Renderers::Modules::RENDERERACQUIRETEXTUREINFORMATION*)0x00d44d1c; // TODO
        Renderers::Modules::RENDERERSETVIEWPORT* _SetViewPort = (Renderers::Modules::RENDERERSETVIEWPORT*)0x00d44d20; // TODO
        Renderers::Modules::RENDERERSETLIGHTCONSTANTS* _SetLightConstants = (Renderers::Modules::RENDERERSETLIGHTCONSTANTS*)0x00d44d24; // TODO
        Renderers::Modules::RENDERERSETLIGHTVECTOR* _SetLightVector = (Renderers::Modules::RENDERERSETLIGHTVECTOR*)0x00d44d28; // TODO
        Renderers::Modules::RENDERERSETAMBIENTLIGHT* _SetAmbientLight = (Renderers::Modules::RENDERERSETAMBIENTLIGHT*)0x00d44d2c; // TODO
        Renderers::Modules::RENDERERSETLIGHTCOLOR* _SetLightColor = (Renderers::Modules::RENDERERSETLIGHTCOLOR*)0x00d44d30; // TODO
        Renderers::Modules::RENDERERSETTRANSFORM* _SetTransform = (Renderers::Modules::RENDERERSETTRANSFORM*)0x00d44d34; // TODO
        Renderers::Modules::RENDERERSETTRANSFORMW2L* _SetTransformW2L = (Renderers::Modules::RENDERERSETTRANSFORMW2L*)0x00d44d38; // TODO
        Renderers::Modules::RENDERERSETTRANSFORMO2W* _SetTransformO2W = (Renderers::Modules::RENDERERSETTRANSFORMO2W*)0x00d44d3c; // TODO
        Renderers::Modules::RENDERERPOLYLIST* _PolyList = (Renderers::Modules::RENDERERPOLYLIST*)0x00d44d40; // TODO
        Renderers::Modules::RENDERERPOLYLISTD3DL* _PolyListD3DL = (Renderers::Modules::RENDERERPOLYLISTD3DL*)0x00d44d44; // TODO
        Renderers::Modules::RENDERERPOLYLISTD3DTL* _PolyListD3DTL = (Renderers::Modules::RENDERERPOLYLISTD3DTL*)0x00d44d48; // TODO
        Renderers::Modules::RENDERERPOLYLISTTL* _PolyListTL = (Renderers::Modules::RENDERERPOLYLISTTL*)0x00d44d4c; // TODO
        Renderers::Modules::RENDERERDOTLISTD3DL* _DotListD3DL = (Renderers::Modules::RENDERERDOTLISTD3DL*)0x00d44d50; // TODO
        Renderers::Modules::RENDERERDOTLISTD3DTL* _DotListD3DTL = (Renderers::Modules::RENDERERDOTLISTD3DTL*)0x00d44d54; // TODO
        Renderers::Modules::RENDERERLINELISTD3DL* _LineListD3DL = (Renderers::Modules::RENDERERLINELISTD3DL*)0x00d44d58; // TODO
        Renderers::Modules::RENDERERLINELISTD3DTL* _LineListD3DTL = (Renderers::Modules::RENDERERLINELISTD3DTL*)0x00d44d5c; // TODO
        Renderers::Modules::RENDERERACQUIRETEXTUREHANDLE* _AcquireTextureHandle = (Renderers::Modules::RENDERERACQUIRETEXTUREHANDLE*)0x00d44d60; // TODO
        Renderers::Modules::RENDERERSELECTTEXTUREBYHANDLE* _SelectTextureByHandle = (Renderers::Modules::RENDERERSELECTTEXTUREBYHANDLE*)0x00d44d64; // TODO
        Renderers::Modules::RENDERERRESETTEXTURECACHE* _ResetTextureCache = (Renderers::Modules::RENDERERRESETTEXTURECACHE*)0x00d44d68; // TODO
        Renderers::Modules::RENDERERENABLECULLING* _EnableCulling = (Renderers::Modules::RENDERERENABLECULLING*)0x00d44d6c; // TODO
        Renderers::Modules::RENDERERENABLECLIPPING* _EnableClipping = (Renderers::Modules::RENDERERENABLECLIPPING*)0x00d44d70; // TODO
        Renderers::Modules::RENDERERSETTEXTURECLAMP* _SetTextureClamp = (Renderers::Modules::RENDERERSETTEXTURECLAMP*)0x00d44d74; // TODO
        Renderers::Modules::RENDERERSETFOG* _SetFog = (Renderers::Modules::RENDERERSETFOG*)0x00d44d78; // TODO
        Renderers::Modules::RENDERERRESETDYNAMICLIGHT* _ResetDynamicLight = (Renderers::Modules::RENDERERRESETDYNAMICLIGHT*)0x00d44d7c; // TODO
        Renderers::Modules::RENDERERADDDYNAMICLIGHT* _AddDynamicLight = (Renderers::Modules::RENDERERADDDYNAMICLIGHT*)0x00d44d80; // TODO
        Renderers::Modules::RENDERERDYNAMICLIGHTHINT* _DynamicLightHint = (Renderers::Modules::RENDERERDYNAMICLIGHTHINT*)0x00d44d84; // TODO
        Renderers::Modules::RENDERERENABLELIGHTING* _EnableLighting = (Renderers::Modules::RENDERERENABLELIGHTING*)0x00d44d88; // TODO
        Renderers::Modules::RENDERERDRAWPIXELS* _DrawPixels = (Renderers::Modules::RENDERERDRAWPIXELS*)0x00d44d8c; // TODO
        Renderers::Modules::RENDERERVIDEOSAVE* _VideoSave = (Renderers::Modules::RENDERERVIDEOSAVE*)0x00d44d90; // TODO
        Renderers::Modules::RENDERERVIDEORESTORE* _VideoRestore = (Renderers::Modules::RENDERERVIDEORESTORE*)0x00d44d94; // TODO
        Renderers::Modules::RENDERERSETMULTITEXTUREBLEND* _SetMultiTextureBlend = (Renderers::Modules::RENDERERSETMULTITEXTUREBLEND*)0x00d44d98; // TODO
        Renderers::Modules::RENDERERBEGINCUBEMAPRENDER* _BeginCubeMapRender = (Renderers::Modules::RENDERERBEGINCUBEMAPRENDER*)0x00d44d9c; // TODO
        Renderers::Modules::RENDERERENDCUBEMAPRENDER* _EndCubeMapRender = (Renderers::Modules::RENDERERENDCUBEMAPRENDER*)0x00d44da0; // TODO
        Renderers::Modules::RENDERERSELECTCUBETEXTURE* _SelectCubeTexture = (Renderers::Modules::RENDERERSELECTCUBETEXTURE*)0x00d44da4; // TODO
        Renderers::Modules::RENDERERBEGINRENDERTOTEXTURE* _BeginRenderToTexture = (Renderers::Modules::RENDERERBEGINRENDERTOTEXTURE*)0x00d44da8; // TODO
        Renderers::Modules::RENDERERENDRENDERTOTEXTURE* _EndRenderToTexture = (Renderers::Modules::RENDERERENDRENDERTOTEXTURE*)0x00d44dac; // TODO
        Renderers::Modules::RENDERERSELECTRENDERTEXTURE* _SelectRenderTexture = (Renderers::Modules::RENDERERSELECTRENDERTEXTURE*)0x00d44db0; // TODO
        Renderers::Modules::RENDERERALLOCATEOPTIMIZEDMESH* _AllocateOptimizedMesh = (Renderers::Modules::RENDERERALLOCATEOPTIMIZEDMESH*)0x00d44db4; // TODO
        Renderers::Modules::RENDERERFREEOPTIMIZEDMESH* _FreeOptimizedMesh = (Renderers::Modules::RENDERERFREEOPTIMIZEDMESH*)0x00d44db8; // TODO
        Renderers::Modules::RENDERERRENDEROPTIMIZEDMESH* _RenderOptimizedMesh = (Renderers::Modules::RENDERERRENDEROPTIMIZEDMESH*)0x00d44dbc; // TODO
        Renderers::Modules::RENDERERSETALPHA* _SetAlpha = (Renderers::Modules::RENDERERSETALPHA*)0x00d44dc0; // TODO
        Renderers::Modules::RENDERERSETSTENCILENABLE* _SetStencilEnable = (Renderers::Modules::RENDERERSETSTENCILENABLE*)0x00d44dc4; // TODO
        Renderers::Modules::RENDERERSETSTENCILPASS* _SetStencilPass = (Renderers::Modules::RENDERERSETSTENCILPASS*)0x00d44dc8; // TODO
        Renderers::Modules::RENDERERSETSTENCILFUNC* _SetStencilFunc = (Renderers::Modules::RENDERERSETSTENCILFUNC*)0x00d44dcc; // TODO
        Renderers::Modules::RENDERERSETSOURCEBLEND* _SetSourceBlend = (Renderers::Modules::RENDERERSETSOURCEBLEND*)0x00d44dd0; // TODO
        Renderers::Modules::RENDERERSETDESTINATIONBLEND* _SetDestinationBlend = (Renderers::Modules::RENDERERSETDESTINATIONBLEND*)0x00d44dd4; // TODO
        Renderers::Modules::RENDERERSETGLOSS* _SetGloss = (Renderers::Modules::RENDERERSETGLOSS*)0x00d44dd8; // TODO
    };

    extern RendererModuleContainer RendererModule;

    BOOL InitializeRendererModule(const HWND hwnd);
    void ReleaseRendererModule(void);
}