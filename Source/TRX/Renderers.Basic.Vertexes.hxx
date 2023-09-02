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
#include "Graphics.Basic.hxx"

namespace Renderers
{
    // a.k.a. SVertex
    struct RVX
    {
        f32x3 XYZ;
        f32x3 Normal;
        f32x2 UV;
    };

    // a.k.a. BasisVertex
    struct RBVX
    {
        u32 Unk[6];// TODO
    };

    struct RCVX
    {
        f32 Unk01;// TODO
        f32 Unk02;// TODO
        f32 Unk03;// TODO
        f32 Unk04;// TODO
        f32 Unk05;// TODO
        f32 Unk06;// TODO
        f32 Unk07;// TODO
        f32 Unk08;// TODO
        f32 Unk09;// TODO
        f32 Unk10;// TODO
        f32 Unk11;// TODO
        f32 Unk12;// TODO
        f32 Unk13;// TODO
        f32 Unk14;// TODO
        f32 Unk15;// TODO
        f32 Unk16;// TODO
    };

    // D3DLVERTEX a.k.a. SDX8LVertex
    struct RLVX
    {
        f32x3 XYZ;
        u32 Color;
        f32x2 UV;
    };

    // D3DTLVERTEX a.k.a. SD3DTLVertex
    struct RTLVX
    {
        f32x3 XYZ;
        f32 RHW;
        u32 Color;
        u32 Specular;
        f32x2 UV;
    };

    struct RPVX
    {
        s32 Unk01;// TODO
        s32 Unk02;// TODO
        s32 Unk03;// TODO
        s32 Unk04;// TODO
        u32 Unk05;// TODO
        u32 Unk06;// TODO
        s32 Unk07;// TODO
        s32 Unk08;// TODO
        u32 Unk09;// TODO
        s32 Unk10;// TODO
        s32 Unk11;// TODO
        u32 Unk12;// TODO
    };
}