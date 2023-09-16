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
#include "IO.Ini.hxx"
#include "Objects.hxx"

#include <stdio.h>
#include "Strings.hxx"
#include "Logger.hxx"
using namespace Logger;

#define MAX_INI_MEMORY_STRING_BUFFER_LENGTH 512
#define INVALID_INI_MEMORY_STRING_VALUE_COUNT (-1)

using namespace Memory;
using namespace Objects;
using namespace Strings;

namespace IO::Ini
{
    IniMemoryContainer IniMemoryState;

    // 0x00490db0
    IniMemory* ConstructIniMemory(IniMemory* self)
    {
        self = (IniMemory*)InitializeIniFile((IniFile*)self, NULL, NULL);
        self->Self = &IniMemoryState.IniMemorySelf;
        self->Buffer = NULL;

        return self;
    }

    // 0x00490de0
    void* ReleaseIniMemory(IniMemory* self, const ReleaseMode mode)
    {
        if ((mode & ReleaseMode::Unknown4) != ReleaseMode::None)
        {
            ReleaseMemory3(ReleaseObject(self, &IniMemoryState.Initializer));

            return self;
        }

        self->Self = &IniMemoryState.IniMemorySelf;

        if (self->Buffer != NULL)
        {
            ReleaseMemory1(self->Buffer);
            self->Buffer = NULL;
        }

        auto abs = ReleaseIniFile((IniFile*)self, ReleaseMode::Unknown1);

        if ((mode & ReleaseMode::Unknown2) != ReleaseMode::None) { ReleaseMemory2(self); }

        return abs;
    }

    // 0x00490e60
    void ReadIniMemory(IniMemory* self, const char* name, char* value, const u32 size)
    {
        if (self->Buffer == NULL) { return; }

        BOOL found = FALSE;

        // Buffer is a moving pointer.
        auto buffer = (char*)self->Buffer;

        // Search for the section header.
        {
            char section[MAX_INI_SECTION_LENGTH];
            char result[MAX_INI_MEMORY_STRING_BUFFER_LENGTH];

            sprintf(section, "[%s]", self->Section);
            while (buffer[0] != NULL)
            {
                buffer = AcquireIniMemoryString(buffer, result);

                if (EqualStrings(section, result)) { found = TRUE; break; }
            }
        }

        if (!found || buffer[0] == NULL) { return; }

        // If section was found, and there is more data in the buffer,
        // then search for the property name and the value.
        {
            char line[MAX_INI_MEMORY_STRING_BUFFER_LENGTH];
            char property[MAX_INI_MEMORY_STRING_BUFFER_LENGTH];

            while (buffer[0] != NULL)
            {
                buffer = AcquireIniMemoryString(buffer, line);

                if (line[0] == ASCII_SQUARE_BRACKET_OPEN) { break; }

                auto count = INVALID_INI_MEMORY_STRING_VALUE_COUNT;
                sscanf(line, "%[^ =] =%n %n", property, &count, &count);

                // Copy the value only, if the property is found.
                if (0 < count && EqualStrings(name, property))
                {
                    strcpy(value, &line[count]);

                    break;
                }
            }
        }
    }

    // 0x00490fb0
    // a.k.a. setString
    void WriteIniMemory(IniMemory* self, const char* name, const char* value)
    {
        // Buffer is a moving pointer.
        char* buffer = (char*)self->Buffer;

        // Search the section header.
        if (buffer != NULL)
        {
            BOOL found = FALSE;
            char section[MAX_INI_MEMORY_STRING_BUFFER_LENGTH];
            sprintf(section, "[%s]", self->Section);

            char line[MAX_INI_MEMORY_STRING_BUFFER_LENGTH];

            while (buffer[0] != NULL)
            {
                buffer = AcquireIniMemoryString(buffer, line);

                if (EqualStrings(section, line)) { found = TRUE; break; }
            }

            if (!found) { buffer = NULL; }
        }

        char kv[MAX_INI_MEMORY_STRING_BUFFER_LENGTH];
        sprintf(kv, "%s=%s\n", name, value);

        // This is the case when the section header was found.
        if (buffer != NULL)
        {
            // Since the section header was found there is a need to locate the property, if it exists.
            // If it does not exists then there is a need to create one.

            u32 lo = 0; // The property line offset within the buffer.
            char line[MAX_INI_MEMORY_STRING_BUFFER_LENGTH];
            char property[MAX_INI_MEMORY_STRING_BUFFER_LENGTH];

            const auto kvl = strlen(kv);

            // Search for the property.
            while (TRUE)
            {
                // Buffer end reached, or new section header found.
                if (buffer[0] == NULL || buffer[0] == ASCII_SQUARE_BRACKET_OPEN)
                {
                    // There is a need to insert the property into the current section.
                    const auto offset = (addr)buffer - (addr)self->Buffer;
                    const auto length = strlen((char*)self->Buffer);

                    self->Buffer = ReallocateMemory(self->Buffer, length + kvl + 1);

                    if (self->Buffer == NULL) { LogError("Unable to allocate memory."); }

                    memmove(&((char*)self->Buffer)[offset + kvl], &((char*)self->Buffer)[offset], length - offset + 1);
                    memcpy(&((char*)self->Buffer)[offset], kv, kvl - 1);

                    return;
                }

                // Search for the property.
                lo = (addr)buffer - (addr)self->Buffer;
                buffer = AcquireIniMemoryString(buffer, line);

                if (sscanf(line, "%[^ =]", property) == 1 && EqualStrings(name, property)) { break; }
            }

            // The property is found within the section.

            {
                const auto length = strlen((char*)self->Buffer);

                // The offset between the end of property key-value line and the beginning of the buffer.
                const auto offset = (addr)buffer - (addr)self->Buffer;
                // The length of the key-value line.
                const auto kvll = offset - lo;

                // Check if the length of the new key-value line does not match the old key-value line length.
                if (kvll != kvl)
                {
                    if (kvl < kvll)
                    {
                        memmove(&((char*)self->Buffer)[kvl + lo], &((char*)self->Buffer)[kvll + lo], length - offset + 1);
                    }

                    self->Buffer = ReallocateMemory(self->Buffer, length + kvl - kvll + 1);

                    if (self->Buffer == NULL) { LogError("Unable to allocate memory."); }

                    if (kvll < kvl)
                    {
                        memmove(&((char*)self->Buffer)[kvl + lo], &((char*)self->Buffer)[kvll + lo], length - offset + 1);
                    }
                }

                // In case the length match, or the proper space was created for the new values above,
                // then update the buffer line with the new value.

                memcpy(&((char*)self->Buffer)[lo], kv, kvl);
            }

            return;
        }

        // This is the case when the section header was not found,
        // therefore there is a need to append the section header and the property value.

        {
            char content[MAX_INI_MEMORY_STRING_BUFFER_LENGTH];
            sprintf(content, "[%s]\n%s", self->Section, kv);

            const auto length = strlen((char*)self->Buffer);

            self->Buffer = ReallocateMemory(self->Buffer, length + strlen(content));

            if (self->Buffer == NULL) { LogError("Unable to allocate memory."); }

            strcpy(&((char*)self->Buffer)[length], content);
        }
    }

    // 0x00490c80
    // Gets one line of text from the buffer, trimming "spaces" on the both sides.
    char* AcquireIniMemoryString(char* input, char* output)
    {
        char* result = input;

        // Skip all "space" characters at the beginning.
        while (isspace(result[0])) { result = &result[1]; }

        u32 count = 0;
        char* value = result;

        while (value[0] != NULL)
        {
            value = result;

            if (value[0] == ASCII_CARRIAGE_RETURN || value[0] == ASCII_NEW_LINE) { break; }

            if (count < (MAX_INI_MEMORY_STRING_BUFFER_LENGTH - 1))
            {
                // Replace any "space" character with actual space, otherwise emplace real character.
                output[count] = isspace(value[0]) ? ASCII_SPACE : value[0];

                count = count + 1;
            }

            result = &result[1];
        }

        // Iterate backwards and trim "space" characters from the output's end.
        for (u32 x = count - 1; 0 < x; x--)
        {
            if (!isspace(output[x])) { break; }

            count = count - 1;
        }

        output[count] = NULL;

        // Additionally skip all "space" characters at the end.
        while (isspace(result[0])) { result = &result[1]; }

        return result;
    }
}