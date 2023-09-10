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
#include "Native.Basic.hxx"

namespace Time
{
    struct TimeContainer
    {
        struct
        {
            BOOL* _IsActive = (BOOL*)0x00d6921c; // TODO
        } Timer;

        struct
        {
            struct
            {
                LARGE_INTEGER Frequency; // 0x00d69200

                f64 Divisor; // 0x00d69208
                f64 Quotient; // 0x00d69210
            } Performance;

            LARGE_INTEGER Current; // 0x00d691f8

            struct
            {
                DWORD LowPart; // 0x00d691f0
                LONG HighPart; // 0x00d691f4
            } Previous;

            struct
            {
                DWORD LowPart; // 0x00d691e0
                LONG HighPart; // 0x00d691e4

                s32 Value; // 0x00d691e8
            } Delta;

            s32 Time; // 0x00d69218
        } Counter;
    };

    extern TimeContainer TimeState;

    u32 AcquireTime(void);
    void InitializeTime(void);
    void ReleaseTime(void);
}