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

#include "App.Windows.hxx"
#include "Editor.Clipboard.hxx"
#include "Memory.hxx"
#include "Native.hxx"
#include "Strings.hxx"

using namespace App::Windows;
using namespace Memory;
using namespace Strings;

namespace Editor
{
    ClipboardContainer ClipboardState;

    // 0x00444d00
    char* AcquireClipboardValue(Editor* self)
    {
        if (OpenClipboard(*AppWindowState._HWND))
        {
            auto mem = (char*)GetClipboardData(CF_TEXT);

            if (mem != NULL)
            {
                auto length = strlen(mem);

                ClipboardState.Value = (char*)ReallocateMemory(ClipboardState.Value, length);

                CopyMemory(ClipboardState.Value, mem, length);
            }

            CloseClipboard();
        }

        return IsNotNull(ClipboardState.Value) ? ClipboardState.Value : (char*)&ClipboardState.NullOutString[0];
    }

    // 0x00444d80
    void SelectClipboardValue(Editor* self, const char* value)
    {
        const auto text = IsNull(value) ? (char*)&ClipboardState.NullInString[0] : value;
        const auto length = strlen(text);

        if (OpenClipboard(*AppWindowState._HWND))
        {
            auto mem = GlobalAlloc(GMEM_SHARE | GMEM_MOVEABLE, length);

            if (mem != NULL)
            {
                auto data = GlobalLock(mem);

                if (data == NULL)
                {
                    GlobalFree(mem);
                }
                else
                {
                    CopyMemory(data, text, length);

                    GlobalUnlock(mem);

                    SetClipboardData(CF_TEXT, mem);
                }
            }

            CloseClipboard();
        }

        ClipboardState.Value = (char*)ReallocateMemory(ClipboardState.Value, length);

        if (ClipboardState.Value == NULL) { return; }

        CopyMemory(ClipboardState.Value, text, length);
    }
}