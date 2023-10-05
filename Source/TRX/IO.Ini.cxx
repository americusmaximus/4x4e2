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
#include "Memory.hxx"
#include "Strings.hxx"
#include "Logger.hxx"

#include <stdio.h>

#define DEFAULT_INI_LINE_INDEX 0
#define INVALID_INI_CHARACTER_INDEX (-1)

#define MAX_INI_NUMBER_BUFFER_LENGTH 100

using namespace Logger;
using namespace Strings;
using namespace Memory;
using namespace Objects;

namespace IO::Ini
{
    extern IniContainer IniState;

    // 0x0047e720
    Ini* ConstructIni(Ini* self) { return self; }

    // 0x0047e730
    void* ReleaseIni(Ini* self) { return self; }

    // 0x0047e1e0
    // 0x0047db60
    BOOL ReadIni(const char* file, const char* section, const char* name, char* value, const u32 size)
    {
        auto f = fopen(file, "rt");

        BOOL found = FALSE;
        s32 index = INVALID_INI_CHARACTER_INDEX;
        char line[MAX_INI_STRING_BUFFER_LENGTH];

        if (f != NULL)
        {
            char tag[MAX_INI_STRING_BUFFER_LENGTH];

            sprintf(tag, "[%s]\n", section);

            while (!feof(f) && fgets(line, MAX_INI_STRING_BUFFER_LENGTH, f) != NULL)
            {
                if (EqualStrings(line, tag)) { found = TRUE; break; }
            }

            if (found)
            {
                found = FALSE;

                const auto vln = strlen(name);

                while (!feof(f) && fgets(line, MAX_INI_STRING_BUFFER_LENGTH, f) != NULL)
                {
                    if (StartsWithString(line, name))
                    {
                        if (line[vln] != ASCII_EQUAL && line[vln] != ASCII_SPACE) { continue; }

                        found = TRUE;

                        for (u32 x = 0; x < MAX_INI_STRING_BUFFER_LENGTH; x++)
                        {
                            const auto v = line[x];

                            if (v == NULL) { break; }

                            if (index == INVALID_INI_CHARACTER_INDEX && v == ASCII_EQUAL) { index = x + 1; }

                            if (v == ASCII_CARRIAGE_RETURN || v == ASCII_NEW_LINE) { line[x] = NULL; }
                        }

                        break;
                    }
                }
            }

            fclose(f);
        }

        if (!found) { return FALSE; }

        ZeroMemory(value, size);

        strncpy(value, &line[index], strlen(&line[index]));

        return TRUE;
    }

    // 0x0047dd40
    u32 AcquireIniState(const char* file, const char* section, const char* name, const char* value, BOOL* obsolete)
    {
        *obsolete = TRUE;

        u32 result = 0;
        BOOL found = FALSE;
        auto f = fopen(file, "rt");

        if (f == NULL) { return DEFAULT_INI_LINE_INDEX; }

        char buffer[MAX_INI_STRING_BUFFER_LENGTH];
        char line[MAX_INI_STRING_BUFFER_LENGTH];

        sprintf(buffer, "[%s]\n", section);

        // Search for the section.
        while (!feof(f) && fgets(line, (MAX_INI_STRING_BUFFER_LENGTH - 1), f) != NULL)
        {
            result = result + 1;

            if (EqualStrings(line, buffer)) { found = TRUE; break; }
        }

        if (found)
        {
            found = FALSE;

            const auto vln = strlen(name);

            // Search for the property.
            while (!feof(f) && fgets(line, (MAX_INI_STRING_BUFFER_LENGTH - 1), f) != NULL)
            {
                if (StartsWithString(line, name))
                {
                    if (line[vln] != ASCII_EQUAL && line[vln] != ASCII_SPACE) { continue; }

                    found = TRUE;

                    // Search for the value.
                    for (u32 x = 0; x < MAX_INI_STRING_BUFFER_LENGTH; x++)
                    {
                        const auto v = line[x];

                        if (v == NULL) { break; }

                        if (v == ASCII_EQUAL)
                        {
                            // Check if the value is changed.
                            if (StartsWithString(&line[x + 1], value)) { *obsolete = FALSE; break; }
                        }
                    }

                    // At this point it is clear that that value is missing or is changed,
                    // therefore there is no need to continue the search.
                    break;
                }

                result = result + 1;
            }
        }

        fclose(f);

        return found ? result : DEFAULT_INI_LINE_INDEX;
    }

    // 0x0047e2c0
    // 0x0047dee0
    // a.k.a. writeProfileString
    BOOL WriteIni(const char* file, const char* section, const char* name, const char* value)
    {
        BOOL obsolete = FALSE;
        const auto line = AcquireIniState(file, section, name, value, &obsolete);

        if (!obsolete) { return TRUE; }

        auto original = fopen(file, "rt");

        if (original == NULL)
        {
            auto writer = fopen(file, "wt");

            if (writer == NULL) { return FALSE; }

            fclose(writer);

            original = fopen(file, "rt");

            if (original == NULL) { return FALSE; }
        }

        char tmp[MAX_INI_SECTION_LENGTH];

        strcpy(tmp, file);
        tmp[strlen(tmp) - 1] = ASCII_CHARACTER_LOWER_CASE_X;

        const auto backup = fopen(tmp, "wt");

        if (backup == NULL) { LogError("Unable to create an ini file to write to."); }

        if (line == DEFAULT_INI_LINE_INDEX)
        {
            BOOL found = FALSE;
            char tag[MAX_INI_STRING_BUFFER_LENGTH];
            char line[MAX_INI_STRING_BUFFER_LENGTH];

            sprintf(tag, "[%s]\n", section);

            while (!feof(original) && fgets(line, (MAX_INI_STRING_BUFFER_LENGTH - 1), original) != NULL)
            {
                fprintf(backup, line);

                if (EqualStrings(tag, line))
                {
                    fprintf(backup, "%s=%s\n", name, value);
                    found = TRUE;
                }
            }

            if (!found)
            {
                fprintf(backup, tag);
                fprintf(backup, "%s=%s\n", name, value);
            }
        }
        else
        {
            char buffer[MAX_INI_STRING_BUFFER_LENGTH];

            for (u32 x = 0; x < line; x++)
            {
                fgets(buffer, (MAX_INI_STRING_BUFFER_LENGTH - 1), original);
                fprintf(backup, buffer);
            }

            fprintf(backup, "%s=%s\n", name, value);

            fgets(buffer, (MAX_INI_STRING_BUFFER_LENGTH - 1), original);

            while (!feof(original) && fgets(buffer, (MAX_INI_STRING_BUFFER_LENGTH - 1), original) != NULL)
            {
                fprintf(backup, buffer);
            }
        }

        fclose(original);
        fclose(backup);

        remove(file);
        rename(tmp, file);

        return TRUE;
    }
}