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
#include "Objects.hxx"

namespace Editor
{
    struct ScrollBar
    {
        s32 Unk1; // TODO
        s32 Unk2; // TODO
        s32 Unk3; // TODO
        s32 Unk4; // TODO
        s32 Unk5; // TODO
        s32 Unk6; // TODO
        s32 Unk7; // TODO
        s32 Unk8; // TODO
        s32 Unk9; // TODO
        s32 Unk10;// TODO
        s32 Unk11;// TODO
        s32 Unk12;// TODO
        s32 Unk13;// TODO
        s32 Unk14;// TODO
    };

    ScrollBar* ConstructScrollBar(ScrollBar* self);
    void* ReleaseScrollBar(ScrollBar* self, const Objects::ReleaseMode mode);
    void ClearScrollBar(ScrollBar* self);
    void ComputeScrollBar(ScrollBar* self);
}