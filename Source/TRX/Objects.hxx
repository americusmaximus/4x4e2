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
#include "Memory.hxx"

namespace Objects
{
    // TODO
    OPTIONS(ReleaseMode, u32)
    {
        None = 0,
        Unknown1 = 1,
        Unknown2 = 2,
        Unknown4 = 4
    };

    typedef const void* (CDECLAPI* ABSTRACTOBJECTINITIALIZERINITIALIZE) (void* self);
    typedef const void* (CDECLAPI* ABSTRACTOBJECTINITIALIZERASSIGN) (void* self, void* value);
    typedef const void* (CDECLAPI* ABSTRACTOBJECTINITIALIZERRELEASE) (void* self, const ReleaseMode mode);

    struct AbstractObjectInitializer
    {
        u32 Options; // TODO enum
        ABSTRACTOBJECTINITIALIZERINITIALIZE Initialize;
        ABSTRACTOBJECTINITIALIZERASSIGN Assign;
        ABSTRACTOBJECTINITIALIZERRELEASE Release;
        u32 Size;
        const char* Name;
    };

    void* InitializeObjectCollection(void* self, const u32 count, const AbstractObjectInitializer* initializer);
    void* ReleaseObject(void* self, const AbstractObjectInitializer* initializer);
    void* ReleaseObjectCollection(void* self, const u32 count, const AbstractObjectInitializer* initializer);
}