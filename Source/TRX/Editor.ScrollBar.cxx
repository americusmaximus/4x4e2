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

#include "Editor.hxx"
#include "Editor.ScrollBar.hxx"
#include "Logger.hxx"

using namespace Logger;
using namespace Objects;

namespace Editor
{
    // 0x00448c10
    ScrollBar* ConstructScrollBar(ScrollBar* self)
    {
        self->Unk1 = 0;
        self->Unk2 = 1; // TODO constant
        self->Unk3 = 1; // TODO constant
        self->Unk4 = 1; // TODO constant
        self->Unk5 = 0;
        self->Unk6 = 0;
        self->Unk7 = 0;
        self->Unk8 = 0;
        self->Unk9 = 0;

        return self;
    }

    // 0x00448c50
    void* ReleaseScrollBar(ScrollBar* self, const ReleaseMode mode)
    {
        ClearScrollBar(self);

        return self;
    }

    // 0x00448c70
    void ClearScrollBar(ScrollBar* self)
    {
        if (*EditorState.Controls._Active == self->Unk1) { *EditorState.Controls._Active = 0; } // TODO constant
    }

    // 0x00448ff0
    // a.k.a. 
    void ComputeScrollBar(ScrollBar* self)
    {
        if (self->Unk2 <= self->Unk3 || self->Unk2 < 1)
        {
            self->Unk12 = self->Unk10;
            self->Unk13 = self->Unk11;

            return;
        }

        auto unk1 = self->Unk11 - self->Unk10; // TODO name

        if (unk1 < 1) { LogError("Invalid scroll bar dimensions."); }

        auto unk2 = unk1 * self->Unk3 / self->Unk2; // TODO name

        auto unk3 = self->Unk5 == 0 ? self->Unk8 - self->Unk6 : self->Unk9 - self->Unk7; // TODO name

        if (unk2 < unk3) { unk2 = unk3; }
        if (unk1 < unk2) { unk2 = unk1; }

        self->Unk12 = self->Unk10 + ((unk1 - unk2) * self->Unk1) / (self->Unk2 - self->Unk3);

        if (self->Unk12 < self->Unk10) { self->Unk12 = self->Unk10; }

        self->Unk13 = self->Unk12 + unk2;

        if (self->Unk11 < self->Unk13)
        {
            const auto unk4 = self->Unk13; // TODO name

            self->Unk13 = self->Unk11;
            self->Unk12 = self->Unk12 - (unk4 - self->Unk11);
        }

        if (self->Unk10 <= self->Unk12) { return; }

        self->Unk12 = self->Unk10;
    }
}