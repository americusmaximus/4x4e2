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

#include "ASCII.hxx"
#include "IO.FileFinder.hxx"
#include "Logger.hxx"
#include "Strings.hxx"

#include <stdlib.h>

#define MAX_FILE_FINDER_DELIMITER_LENGTH 4

using namespace Logger;
using namespace Objects;
using namespace Strings;

namespace IO
{
    FileFinderContainer FileFinderState;

    static time_t filetime_to_timet(FILETIME const& ft)
    {
        ULARGE_INTEGER ull;

        ull.LowPart = ft.dwLowDateTime;
        ull.HighPart = ft.dwHighDateTime;

        return (time_t)(ull.QuadPart / 10000000ULL - 11644473600ULL);
    }

    // 0x0043a3c0
    FileFinder* ConstructFileFinder(FileFinder* self)
    {
        self->Search.Count = INVALID_FILE_FINDER_COUNT;
        self->Search.IsActive = FALSE;
        self->File.Name[0] = NULL;

        return self;
    }

    // 0x0043a3f0
    void* ReleaseFileFinder(FileFinder* self, const ReleaseMode mode)
    {
        CloseFileFinder(self);

        return self;
    }

    // 0x0043a590
    void CloseFileFinder(FileFinder* self)
    {
        CloseFileFinderHandle(self);

        self->File.Name[0] = NULL;
        self->Search.Count = INVALID_FILE_FINDER_COUNT;
    }

    // 0x0043a5c0
    void CloseFileFinderHandle(FileFinder* self)
    {
        if (!self->Search.IsActive) { return; }

        FindClose(self->Search.Handle);

        self->Search.IsActive = FALSE;
    }

    // 0x0043a500
    // a.k.a. open
    BOOL ExecuteFileFinder(FileFinder* self)
    {
        if (self->Search.Count < 0 || !self->Search.IsActive) { LogError("File finder search is inactive."); }

        WIN32_FIND_DATAA data;

        if (!FindNextFileA(self->Search.Handle, &data))
        {
            SelectFileFinderState(self, &data);

            return TRUE;
        }

        return SearchFileFinder(self);
    }

    // 0x0043a410
    BOOL OpenFileFinder(FileFinder* self, const char* pattern)
    {
        CloseFileFinder(self);

        strcpy(self->Search.Pattern, pattern);

        self->Search.Count = INVALID_FILE_FINDER_COUNT;

        return SearchFileFinder(self);
    }

    // 0x0043a460
    BOOL SearchFileFinder(FileFinder* self)
    {
        WIN32_FIND_DATAA data;

        while (TRUE)
        {
            self->Search.Count = self->Search.Count + 1;

            if (!AcquireFileFinderFile(self->Search.Pattern, self->Search.Count, self->Search.Name))
            {
                CloseFileFinder(self);

                return FALSE;
            }

            CloseFileFinder(self);

            auto handle = FindFirstFileA(self->Search.Name, &data);

            if (handle != INVALID_HANDLE_VALUE)
            {
                self->Search.Handle = handle;

                SelectFileFinderState(self, &data);

                self->Search.IsActive = TRUE;

                return TRUE;
            }

            if (self->Search.IsActive) { return TRUE; }
        }

        return FALSE;
    }

    // 0x0043afd0
    BOOL AcquireFileFinderFile(const char* pattern, const u32 indx, char* file)
    {
        auto index = indx;
        char* ptr = (char*)&pattern[0];

        char current = ASCII_SEMICOLON;

        char disk[MAX_IO_DISK_NAME_LENGTH];
        char dir[STANDARD_IO_DIRECTORY_NAME_LENGTH];
        char name[MAX_IO_FILE_NAME_LENGTH];

        char path[MAX_IO_FILE_NAME_LENGTH];

        char delimiter[MAX_FILE_FINDER_DELIMITER_LENGTH];

        while (ptr != NULL)
        {
            if (current == ASCII_SEMICOLON) { path[0] = NULL; }

            ptr = AcquireFileFinderFileDetails(ptr, name, delimiter);

            if (name[0] == NULL)
            {
                if (index == 0)
                {
                    AcquireNormalizedFilePath(path, name, file);

                    return TRUE;
                }

                index = index + 1;

                if (current == ASCII_SEMICOLON)
                {
                    _splitpath(name, disk, dir, NULL, NULL);
                    _makepath(path, disk, dir, NULL, NULL);
                }
            }

            current = delimiter[0];
        }

        file[0] = NULL;

        return FALSE;
    }

    // 0x0043af00
    char* AcquireFileFinderFileDetails(const char* pattern, char* name, char* delimiter)
    {
        name[0] = NULL;

        char* ptr = (char*)&pattern[0];

        if (ptr[0] == NULL) { return ptr; }

        // Skip all the leading "space" characters.
        while (isspace(ptr[0])) { ptr = &ptr[1]; }

        u32 indx = 0;
        while (ptr[0] != NULL && ptr[0] != ASCII_SEMICOLON && ptr[0] != ASCII_COMMA)
        {
            name[indx] = ptr[0];

            indx = indx + 1;

            ptr = &ptr[1];
        }

        // Skip all the trailing "space" characters.
        while (0 < indx && isspace(name[indx - 1])) { indx = indx - 1; }

        name[indx] = NULL;

        if (ptr[0] != NULL)
        {
            if (delimiter != NULL) { delimiter[0] = ptr[0]; }

            return &ptr[1];
        }

        return NULL;
    }

    // 0x0043a600
    // a.k.a. convertStruct
    void SelectFileFinderState(FileFinder* self, WIN32_FIND_DATAA* data)
    {
        ZeroMemory(&self->File.Name, STANDARD_IO_FILE_NAME_LENGTH);

        strncpy(self->File.Name, data->cFileName, (STANDARD_IO_FILE_NAME_LENGTH - 1));

        self->File.Options = FileFinderFileOptions::None;

        if (data->dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE)
        {
            self->File.Options = self->File.Options | FileFinderFileOptions::Archive;
        }

        if (data->dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)
        {
            self->File.Options = self->File.Options | FileFinderFileOptions::Hidden;
        }

        if (data->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            self->File.Options = self->File.Options | FileFinderFileOptions::Directory;
        }

        if (data->dwFileAttributes & FILE_ATTRIBUTE_READONLY)
        {
            self->File.Options = self->File.Options | FileFinderFileOptions::ReadOnly;
        }

        if (data->nFileSizeHigh != 0 || 0x7fffffff < data->nFileSizeLow)
        {
            LogError("File %s is to big for the game to handle.", data->cFileName);
        }

        self->File.Size = data->nFileSizeLow;
        self->File.DateTime = filetime_to_timet(data->ftLastWriteTime);
    }
}