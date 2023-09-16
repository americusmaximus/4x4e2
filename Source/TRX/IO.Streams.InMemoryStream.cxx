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

#include "IO.Streams.hxx"
#include "Mathematics.Basic.hxx"
#include "Memory.hxx"

using namespace Mathematics;
using namespace Memory;
using namespace Objects;

namespace IO::Streams
{
    InStreamMemoryContainer InStreamMemoryState;

    // 0x005d2b50
    InStreamMemory* AssignInStreamMemory(InStreamMemory* self, InStreamMemory* value)
    {
        InitializeAbstractInStream((AbstractInStream*)self);

        self->Self = &InStreamMemoryState.InStreamMemorySelf;

        return self;
    }

    // 0x005d2ad0
    InStreamMemory* ConstructInStreamMemory(InStreamMemory* self)
    {
        self = (InStreamMemory*)InitializeAbstractInStream((AbstractInStream*)self);
        self->Self = &InStreamMemoryState.InStreamMemorySelf;

        self->Buffer = NULL;
        self->Size = 0;
        self->Offset = 0;
        self->Mode = StreamMode::Text;

        return self;
    }

    // 0x005d2b00
    InStreamMemory* ConstructInStreamMemoryText(InStreamMemory* self, void* data, const u32 size)
    {
        self = (InStreamMemory*)InitializeAbstractInStream((AbstractInStream*)self);
        self->Self = &InStreamMemoryState.InStreamMemorySelf;

        self->Buffer = NULL;
        self->Size = 0;
        self->Offset = 0;
        self->Mode = StreamMode::Text;

        SetInStreamMemoryText(self, data, size);

        return self;
    }

    // 0x005d2b80
    void* ReleaseInStreamMemory(InStreamMemory* self, const ReleaseMode mode)
    {
        if ((mode & ReleaseMode::Unknown4) != ReleaseMode::None)
        {
            ReleaseMemory3(ReleaseObject(self, &InStreamMemoryState.Initializer));

            return self;
        }

        self->Self = &InStreamMemoryState.InStreamMemorySelf;

        CloseInStreamMemory(self);

        auto abs = ReleaseAbstractInStream((AbstractInStream*)self, ReleaseMode::Unknown1);

        if ((mode & ReleaseMode::Unknown2) != ReleaseMode::None) { ReleaseMemory2(self); }

        return abs;
    }

    // 0x005d2be0
    s32 TakeInStreamMemory(InStreamMemory* self)
    {
        if (self->Size <= self->Offset) { return STREAM_RESULT_END_OF_DATA; }
        if (self->Buffer == NULL) { return STREAM_RESULT_FAILURE; }

        const auto result = self->Buffer[self->Offset];

        self->Offset = self->Offset + 1;

        return result;
    }

    // 0x005d2c30
    BOOL PutInStreamMemory(InStreamMemory* self, const s32 value)
    {
        if (MIN_STREAM_ITEM_VALUE < value && value < MAX_STREAM_ITEM_VALUE && self->Buffer != NULL && 0 < self->Offset)
        {
            self->Offset = self->Offset + -1;

            return TRUE;
        }

        return FALSE;
    }

    // 0x005d2c70
    s32 PeekInStreamMemory(InStreamMemory* self)
    {
        if (self->Buffer == NULL) { return STREAM_RESULT_FAILURE; }
        if (self->Size <= self->Offset) { return STREAM_RESULT_END_OF_DATA; }

        return self->Buffer[self->Offset];
    }

    // 0x005d2ca0
    s32 ReadInStreamMemory(InStreamMemory* self, void* data, const u32 size)
    {
        if (self->Buffer == NULL) { return STREAM_RESULT_FAILURE; }

        if (self->Offset < self->Size)
        {
            const auto length = Min<u32>(self->Size - self->Offset, size);

            CopyMemory(data, &self->Buffer[self->Offset], length);

            self->Offset = self->Offset + length;

            return length;
        }

        return STREAM_RESULT_END_OF_DATA;
    }

    // 0x005d2d00
    BOOL SeekInStreamMemory(InStreamMemory* self, const s32 offset, const StreamSeek mode)
    {
        if (self->Buffer == NULL) { return STREAM_RESULT_FAILURE; }

        s32 off = 0;

        switch (mode)
        {
        case StreamSeek::Set: { off = offset; break; }
        case StreamSeek::Current: { off = offset + self->Offset; break; }
        case StreamSeek::End: { off = offset + self->Size; break; }
        }

        if (-1 < off && off <= self->Size)
        {
            self->Offset = off;

            return TRUE;
        }

        return FALSE;
    }

    // 0x005d2d50
    s32 TellInStreamMemory(InStreamMemory* self)
    {
        if (self->Buffer == NULL) { return STREAM_RESULT_FAILURE; }

        return self->Offset;
    }

    BOOL IsInStreamMemoryAvailable(InStreamMemory* self)
    {
        return self->Buffer != NULL;
    }

    // 0x005d2d70
    s32 IsInStreamMemoryEndReached(InStreamMemory* self)
    {
        if (self->Buffer == NULL) { return STREAM_RESULT_END_OF_DATA; }

        if (self->Size <= self->Offset) { return TRUE; }

        return FALSE;
    }

    // 0x005d2da0
    void CloseInStreamMemory(InStreamMemory* self)
    {
        if (self->Buffer != NULL)
        {
            if (self->Mode != StreamMode::Text)
            {
                ReleaseMemory1(self->Buffer);
            }

            self->Buffer = NULL;
        }

        self->Size = 0;
        self->Offset = 0;
        self->Mode = StreamMode::Text;
    }

    // 0x005d2df0
    void SetInStreamMemoryText(InStreamMemory* self, void* data, const u32 size)
    {
        self->Self->Close(self);

        self->Offset = 0;
        self->Mode = StreamMode::Text;
        self->Buffer = (u8*)data;
        self->Size = size;
    }

    // 0x005d2e20
    void SetInStreamMemoryData(InStreamMemory* self, void* data, const u32 size)
    {
        self->Self->Close(self);

        self->Offset = 0;
        self->Mode = StreamMode::Data;
        self->Buffer = (u8*)data;
        self->Size = size;
    }
}