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

#define MAX_IO_STREAM_POOL_FILE_COUNT 5

namespace IO::Streams
{
    struct PoolFile
    {
        HANDLE Handle;
        char Name[MAX_IO_FILE_NAME_LENGTH];
    };

    struct PoolContainer
    {
        // NOTE: Contains the list of recently "closed" files,
        // that are still kept open, in case there is a need to reopen same file.
        PoolFile Files[MAX_IO_STREAM_POOL_FILE_COUNT]; // 0x00d5ebf0
    };

    extern PoolContainer PoolState;

    void ReleaseFilePool(void);
}