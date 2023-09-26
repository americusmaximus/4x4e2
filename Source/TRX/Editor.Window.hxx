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

#define MAX_EDITOR_WINDOW_COUNT 5

namespace Editor
{
    struct Window
    {
        s32 Unk001; // TODO
        s32 Unk002; // TODO
        s32 Unk003; // TODO
        s32 Unk004; // TODO

        s32 Unk005; // TODO
        s32 Unk006; // TODO
        s32 Unk007; // TODO
        s32 Unk008; // TODO

        f32 Unk009; // TODO
        f32 Unk010; // TODO
        f32 Unk011; // TODO
        f32 Unk012; // TODO

        s32 Unk013; // TODO
        s32 Unk014; // TODO
        s32 Unk015; // TODO
        s32 Unk016; // TODO
        s32 Unk017; // TODO
        s32 Unk018; // TODO
        s32 Unk019; // TODO
        s32 Unk020; // TODO
        s32 Unk021; // TODO

        char Unknown1[388]; // TODO

        void* AllocatedMemory; // TODO

        char Unknown2[32]; // TODO
    };

    struct WindowContainer
    {
        Window* _Windows = (Window*)0x00813390; // TODO an array of [MAX_EDITOR_WINDOW_COUNT]
    };

    extern WindowContainer WindowState;
}