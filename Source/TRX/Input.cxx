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

#include "App.hxx"
#include "ASCII.hxx"
#include "Graphics.hxx"
#include "Input.hxx"

using namespace App;
using namespace Graphics;

namespace Input
{
    InputContainer InputState;

    // 0x0060b0b0
    u32 ResetInput(void)
    {
        *InputState._Unknown1 = 0; // TODO constant
        *InputState._Unknown2 = 0; // TODO constant

        return 0; // TODO constant
    }

    // 0x0060b140
    BOOL AreUnknownsNotEqual(void)
    {
        return *InputState._Unknown2 != *InputState._Unknown1;
    }

    // 0x0060b170
    void SelectUnknownArray1Value(const u32 value)
    {
        *InputState._Unknown1 = *InputState._Unknown1 + 1;

        InputState._UnknownArray1[*InputState._Unknown1] = value;

        if (MAX_UNKNOWN_ARRAY1_COUNT < *InputState._Unknown1) { *InputState._Unknown1 = 0; }

        if (*InputState._Unknown1 == *InputState._Unknown2)
        {
            *InputState._Unknown2 = *InputState._Unknown2 + 1;

            if (MAX_UNKNOWN_ARRAY1_COUNT < *InputState._Unknown2) { *InputState._Unknown2 = 0; }
        }
    }

    // 0x0060b0d0
    char AcquireUnknownArray1Value(void)
    {
        while (*InputState._Unknown2 == *InputState._Unknown1)
        {
            if (*AppState._IsQuit) { break; }

            FUN_006029d0(); // TODO
        }

        char result = ASCII_NULL;

        if (*InputState._Unknown2 != *InputState._Unknown1)
        {
            result = InputState._UnknownArray1[*InputState._Unknown2];

            *InputState._Unknown2 = *InputState._Unknown2 + 1;

            if (MAX_UNKNOWN_ARRAY1_COUNT < *InputState._Unknown2)
            {
                *InputState._Unknown2 = 0;
            }
        }

        return result;
    }
}