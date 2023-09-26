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

#include "Objects.hxx"

namespace Editor
{
    // a.k.a. CEdButton
    struct TextButton
    {
        s32 Unk1; // TODO
        s32 Unk2; // TODO

        s32 X;
        s32 Y;
        s32 Width;
        s32 Height;

        BOOL IsActive;

        char Text[100]; // TODO

        char Unk9[100]; // TODO
    };

    TextButton* ConstructTextButton(TextButton* self);
    void* ReleaseTextButton(TextButton* self, const Objects::ReleaseMode mode);

    struct TextButtonContainer
    {
        // 0x00664440
        Objects::AbstractObjectInitializer Initializer =
        {
            .Options = 0x20100, // TODO
            .Initialize = (Objects::ABSTRACTOBJECTINITIALIZERINITIALIZE)&ConstructTextButton,
            .Assign = NULL,
            .Release = (Objects::ABSTRACTOBJECTINITIALIZERRELEASE)&ReleaseTextButton,
            .Size = sizeof(TextButton),
            .Name = "$CEdButton$$"
        };

        u32* _Mode = (u32*)0x00813e54; // TODO enumeration
    };

    extern TextButtonContainer TextButtonState;

    void InitializeTextButton(TextButton* self, const s32 x, const s32 y, const s32 width, const s32 height, const char* text);
    BOOL ToggleTextButton(TextButton* self);
    void ClearTextButton(TextButton* self);
}