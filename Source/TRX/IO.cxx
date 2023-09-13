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

#include "IO.hxx"
#include "IO.Handlers.hxx"
#include "Strings.hxx"

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>

using namespace Strings;

namespace IO
{
    // 0x00439f10
    void InitializeFileDescriptor(FileDescriptor* self, const char* dir, const char* file)
    {
        if (IsNotNullOrEmpty(dir))
        {
            sprintf(self->Name, "%s\\%s", dir, file);

            return;
        }

        strcpy(self->Name, file);
    }

    // 0x0043a010
    BOOL HandleFileDescriptor(FileDescriptor* self)
    {
        if (*HandlerState._Count < 1) { return AcquireFileDescriptorDetails(self); }

        for (u32 x = 0; x < *HandlerState._Count; x++)
        {
            if ((*HandlerState._Handlers[x])(self)) { return TRUE; }
        }

        return FALSE;
    }

    // 0x0043a070
    BOOL AcquireFileDescriptorDetails(FileDescriptor* self)
    {
        if (IsNullOrEmpty(self->Name)) { return FALSE; }

        char path[STANDARD_IO_FILE_NAME_LENGTH];
        if (!AcquireAbsoluteFilePath(path, self->Name)) { return FALSE; }

        struct _stat32 stat;
        if (_stat32(path, &stat) == 0)
        {
            self->Size = stat.st_size;
            self->Offset = 0;

            self->Unk003 = 0;
            self->Unk004 = stat.st_mtime;
            self->Unk005 = stat.st_mtime;

            strcpy(self->Path, path);

            return TRUE;
        }

        return FALSE;
    }

    // 0x0043a890
    BOOL AcquireAbsoluteFilePath(char* buffer, const char* file)
    {
        if (buffer == _fullpath(buffer, file, STANDARD_IO_FILE_NAME_LENGTH)) { return TRUE; }

        strcpy(buffer, file);

        return FALSE;
    }
}