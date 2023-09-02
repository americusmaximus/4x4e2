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
#include "Quit.hxx"

using namespace Renderer::Module;

namespace DX
{
    void DXC(const HRESULT code, const char* message)
    {
        if (code == D3D_OK) { return; }

        const char* description = NULL;

        switch (code)
        {
        case D3DERR_CONFLICTINGRENDERSTATE: { description = "The currently set render states cannot be used together."; break; }
        case D3DERR_CONFLICTINGTEXTUREFILTER: { description = "The current texture filters cannot be used together."; break; };
        case D3DERR_CONFLICTINGTEXTUREPALETTE: { description = "The current textures cannot be used simultaneously."; break; }
        case D3DERR_DEVICELOST: { description = "The device is lost and cannot be restored at the current time, so rendering is not possible."; break; }
        case D3DERR_DEVICENOTRESET: { description = "The device cannot be reset."; break; }
        case D3DERR_DRIVERINTERNALERROR: { description = "Internal driver error."; break; }
        case D3DERR_INVALIDCALL: { description = "The method call is invalid. For example, a method's parameter may not be a valid pointer."; break; }
        case D3DERR_INVALIDDEVICE: { description = "The requested device type is not valid."; break; }
        case D3DERR_MOREDATA: { description = "There is more data available than the specified buffer size can hold."; break; }
        case D3DERR_NOTAVAILABLE: { description = "This device does not support the queried technique."; break; }
        case D3DERR_NOTFOUND: { description = "The requested item was not found."; break; }
        case D3DERR_OUTOFVIDEOMEMORY: { description = "Direct3D does not have enough display memory to perform the operation."; break; }
        case D3DERR_TOOMANYOPERATIONS: { description = "The application is requesting more texture-filtering operations than the device supports."; break; }
        case D3DERR_UNSUPPORTEDALPHAARG: { description = "The device does not support a specified texture-blending argument for the alpha channel."; break; }
        case D3DERR_UNSUPPORTEDALPHAOPERATION: { description = "The device does not support a specified texture-blending operation for the alpha channel."; break; }
        case D3DERR_UNSUPPORTEDCOLORARG: { description = "The device does not support a specified texture-blending argument for color values."; break; }
        case D3DERR_UNSUPPORTEDCOLOROPERATION: { description = "The device does not support a specified texture-blending operation for color values."; break; }
        case D3DERR_UNSUPPORTEDFACTORVALUE: { description = "The device does not support the specified texture factor value."; break; }
        case D3DERR_UNSUPPORTEDTEXTUREFILTER: { description = "The device does not support the specified texture filter."; break; }
        case D3DERR_WRONGTEXTUREFORMAT: { description = "The pixel format of the texture surface is not valid."; break; }
        case E_FAIL: { description = "An undetermined error occurred inside the Direct3D subsystem."; break; }
        case E_INVALIDARG: { description = "An invalid parameter was passed to the returning function."; break; }
        case E_OUTOFMEMORY: { description = "Direct3D could not allocate sufficient memory to complete the call."; break; }
        }

        Quit("DirectX: %s %s", message, description ? description : "An unknown error code was returned.");
    }
}