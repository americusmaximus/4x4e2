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

#include "Editor.TextButton.hxx"
#include "Editor.hxx"
#include "Graphics.hxx"

#include <string.h>

using namespace Graphics;
using namespace Objects;

namespace Editor
{
    TextButtonContainer TextButtonState;

    // 0x00449630
    TextButton* ConstructTextButton(TextButton* self)
    {
        self->Unk1 = 0; // TODO
        self->Unk2 = 0; // TODO

        self->Text[0] = NULL;

        self->IsActive = FALSE;

        self->X = 0;
        self->Y = 0;
        self->Width = 0;
        self->Height = 0;

        return self;
    }

    // 0x00449660
    void* ReleaseTextButton(TextButton* self, const ReleaseMode mode)
    {
        ClearTextButton(self);

        return self;
    }

    // 0x00449680
    void ClearTextButton(TextButton* self)
    {
        if (*EditorState.Controls._Active == self->Unk1) { *EditorState.Controls._Active = 0; } // TODO constant
    }

    // 0x004496e0
    void InitializeTextButton(TextButton* self, const s32 x, const s32 y, const s32 width, const s32 height, const char* text)
    {
        strcpy(self->Text, text);

        self->X = x;
        self->Y = y;
        self->Width = width + 1;
        self->Height = height + 1;
    }

    // 0x00449990
    BOOL ToggleTextButton(TextButton* self)
    {
        const auto active = self->Unk1 == 0 && self->X <= *GraphicsState.View._Width && self->Y <= *GraphicsState.View._Height
            && *GraphicsState.View._Width < self->Width && *GraphicsState.View._Height < self->Height;

        if (*EditorState.Controls._Active == self->Unk1)
        {
            self->IsActive = active;

            if ((*TextButtonState._Mode & 1) != 0)
            {
                *EditorState.Controls._Active = *EditorState.Controls._Active ^ self->Unk1;

                if (active)
                {
                    self->IsActive = FALSE;

                    return TRUE;
                }
            }

            return FALSE;
        }

        if (active && *EditorState.Controls._Active == 0 && (*TextButtonState._Mode & 1) != 0) // TODO constant
        {
            self->IsActive = TRUE;
            *EditorState.Controls._Active = self->Unk1;
        }

        return FALSE;
    }
}