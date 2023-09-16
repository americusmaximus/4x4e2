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

#include "ASCII.hxx"
#include "IO.hxx"
#include "Objects.hxx"

#define STREAM_RESULT_SUCCESS 0
#define STREAM_RESULT_END_OF_DATA (-1)
#define STREAM_RESULT_FAILURE (-2)

#define MIN_STREAM_ITEM_VALUE (ASCII_NULL - 1)
#define MAX_STREAM_ITEM_VALUE (MAX_ASCII_CODE + 1)

namespace IO::Streams
{
    enum class StreamSeek : u32
    {
        Set = 0,
        Current = 1,
        End = 2
    };

    enum class StreamMode : u32
    {
        Text = 0,
        Data = 1
    };

    struct AbstractInStream;

    typedef const void* (CDECLAPI* ABSTRACTINSTREAMRELEASE) (AbstractInStream* self, const Objects::ReleaseMode mode);
    typedef const s32(CDECLAPI* ABSTRACTINSTREAMTAKE) (AbstractInStream* self);
    typedef const BOOL(CDECLAPI* ABSTRACTINSTREAMPUT) (AbstractInStream* self, const s32 value);
    typedef const s32(CDECLAPI* ABSTRACTINSTREAMPEEK) (AbstractInStream* self);
    typedef const s32(CDECLAPI* ABSTRACTINSTREAMREAD) (AbstractInStream* self, void* data, const u32 size);
    typedef const BOOL(CDECLAPI* ABSTRACTINSTREAMSEEK) (AbstractInStream* self, const s32 offset, const StreamSeek mode);
    typedef const s32(CDECLAPI* ABSTRACTINSTREAMTELL) (AbstractInStream* self);
    typedef const BOOL(CDECLAPI* ABSTRACTINSTREAMISAVAILABLE) (AbstractInStream* self);
    typedef const s32(CDECLAPI* ABSTRACTINSTREAMISENDREACHED) (AbstractInStream* self);

    AbstractInStream* InitializeAbstractInStream(AbstractInStream* self);

    void* ReleaseAbstractInStream(AbstractInStream* self, const Objects::ReleaseMode mode);
    s32 TakeAbstractInStream(AbstractInStream* self);
    BOOL PutAbstractInStream(AbstractInStream* self, const s32 value);
    s32 PeekAbstractInStream(AbstractInStream* self);
    s32 ReadAbstractInStream(AbstractInStream* self, void* data, const u32 size);
    BOOL SeekAbstractInStream(AbstractInStream* self, const s32 offset, const StreamSeek mode);
    s32 TellAbstractInStream(AbstractInStream* self);
    BOOL IsAbstractInStreamAvailable(AbstractInStream* self);
    s32 IsAbstractInStreamEndReached(AbstractInStream* self);

    struct AbstractInStreamSelf
    {
        ABSTRACTINSTREAMRELEASE Release = (ABSTRACTINSTREAMRELEASE)&ReleaseAbstractInStream; // 0x005d0d00
        ABSTRACTINSTREAMTAKE Take = (ABSTRACTINSTREAMTAKE)&TakeAbstractInStream; // 0x006121af
        ABSTRACTINSTREAMPUT Put = (ABSTRACTINSTREAMPUT)&PutAbstractInStream; // 0x005d0d30
        ABSTRACTINSTREAMPEEK Peek = (ABSTRACTINSTREAMPEEK)&PeekAbstractInStream; // 0x006121af
        ABSTRACTINSTREAMREAD ReadBlock = (ABSTRACTINSTREAMREAD)&ReadAbstractInStream; // 0x006121af
        ABSTRACTINSTREAMSEEK Seek = (ABSTRACTINSTREAMSEEK)&SeekAbstractInStream; // 0x005d0d30
        ABSTRACTINSTREAMTELL Tell = (ABSTRACTINSTREAMTELL)&TellAbstractInStream; // 0x005d0d30
        ABSTRACTINSTREAMISAVAILABLE IsAvailable = (ABSTRACTINSTREAMISAVAILABLE)&IsAbstractInStreamAvailable; // 0x0x006121af
        ABSTRACTINSTREAMISENDREACHED IsEndReached = (ABSTRACTINSTREAMISENDREACHED)&IsAbstractInStreamEndReached; // 0x0x006121af
    };

    struct AbstractInStream
    {
        AbstractInStreamSelf* Self;
    };

    struct AbstractInStreamContainer
    {
        AbstractInStreamSelf AbstractInStreamSelf; // 0x00667bf4
    };

    extern AbstractInStreamContainer AbstractInStreamState;

    struct InStreamFile;

    typedef const void* (CDECLAPI* INSTREAMFILERELEASE) (InStreamFile* self, const Objects::ReleaseMode mode);
    typedef const s32(CDECLAPI* INSTREAMFILETAKE) (InStreamFile* self);
    typedef const BOOL(CDECLAPI* INSTREAMFILEPUT) (InStreamFile* self, const s32 value);
    typedef const s32(CDECLAPI* INSTREAMFILEPEEK) (InStreamFile* self);
    typedef const s32(CDECLAPI* INSTREAMFILEREAD) (InStreamFile* self, void* data, const u32 size);
    typedef const BOOL(CDECLAPI* INSTREAMFILESEEK) (InStreamFile* self, const s32 offset, const StreamSeek mode);
    typedef const s32(CDECLAPI* INSTREAMFILETELL) (InStreamFile* self);
    typedef const BOOL(CDECLAPI* INSTREAMFILEISAVAILABLE) (InStreamFile* self);
    typedef const s32(CDECLAPI* INSTREAMFILEISENDREACHED) (InStreamFile* self);
    typedef const BOOL(CDECLAPI* INSTREAMFILECLOSE) (InStreamFile* self);
    typedef const BOOL(CDECLAPI* INSTREAMFILEOPENFILE) (InStreamFile* self, const char* file, const StreamMode mode);
    typedef const BOOL(CDECLAPI* INSTREAMFILEOPENFILEPATH) (InStreamFile* self, const char* dir, const char* file, const StreamMode mode);
    typedef const BOOL(CDECLAPI* INSTREAMFILEOPENFILEDESCRIPTOR) (InStreamFile* self, const FileDescriptor* desc, const StreamMode mode);

    InStreamFile* AssignInStreamFile(InStreamFile* self, InStreamFile* value);
    InStreamFile* ConstructInStreamFile(InStreamFile* self);

    void* ReleaseInStreamFile(InStreamFile* self, const Objects::ReleaseMode mode);
    s32 TakeInStreamFile(InStreamFile* self);
    BOOL PutInStreamFile(InStreamFile* self, const s32 value);
    s32 PeekInStreamFile(InStreamFile* self);
    s32 ReadInStreamFile(InStreamFile* self, void* data, const u32 size);
    BOOL SeekInStreamFile(InStreamFile* self, const s32 offset, const StreamSeek mode);
    s32 TellInStreamFile(InStreamFile* self);
    BOOL IsInStreamFileAvailable(InStreamFile* self);
    s32 IsInStreamFileEndReached(InStreamFile* self);
    BOOL CloseInStreamFile(InStreamFile* self);
    BOOL OpenInStreamFileFile(InStreamFile* self, const char* file, const StreamMode mode);
    BOOL OpenInStreamFileFilePath(InStreamFile* self, const char* dir, const char* file, const StreamMode mode);
    BOOL OpenInStreamFileFileDescriptor(InStreamFile* self, const FileDescriptor* desc, const StreamMode mode);

    // INHERITANCE: AbstractInStreamSelf
    struct InStreamFileSelf
    {
        INSTREAMFILERELEASE Release = (INSTREAMFILERELEASE)&ReleaseInStreamFile; // 0x005d2300
        INSTREAMFILETAKE Take = (INSTREAMFILETAKE)&TakeInStreamFile; // 0x005d2360
        INSTREAMFILEPUT Put = (INSTREAMFILEPUT)&PutInStreamFile; // 0x005d23c0
        INSTREAMFILEPEEK Peek = (INSTREAMFILEPEEK)&PeekInStreamFile; // 0x005d2440
        INSTREAMFILEREAD Read = (INSTREAMFILEREAD)&ReadInStreamFile; // 0x005d2500
        INSTREAMFILESEEK Seek = (INSTREAMFILESEEK)&SeekInStreamFile; // 0x005d2620
        INSTREAMFILETELL Tell = (INSTREAMFILETELL)&TellInStreamFile; // 0x005d2670
        INSTREAMFILEISAVAILABLE IsAvailable = (INSTREAMFILEISAVAILABLE)&IsInStreamFileAvailable; // 0x005d2690
        INSTREAMFILEISENDREACHED IsEndReached = (INSTREAMFILEISENDREACHED)&IsInStreamFileEndReached;// 0x005d26b0
        INSTREAMFILECLOSE Close = (INSTREAMFILECLOSE)&CloseInStreamFile; // 0x005d26e0
        INSTREAMFILEOPENFILE OpenFile = (INSTREAMFILEOPENFILE)&OpenInStreamFileFile; // 0x005d2970
        INSTREAMFILEOPENFILEPATH OpenPath = (INSTREAMFILEOPENFILEPATH)&OpenInStreamFileFilePath; // 0x005d29e0
        INSTREAMFILEOPENFILEDESCRIPTOR OpenDescriptor = (INSTREAMFILEOPENFILEDESCRIPTOR)&OpenInStreamFileFileDescriptor; // 0x005d2a60
    };

    // a.k.a. CInStreamFile
    // INHERITANCE: AbstractInStream
    struct InStreamFile
    {
        InStreamFileSelf* Self;

        struct
        {
            HANDLE Handle;
            char Name[STANDARD_IO_FILE_NAME_LENGTH];
            s32 Size;
            s32 Offset;
            s32 Position;
        } File;

        struct
        {
            struct
            {
                u8* Allocated;
                u8* Aligned;
            } Data;

            s32 Offset;
            s32 Size;
            s32 Length;
        } Buffer;

        StreamMode Mode;
    };

    struct InStreamFileContainer
    {
        InStreamFileSelf InStreamFileSelf; // 0x00667c24

        // 0x00662500
        Objects::AbstractObjectInitializer Initializer =
        {
            .Options = 0x20100, // TODO
            .Initialize = (Objects::ABSTRACTOBJECTINITIALIZERINITIALIZE)&ConstructInStreamFile,
            .Assign = (Objects::ABSTRACTOBJECTINITIALIZERASSIGN)&AssignInStreamFile,
            .Release = (Objects::ABSTRACTOBJECTINITIALIZERRELEASE)&ReleaseInStreamFile,
            .Size = sizeof(InStreamFile),
            .Name = "$CInStreamFile$$"
        };
    };

    extern InStreamFileContainer InStreamFileState;

    BOOL AcquireInStreamFileData(InStreamFile* self, const s32 offset);
    BOOL AcquireInStreamFileDetails(InStreamFile* self, const char* file);
    s32 AcquireInStreamFileValue(InStreamFile* self, const s32 offset);
    void AllocateInStreamFileBuffer(InStreamFile* self);
    void DisposeInStreamFile(InStreamFile* self);

    struct InStreamMemory;

    typedef const void* (CDECLAPI* INSTREAMMEMORYRELEASE) (InStreamMemory* self, const Objects::ReleaseMode mode);
    typedef const s32(CDECLAPI* INSTREAMMEMORYTAKE) (InStreamMemory* self);
    typedef const BOOL(CDECLAPI* INSTREAMMEMORYPUT) (InStreamMemory* self, const s32 value);
    typedef const s32(CDECLAPI* INSTREAMMEMORYPEEK) (InStreamMemory* self);
    typedef const s32(CDECLAPI* INSTREAMMEMORYREAD) (InStreamMemory* self, void* data, const u32 size);
    typedef const BOOL(CDECLAPI* INSTREAMMEMORYSEEK) (InStreamMemory* self, const s32 offset, const StreamSeek mode);
    typedef const s32(CDECLAPI* INSTREAMMEMORYTELL) (InStreamMemory* self);
    typedef const BOOL(CDECLAPI* INSTREAMMEMORYISAVAILABLE) (InStreamMemory* self);
    typedef const s32(CDECLAPI* INSTREAMMEMORYISENDREACHED) (InStreamMemory* self);
    typedef const void(CDECLAPI* INSTREAMMEMORYCLOSE) (InStreamMemory* self);
    typedef const void(CDECLAPI* INSTREAMMEMORYSETTEXT) (InStreamMemory* self, void* data, const u32 size);
    typedef const void(CDECLAPI* INSTREAMMEMORYSETDATA) (InStreamMemory* self, void* data, const u32 size);

    InStreamMemory* AssignInStreamMemory(InStreamMemory* self, InStreamMemory* value);
    InStreamMemory* ConstructInStreamMemory(InStreamMemory* self);

    void* ReleaseInStreamMemory(InStreamMemory* self, const Objects::ReleaseMode mode);
    s32 TakeInStreamMemory(InStreamMemory* self);
    BOOL PutInStreamMemory(InStreamMemory* self, const s32 value);
    s32 PeekInStreamMemory(InStreamMemory* self);
    s32 ReadInStreamMemory(InStreamMemory* self, void* data, const u32 size);
    BOOL SeekInStreamMemory(InStreamMemory* self, const s32 offset, const StreamSeek mode);
    s32 TellInStreamMemory(InStreamMemory* self);
    BOOL IsInStreamMemoryAvailable(InStreamMemory* self);
    s32 IsInStreamMemoryEndReached(InStreamMemory* self);
    void CloseInStreamMemory(InStreamMemory* self);
    void SetInStreamMemoryText(InStreamMemory* self, void* data, const u32 size);
    void SetInStreamMemoryData(InStreamMemory* self, void* data, const u32 size);

    // INHERITANCE: AbstractInStreamSelf
    struct InStreamMemorySelf
    {
        INSTREAMMEMORYRELEASE Release = (INSTREAMMEMORYRELEASE)&ReleaseInStreamMemory; // 0x005d2b80
        INSTREAMMEMORYTAKE Take = (INSTREAMMEMORYTAKE)&TakeInStreamMemory; // 0x005d2be0
        INSTREAMMEMORYPUT Put = (INSTREAMMEMORYPUT)&PutInStreamMemory; // 0x005d2c30
        INSTREAMMEMORYPEEK Peek = (INSTREAMMEMORYPEEK)&PeekInStreamMemory; // 0x005d2c70
        INSTREAMMEMORYREAD Read = (INSTREAMMEMORYREAD)&ReadInStreamMemory; // 0x005d2ca0
        INSTREAMMEMORYSEEK Seek = (INSTREAMMEMORYSEEK)&SeekInStreamMemory; // 0x005d2d00
        INSTREAMMEMORYTELL Tell = (INSTREAMMEMORYTELL)&TellInStreamMemory; // 0x005d2d50
        INSTREAMMEMORYISAVAILABLE IsAvailable = (INSTREAMMEMORYISAVAILABLE)&IsInStreamMemoryAvailable; // 0x005d2690
        INSTREAMMEMORYISENDREACHED IsEndReached = (INSTREAMMEMORYISENDREACHED)&IsInStreamMemoryEndReached;// 0x005d2d70
        INSTREAMMEMORYCLOSE Close = (INSTREAMMEMORYCLOSE)&CloseInStreamMemory; // 0x005d2da0
        INSTREAMMEMORYSETTEXT SetText = (INSTREAMMEMORYSETTEXT)&SetInStreamMemoryText; // 0x005d2df0
        INSTREAMMEMORYSETDATA SetData = (INSTREAMMEMORYSETDATA)&SetInStreamMemoryData; // 0x005d2e20
    };

    // a.k.a. CInStreamMemory
    // INHERITANCE: AbstractInStream
    struct InStreamMemory
    {
        InStreamMemorySelf* Self;

        u8* Buffer;
        s32 Size;
        s32 Offset;

        StreamMode Mode;
    };

    struct InStreamMemoryContainer
    {
        InStreamMemorySelf InStreamMemorySelf; // 0x00667c64

        // 0x00663830
        Objects::AbstractObjectInitializer Initializer =
        {
            .Options = 0x20100, // TODO
            .Initialize = (Objects::ABSTRACTOBJECTINITIALIZERINITIALIZE)&ConstructInStreamMemory,
            .Assign = (Objects::ABSTRACTOBJECTINITIALIZERASSIGN)&AssignInStreamMemory,
            .Release = (Objects::ABSTRACTOBJECTINITIALIZERRELEASE)&ReleaseInStreamMemory,
            .Size = sizeof(InStreamMemory),
            .Name = "$CInStreamMemory$$"
        };
    };

    extern InStreamMemoryContainer InStreamMemoryState;

    InStreamMemory* ConstructInStreamMemoryText(InStreamMemory* self, void* data, const u32 size);
}