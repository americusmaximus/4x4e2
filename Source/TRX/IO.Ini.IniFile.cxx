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

#include "IO.Ini.hxx"
#include "Logger.hxx"
#include "Memory.hxx"
#include "Strings.hxx"

using namespace Logger;
using namespace Memory;
using namespace Objects;
using namespace Strings;

namespace IO::Ini
{
    IniFileContainer IniFileState;

    // 0x0047e2f0
    IniFile* InitializeIniFile(IniFile* self, const char* file, const char* section)
    {
        self->Self = &IniFileState.IniFileSelf;

        SelectAbstractIniFile((AbstractIni*)self, file);
        SelectAbstractIniSection((AbstractIni*)self, section);

        return self;
    }

    // 0x0047e320
    void* ReleaseIniFile(IniFile* self, const ReleaseMode mode)
    {
        if ((mode & ReleaseMode::Unknown4) != ReleaseMode::None)
        {
            ReleaseMemory3(ReleaseObject(self, &IniFileState.Initializer));

            return self;
        }

        self->Self = &IniFileState.IniFileSelf;

        if ((mode & ReleaseMode::Unknown2) != ReleaseMode::None) { ReleaseMemory2(self); }

        return self;
    }

    // 0x0047e3f0
    // a.k.a. getString
    void ReadIniFile(IniFile* self, const char* name, char* value, const u32 size)
    {
        if (IsNullOrEmpty(self->Name)) { LogError("Unable to read a string value from an ini file. Name is not set."); }

        if (IsNullOrEmpty(self->Section)) { LogError("Unable to read a string value from an ini file. Section is not set."); }

        ReadIni(self->Name, self->Section, name, value, size);
    }

    // 0x0047e480
    // a.k.a. setString
    void WriteIniFile(IniFile* self, const char* name, const char* value)
    {
        if (IsNullOrEmpty(self->Name)) { LogError("Unable to write a string value to an ini file. Name is not set."); }

        if (IsNullOrEmpty(self->Section)) { LogError("Unable to write a string value to an ini file. Section is not set."); }

        WriteIni(self->Name, self->Section, name, value);
    }
}