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

#include "Editor.Clipboard.hxx"
#include "Editor.Window.hxx"
#include "Editor.hxx"
#include "Graphics.hxx"
#include "Logger.hxx"
#include "Memory.hxx"

using namespace Graphics;
using namespace Logger;
using namespace Memory;
using namespace Objects;

namespace Editor
{
    EditorContainer EditorState;

    // 0x0043f9f0
    void EditorConstructor(void)
    {
        ConstructEditor(EditorState._Instance);
        // TODO NOT IMPLEMENTED: FUN_0060f9ef(&DAT_00671740);
    }

    // 0x00440a90
    Editor* ConstructEditor(Editor* self)
    {
        *EditorState.Windows._Count = 0;

        ZeroMemory(WindowState._Windows, sizeof(Window) * MAX_EDITOR_WINDOW_COUNT);

        SelectEditorColors(self);

        return self;
    }

    // 0x00440ac0
    void* ReleaseEditor(Editor* self, const ReleaseMode mode)
    {
        while (0 < *EditorState.Windows._Count) { PopEditorWindow(self); }

        if (ClipboardState.Value != NULL) { ReleaseMemory1(ClipboardState.Value); }

        return self;
    }

    // 0x00444450
    // a.k.a. popWindow
    void PopEditorWindow(Editor* self)
    {
        if (*EditorState.Windows._Count < 1) { LogError("Unable to pop editor window, no open windows."); }

        *EditorState.Windows._Count = *EditorState.Windows._Count - 1;

        if (WindowState._Windows[*EditorState.Windows._Count].AllocatedMemory != NULL)
        {
            ReleaseMemory1(WindowState._Windows[*EditorState.Windows._Count].AllocatedMemory);
            WindowState._Windows[*EditorState.Windows._Count].AllocatedMemory = NULL;
        }

        FUN_004969b0(WindowState._Windows[*EditorState.Windows._Count].Unk009,
            WindowState._Windows[*EditorState.Windows._Count].Unk010,
            WindowState._Windows[*EditorState.Windows._Count].Unk011,
            WindowState._Windows[*EditorState.Windows._Count].Unk012); // TODO

        *EditorState.Windows.Active._Unk005 = WindowState._Windows[*EditorState.Windows._Count].Unk005;
        *EditorState.Windows.Active._Unk006 = WindowState._Windows[*EditorState.Windows._Count].Unk006;
        *EditorState.Windows.Active._Unk007 = WindowState._Windows[*EditorState.Windows._Count].Unk007;
        *EditorState.Windows.Active._Unk008 = WindowState._Windows[*EditorState.Windows._Count].Unk008;

        *EditorState.Windows.Active._Unk013 = WindowState._Windows[*EditorState.Windows._Count].Unk013;
        *EditorState.Windows.Active._Unk014 = WindowState._Windows[*EditorState.Windows._Count].Unk014;
        *EditorState.Windows.Active._Unk015 = WindowState._Windows[*EditorState.Windows._Count].Unk015;
        *EditorState.Windows.Active._Unk016 = WindowState._Windows[*EditorState.Windows._Count].Unk016;
        *EditorState.Windows.Active._Unk017 = WindowState._Windows[*EditorState.Windows._Count].Unk017;
        *EditorState.Windows.Active._Unk018 = WindowState._Windows[*EditorState.Windows._Count].Unk018;
    }

    // 0x00440b20
    void SelectEditorColors(Editor* self)
    {
        EditorState._Colors[0] = 0xff000000;
        EditorState._Colors[1] = 0xffc0c0c0;
        EditorState._Colors[2] = 0xffc0c0c0;
        EditorState._Colors[3] = 0xffffff00;
        EditorState._Colors[4] = 0xff000000;
        EditorState._Colors[5] = 0xffffffff;
        EditorState._Colors[6] = 0xffffffff;
        EditorState._Colors[7] = 0xffffffff;
        EditorState._Colors[8] = 0xff000080;
        EditorState._Colors[9] = 0xffffffff;
        EditorState._Colors[10] = 0xffffffff;
        EditorState._Colors[11] = 0xffffffff;
        EditorState._Colors[12] = 0xffffffff;
        EditorState._Colors[13] = 0xffffffff;
        EditorState._Colors[14] = 0xff00ff00;
        EditorState._Colors[15] = 0xff000000;
        EditorState._Colors[16] = 0xff000080;
        EditorState._Colors[17] = 0xffffffff;
        EditorState._Colors[18] = 0xffffffff;
        EditorState._Colors[19] = 0xffffffff;
        EditorState._Colors[20] = 0xffffffff;
        EditorState._Colors[21] = 0xff808080;
        EditorState._Colors[22] = 0xff008000;
        EditorState._Colors[23] = 0xff00ff00;
        EditorState._Colors[24] = 0xffc0c0c0;
        EditorState._Colors[25] = 0xff000000;
        EditorState._Colors[26] = 0xffc0c0c0;
        EditorState._Colors[27] = 0xffc0c0c0;
        EditorState._Colors[28] = 0xff000000;
        EditorState._Colors[29] = 0xff000000;
        EditorState._Colors[30] = 0xffffffff;
        EditorState._Colors[31] = 0xff808080;
    }
}