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
#include "Logger.hxx"
#include "Memory.hxx"

#include <stdlib.h>

#define IN_STREAM_FILE_BUFFER_SIZE 4096
#define IN_STREAM_FILE_BUFFER_ALIGNMENT_VALUE 32
#define IN_STREAM_FILE_BUFFER_ALIGNMENT_MASK (~(IN_STREAM_FILE_BUFFER_ALIGNMENT_VALUE - 1))

#define IN_STREAM_FILE_DISTANCE_TO_SEEK_STEP 0xfff
#define IN_STREAM_FILE_DISTANCE_TO_SEEK_MASK 0xfffff000

using namespace Logger;
using namespace Memory;
using namespace Objects;

namespace IO::Streams
{
    InStreamFileContainer InStreamFileState;

    // 0x005d22d0
    InStreamFile* AssignInStreamFile(InStreamFile* self, InStreamFile* value)
    {
        InitializeAbstractInStream((AbstractInStream*)self);

        self->Self = &InStreamFileState.InStreamFileSelf;

        return self;
    }

    // 0x005d2130
    InStreamFile* ConstructInStreamFile(InStreamFile* self)
    {
        self = (InStreamFile*)InitializeAbstractInStream((AbstractInStream*)self);
        self->Self = (InStreamFileSelf*)&InStreamFileState.InStreamFileSelf;

        self->File.Handle = 0;
        self->File.Size = 0;
        self->File.Offset = 0;
        self->File.Name[0] = NULL;
        self->File.Position = 0;

        self->Buffer.Data.Allocated = NULL;
        self->Buffer.Data.Aligned = NULL;
        self->Buffer.Size = 0;
        self->Buffer.Length = 0;
        self->Buffer.Offset = 0;

        return self;
    }

    // 0x005d2300
    void* ReleaseInStreamFile(InStreamFile* self, const ReleaseMode mode)
    {
        if ((mode & ReleaseMode::Unknown4) != ReleaseMode::None)
        {
            ReleaseMemory3(ReleaseObject(self, &InStreamFileState.InStreamFileInitializer));

            return self;
        }

        self->Self = &InStreamFileState.InStreamFileSelf;

        CloseInStreamFile(self);

        auto abs = ReleaseAbstractInStream((AbstractInStream*)self, ReleaseMode::Unknown1);

        if ((mode & ReleaseMode::Unknown2) != ReleaseMode::None) { ReleaseMemory2(self); }

        return abs;
    }

    // 0x005d2360
    s32 TakeInStreamFile(InStreamFile* self)
    {
        const auto result = AcquireInStreamFileValue(self, self->File.Position);

        if (MIN_STREAM_ITEM_VALUE < result)
        {
            self->File.Position = self->File.Position + 1;

            if (self->Mode == StreamMode::Text && result == ASCII_CARRIAGE_RETURN)
            {
                if (AcquireInStreamFileValue(self, self->File.Position) == ASCII_NEW_LINE)
                {
                    self->File.Position = self->File.Position + 1;

                    return ASCII_NEW_LINE;
                }

                return ASCII_NEW_LINE;
            }
        }

        return result;
    }

    // 0x005d23c0
    BOOL PutInStreamFile(InStreamFile* self, const s32 value)
    {
        if (MIN_STREAM_ITEM_VALUE < value && value < MAX_STREAM_ITEM_VALUE
            && self->File.Handle != NULL && 0 < self->File.Position)
        {
            self->File.Position = self->File.Position - 1;

            if (value == ASCII_NEW_LINE && self->Mode == StreamMode::Text && 0 < self->File.Position)
            {
                if (AcquireInStreamFileValue(self, self->File.Position) == ASCII_NEW_LINE)
                {
                    if (AcquireInStreamFileValue(self, self->File.Position - 1) == ASCII_CARRIAGE_RETURN)
                    {
                        self->File.Position = self->File.Position - 1;

                        return TRUE;
                    }
                }
            }

            return TRUE;
        }

        return FALSE;
    }

    // 0x005d2440
    s32 PeekInStreamFile(InStreamFile* self)
    {
        const auto result = AcquireInStreamFileValue(self, self->File.Position);

        if (STREAM_RESULT_END_OF_DATA < result)
        {
            if (self->Mode != StreamMode::Text || result != ASCII_CARRIAGE_RETURN) { return result; }

            return ASCII_NEW_LINE;
        }

        return result;
    }

    // 0x005d2500
    s32 ReadInStreamFile(InStreamFile* self, void* data, const s32 count)
    {
        if (self->File.Handle == NULL) { return STREAM_RESULT_FAILURE; }
        if (self->File.Size <= self->File.Position) { return STREAM_RESULT_END_OF_DATA; }

        auto result = STREAM_RESULT_SUCCESS;
        auto readable = self->File.Size - self->File.Position;

        auto left = count;
        if (readable < count) { left = readable; }

        while (TRUE)
        {
            if (left < 1) { return result; }

            if (self->File.Position < self->Buffer.Offset || (self->Buffer.Offset + self->Buffer.Length <= self->File.Position))
            {
                if (!AcquireInStreamFileData(self, self->File.Position)) { break; }
            }

            readable = self->Buffer.Offset + self->Buffer.Length - self->File.Position;

            if (left < readable) { readable = left; }

            left = left - readable;

            CopyMemory(&((u8*)data)[result], &self->Buffer.Data.Aligned[self->File.Position - self->Buffer.Offset], readable);

            result = result + readable;

            self->File.Position = self->File.Position + readable;
        }

        return result;
    }

    // 0x005d2620
    BOOL SeekInStreamFile(InStreamFile* self, const s32 offset, const StreamSeek mode)
    {
        if (self->File.Handle == NULL) { return FALSE; }

        s32 off = 0;

        switch (mode)
        {
        case StreamSeek::Set: { off = offset; break; }
        case StreamSeek::Current: { off = offset + self->File.Position; break; }
        case StreamSeek::End: { off = offset + self->File.Size; break; }
        }

        if (-1 < off && off <= self->File.Size)
        {
            self->File.Position = off;

            return TRUE;
        }

        return FALSE;
    }

    // 0x005d2670
    s32 TellInStreamFile(InStreamFile* self)
    {
        if (self->File.Handle == NULL) { return STREAM_RESULT_END_OF_DATA; }

        return self->File.Position;
    }

    // 0x005d2690
    BOOL IsInStreamFileAvailable(InStreamFile* self)
    {
        return self->File.Handle != NULL;
    }

    // 0x005d26b0
    s32 IsInStreamFileEndReached(InStreamFile* self)
    {
        if (self->File.Handle == NULL) { return STREAM_RESULT_END_OF_DATA; }

        if (self->File.Size <= self->File.Position) { return TRUE; }

        return FALSE;
    }

    // 0x005d26e0
    BOOL CloseInStreamFile(InStreamFile* self)
    {
        // NOTE: Use of the file pool like in the original game was removed, I don't think there is a need for it.

        if (self->File.Handle != NULL)
        {
            CloseHandle(self->File.Handle);

            self->File.Handle = NULL;
        }

        self->File.Name[0] = NULL;

        self->File.Size = 0;
        self->File.Offset = 0;
        self->File.Position = 0;

        DisposeInStreamFile(self);

        return TRUE;
    }

    // 0x005d2970
    BOOL OpenInStreamFileFile(InStreamFile* self, const char* file, const StreamMode mode)
    {
        self->Self->Close(self);

        self->File.Position = 0;

        if (AcquireInStreamFileDetails(self, file))
        {
            self->File.Size = GetFileSize(self->File.Handle, NULL);

            if (-1 < self->File.Size)
            {
                AllocateInStreamFileBuffer(self);

                self->Mode = mode;

                return TRUE;
            }
        }

        self->Self->Close(self);

        return FALSE;
    }

    // 0x005d29e0
    BOOL OpenInStreamFileFilePath(InStreamFile* self, const char* dir, const char* file, const StreamMode mode)
    {
        self->Self->Close(self);

        self->File.Position = 0;

        FileDescriptor desc;
        InitializeFileDescriptor(&desc, dir, file);

        if (HandleFileDescriptor(&desc)) { return self->Self->OpenDescriptor(self, &desc, mode); }

        self->Self->Close(self);

        return FALSE;
    }

    // 0x005d2a60
    BOOL OpenInStreamFileFileDescriptor(InStreamFile* self, const FileDescriptor* desc, const StreamMode mode)
    {
        self->File.Size = desc->Size;
        self->File.Offset = desc->Offset;

        if (AcquireInStreamFileDetails(self, desc->Path))
        {
            if (self->Self->IsAvailable(self))
            {
                AllocateInStreamFileBuffer(self);

                self->Mode = mode;

                return TRUE;
            }
        }

        self->Self->Close(self);

        return FALSE;
    }

    // 0x005d2480
    s32 AcquireInStreamFileValue(InStreamFile* self, const s32 offset)
    {
        if (self->File.Handle == NULL) { return STREAM_RESULT_FAILURE; }

        if (offset < self->File.Size)
        {
            if (offset < self->Buffer.Offset || (self->Buffer.Offset + self->Buffer.Length <= offset))
            {
                if (!AcquireInStreamFileData(self, offset)) { return STREAM_RESULT_END_OF_DATA; }
            }

            return self->Buffer.Data.Aligned[offset - self->Buffer.Offset];
        }

        return STREAM_RESULT_END_OF_DATA;
    }

    // 0x005d28b0
    BOOL AcquireInStreamFileData(InStreamFile* self, const s32 offset)
    {
        const auto distance = (offset + self->File.Offset) & IN_STREAM_FILE_DISTANCE_TO_SEEK_MASK;

        self->Buffer.Length = self->File.Size - (distance - self->File.Offset);
        self->Buffer.Offset = distance - self->File.Offset;

        self->Buffer.Length = (self->Buffer.Length + IN_STREAM_FILE_DISTANCE_TO_SEEK_STEP) & IN_STREAM_FILE_DISTANCE_TO_SEEK_MASK;

        if (self->Buffer.Size < self->Buffer.Length)
        {
            self->Buffer.Length = self->Buffer.Size;
        }

        SetFilePointer(self->File.Handle, distance, NULL, FILE_BEGIN);

        DWORD read;
        ReadFile(self->File.Handle, self->Buffer.Data.Aligned, self->Buffer.Length, &read, NULL);

        self->Buffer.Length = read;

        if (read < 1)
        {
            self->Buffer.Length = 0;

            return FALSE;
        }

        return TRUE;
    }

    // 0x005d2060
    BOOL AcquireInStreamFileDetails(InStreamFile* self, const char* name)
    {
        // NOTE: Use of the file pool like in the original game was removed, I don't think there is a need for it.

        const auto handle = CreateFileA(name, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);

        if (handle == INVALID_HANDLE_VALUE || handle == NULL) { self->File.Handle = NULL; return FALSE; }

        self->File.Handle = handle;
        strcpy(self->File.Name, name);

        return TRUE;
    }

    // 0x005d27d0
    void AllocateInStreamFileBuffer(InStreamFile* self)
    {
        DisposeInStreamFile(self);

        self->Buffer.Size = IN_STREAM_FILE_BUFFER_SIZE;
        self->Buffer.Data.Allocated = (u8*)AllocateMemory(IN_STREAM_FILE_BUFFER_SIZE + IN_STREAM_FILE_BUFFER_ALIGNMENT_VALUE);

        if (self->Buffer.Data.Allocated == NULL) { LogError("Unable to allocate file buffer memory."); }

        self->Buffer.Offset = 0;
        self->Buffer.Length = 0;

        self->Buffer.Data.Aligned = (u8*)(((addr)self->Buffer.Data.Allocated + IN_STREAM_FILE_BUFFER_ALIGNMENT_VALUE) & IN_STREAM_FILE_BUFFER_ALIGNMENT_MASK);
    }

    // 0x005d2860
    void DisposeInStreamFile(InStreamFile* self)
    {
        if (self->Buffer.Data.Allocated != NULL)
        {
            ReleaseMemory1(self->Buffer.Data.Allocated);
            self->Buffer.Data.Allocated = NULL;
        }

        self->Buffer.Offset = 0;
        self->Buffer.Size = 0;
        self->Buffer.Length = 0;

        self->Buffer.Data.Aligned = NULL;
    }
}