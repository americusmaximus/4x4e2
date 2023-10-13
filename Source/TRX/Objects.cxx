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

#include "Objects.hxx"

namespace Objects
{
    // 0x0060f9fd
    void* InitializeObjectCollection(void* self, const u32 count, const AbstractObjectInitializer* initializer)
    {
        auto address = (addr)self;

        for (u32 x = 0; x < count; x++)
        {
            initializer->Initialize((void*)address);

            address = address + initializer->Size;
        }

        return self;
    }

    // 0x0060fae6
    void* ReleaseObject(void* self, const AbstractObjectInitializer* initializer)
    {
        if (self == NULL) { return NULL; }

        const auto count = *(u32*)(((addr)self) - 4); // TODO

        ReleaseObjectCollection(self, count, initializer);

        return (void*)(((addr)self) - 4); // TODO
    }

    // 0x0060fb55
    void* ReleaseObjectCollection(void* self, const u32 count, const AbstractObjectInitializer* initializer)
    {
        for (u32 x = 0; x < count; x++)
        {
            initializer->Release((void*)(((addr)self) + x * initializer->Size), ReleaseMode::None);
        }

        return self;
    }
}