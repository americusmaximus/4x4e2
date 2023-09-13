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

#include "IO.hxx"

#define MAX_FILE_HANDLER_COUNT 5
#define MAX_FILE_HANDLER_INDEX (MAX_FILE_HANDLER_COUNT - 1)

namespace IO
{
    typedef const BOOL(CDECLAPI* IOFILEHANDLER) (FileDescriptor* desc);

    struct HandlerContainer
    {
        u32* _Count = (u32*)0x00811730; // TODO
        IOFILEHANDLER* _Handlers = (IOFILEHANDLER*)0x00811734; // TODO
    };

    extern HandlerContainer HandlerState;

    void RegisterHandler(IOFILEHANDLER lambda);
}