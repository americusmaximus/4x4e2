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

#define MAX_IO_FILE_NAME_LENGTH 260

#define STANDARD_IO_DISK_NAME_LENGTH 3
#define STANDARD_IO_FILE_NAME_LENGTH 256
#define STANDARD_IO_DIRECTORY_NAME_LENGTH 256
#define STANDARD_IO_EXTENSION_NAME_LENGTH 256

namespace IO
{
    struct FileDescriptor
    {
        char Name[STANDARD_IO_FILE_NAME_LENGTH];
        char Path[STANDARD_IO_FILE_NAME_LENGTH];

        s32 Offset;
        s32 Size;

        s32 Unk003; // TODO
        s32 Unk004; // TODO
        s32 Unk005; // TODO
    };

    void InitializeFileDescriptor(FileDescriptor* desc, const char* dir, const char* file);
    BOOL HandleFileDescriptor(FileDescriptor* desc);
    BOOL AcquireFileDescriptorDetails(FileDescriptor* self);
    BOOL AcquireAbsoluteFilePath(char* buffer, const char* file);
}