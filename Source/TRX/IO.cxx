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
#include "ASCII.hxx"

#define MAX_IO_PATH_LENGTH 512

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



    // 0x0043a730
    void AcquireNormalizedDirectoryPath(const char* path, char* disk, char* dir)
    {
        char indir[STANDARD_IO_DIRECTORY_NAME_LENGTH];
        char infile[STANDARD_IO_FILE_NAME_LENGTH];
        char inext[STANDARD_IO_EXTENSION_NAME_LENGTH];

        _splitpath(path, disk, indir, infile, inext);
        _makepath(dir, NULL, indir, infile, inext);

        if (IsNotNullOrEmpty(dir))
        {
            const auto len = strlen(dir);

            if (dir[len - 1] != ASCII_BACK_SLASH && dir[len - 1] != ASCII_SLASH)
            {
                dir[len] = ASCII_BACK_SLASH;
                dir[len + 1] = NULL;
            }
        }
    }

    // 0x0043a8e0
    void AcquireNormalizedFilePath(const char* path, char* name, char* file)
    {
        char indisk[MAX_IO_DISK_NAME_LENGTH];
        char indir[STANDARD_IO_DIRECTORY_NAME_LENGTH];
        char infile[STANDARD_IO_FILE_NAME_LENGTH];
        char inext[STANDARD_IO_EXTENSION_NAME_LENGTH];

        _splitpath(name, indisk, indir, infile, inext);

        if (indisk[0] != NULL
            || ((indir[0] == ASCII_BACK_SLASH || indir[0] == ASCII_SLASH) && (indir[1] == ASCII_BACK_SLASH || indir[1] == ASCII_SLASH)))
        {
            strcpy(file, name);

            return;
        }

        char resdir[MAX_IO_PATH_LENGTH];
        char resdisk[MAX_IO_DISK_NAME_LENGTH];

        AcquireNormalizedDirectoryPath(path, resdisk, resdir);

        if (indir[0] == ASCII_SLASH || indir[0] == ASCII_BACK_SLASH) { resdir[0] = NULL; }

        strcat(resdir, indir);

        _makepath(file, resdisk, resdir, infile, inext);
    }
}