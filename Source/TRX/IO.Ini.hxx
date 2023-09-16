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

#define MAX_INI_SECTION_LENGTH 256
#define MAX_INI_STRING_BUFFER_LENGTH 256

namespace IO::Ini
{
    struct Ini;

    Ini* ConstructIni(Ini* self);
    void* ReleaseIni(Ini* self);

    typedef const void* (CDECLAPI* INIRELEASE) (Ini* self, const Objects::ReleaseMode mode);
    typedef const void(CDECLAPI* INIREAD) (Ini* self, const char* name, char* value, const u32 size);
    typedef const void(CDECLAPI* INIWRITE) (Ini* self, const char* name, const char* value);

    struct IniSelf
    {
        INIRELEASE Release;
        INIREAD Read;
        INIWRITE Write;
    };

    // a.k.a. cIni
    struct Ini
    {
        IniSelf* Self;
    };

    struct IniContainer
    {
        // 0x006648f0
        Objects::AbstractObjectInitializer Initializer =
        {
            .Options = 0x20100, // TODO
            .Initialize = (Objects::ABSTRACTOBJECTINITIALIZERINITIALIZE)&ConstructIni,
            .Assign = NULL,
            .Release = (Objects::ABSTRACTOBJECTINITIALIZERRELEASE)&ReleaseIni,
            .Size = sizeof(Ini),
            .Name = "$cIni$$"
        };
    };

    extern IniContainer IniState;

    u32 AcquireIniState(const char* file, const char* section, const char* name, const char* value, BOOL* obsolete);
    BOOL ReadIni(const char* file, const char* section, const char* name, char* value, const u32 size);
    BOOL WriteIni(const char* file, const char* section, const char* name, const char* value);

    struct AbstractIni;

    typedef const void* (CDECLAPI* ABSTRACTINIRELEASE) (AbstractIni* self, const Objects::ReleaseMode mode);
    typedef const void(CDECLAPI* ABSTRACTINIREAD) (AbstractIni* self, const char* name, char* value, const u32 size);
    typedef const void(CDECLAPI* ABSTRACTINIWRITE) (AbstractIni* self, const char* name, const char* value);

    // INHERITANCE: IniSelf
    struct AbstractIniSelf
    {
        ABSTRACTINIRELEASE Release;
        ABSTRACTINIREAD Read;
        ABSTRACTINIWRITE Write;
    };

    struct AbstractIni
    {
        char Name[STANDARD_IO_FILE_NAME_LENGTH];
        char Section[MAX_INI_SECTION_LENGTH];
        AbstractIniSelf* Self;
    };

    void SelectAbstractIniFile(AbstractIni* self, const char* file);
    void SelectAbstractIniSection(AbstractIni* self, const char* section);

    struct IniFile;

    void* ReleaseIniFile(IniFile* self, const Objects::ReleaseMode mode);
    void ReadIniFile(IniFile* self, const char* name, char* value, const u32 size);
    void WriteIniFile(IniFile* self, const char* name, const char* value);

    // INHERITANCE: AbstractIniSelf
    struct IniFileSelf
    {
        ABSTRACTINIRELEASE Release = (ABSTRACTINIRELEASE)&ReleaseIniFile; // 0x0047e320
        ABSTRACTINIREAD Read = (ABSTRACTINIREAD)&ReadIniFile; // 0x0047e3f0
        ABSTRACTINIWRITE Write = (ABSTRACTINIWRITE)&WriteIniFile; // 0x0047e480
    };

    // a.k.a. CIniFile
    // INHERITANCE: AbstractIni
    struct IniFile
    {
        char Name[STANDARD_IO_FILE_NAME_LENGTH];
        char Section[MAX_INI_SECTION_LENGTH];
        IniFileSelf* Self;
    };

    struct IniFileContainer
    {
        IniFileSelf IniFileSelf; // 0x006648e4

        // 0x00664910
        Objects::AbstractObjectInitializer Initializer =
        {
            .Options = 0x20100, // TODO
            .Initialize = NULL,
            .Assign = NULL,
            .Release = (Objects::ABSTRACTOBJECTINITIALIZERRELEASE)&ReleaseIniFile,
            .Size = sizeof(IniFile),
            .Name = "$CIniFile$$"
        };
    };

    extern IniFileContainer IniFileState;

    IniFile* InitializeIniFile(IniFile* self, const char* file, const char* section);

    struct IniMemory;

    IniMemory* ConstructIniMemory(IniMemory* self);

    void* ReleaseIniMemory(IniMemory* self, const Objects::ReleaseMode mode);
    void ReadIniMemory(IniMemory* self, const char* name, char* value, const u32 size);
    void WriteIniMemory(IniMemory* self, const char* name, const char* value);

    // INHERITANCE: AbstractIniSelf
    struct IniMemorySelf
    {
        ABSTRACTINIRELEASE Release = (ABSTRACTINIRELEASE)&ReleaseIniMemory; // 0x00490de0
        ABSTRACTINIREAD Read = (ABSTRACTINIREAD)&ReadIniMemory; // 0x00490e60
        ABSTRACTINIWRITE Write = (ABSTRACTINIWRITE)&WriteIniMemory; // 0x00490fb0
    };

    // a.k.a. CMemoryIni
    // INHERITANCE: AbstractIni
    struct IniMemory
    {
        char Name[STANDARD_IO_FILE_NAME_LENGTH];
        char Section[MAX_INI_SECTION_LENGTH];
        IniMemorySelf* Self;
        void* Buffer;
    };

    struct IniMemoryContainer
    {
        IniMemorySelf IniMemorySelf; // 0x00664e84

        // 0x00664e90
        Objects::AbstractObjectInitializer Initializer =
        {
            .Options = 0x20100, // TODO
            .Initialize = (Objects::ABSTRACTOBJECTINITIALIZERINITIALIZE)&ConstructIniMemory,
            .Assign = NULL,
            .Release = (Objects::ABSTRACTOBJECTINITIALIZERRELEASE)&ReleaseIniMemory,
            .Size = sizeof(IniMemory),
            .Name = "$CMemoryIni$$"
        };
    };

    extern IniMemoryContainer IniMemoryState;

    char* AcquireIniMemoryString(char* input, char* output);
}