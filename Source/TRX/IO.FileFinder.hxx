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
#include "Objects.hxx"
#include "Native.Basic.hxx"

#define INVALID_FILE_FINDER_COUNT (-1)
#define MAX_FILE_FINDER_SEARCH_PATTERN_FILE_COUNT 4

namespace IO
{
    OPTIONS(FileFinderFileOptions, u32)
    {
        None = 0,
        Archive = 1,
        Hidden = 2,
        Directory = 4,
        ReadOnly = 8
    };

    // a.k.a. CFileFinder
    struct FileFinder
    {
        struct
        {
            char Name[STANDARD_IO_FILE_NAME_LENGTH];
            FileFinderFileOptions Options;
            u32 Size;
            u32 DateTime;
        } File;

        struct
        {
            char Pattern[MAX_FILE_FINDER_SEARCH_PATTERN_FILE_COUNT * MAX_FILE_FINDER_SEARCH_PATTERN_FILE_COUNT];
            char Name[MAX_IO_FILE_NAME_LENGTH];
            int Count;
            BOOL IsActive;
            HANDLE Handle;
        } Search;
    };

    FileFinder* ConstructFileFinder(FileFinder* self);
    void* ReleaseFileFinder(FileFinder* self, const Objects::ReleaseMode mode);

    struct FileFinderContainer
    {
        // 0x006642b0
        Objects::AbstractObjectInitializer Initializer =
        {
            .Options = 0x20100, // TODO
            .Initialize = (Objects::ABSTRACTOBJECTINITIALIZERINITIALIZE)&ConstructFileFinder,
            .Assign = NULL,
            .Release = (Objects::ABSTRACTOBJECTINITIALIZERRELEASE)&ReleaseFileFinder,
            .Size = sizeof(FileFinder),
            .Name = "$CFileFinder$$"
        };
    };

    extern FileFinderContainer FileFinderState;

    void CloseFileFinder(FileFinder* self);
    void CloseFileFinderHandle(FileFinder* self);

    BOOL AcquireFileFinderFile(const char* pattern, const u32 indx, char* file);
    BOOL ExecuteFileFinder(FileFinder* self);
    BOOL OpenFileFinder(FileFinder* self, const char* pattern);
    BOOL SearchFileFinder(FileFinder* self);
    char* AcquireFileFinderFileDetails(const char* pattern, char* name, char* delimiter);
    void SelectFileFinderState(FileFinder* self, WIN32_FIND_DATAA* data);
}