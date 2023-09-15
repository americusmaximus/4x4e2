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

using namespace Objects;

namespace IO::Streams
{
    AbstractInStreamContainer AbstractInStreamState;

    // 0x005d0cf0
    AbstractInStream* InitializeAbstractInStream(AbstractInStream* self)
    {
        self->Self = &AbstractInStreamState.AbstractInStreamSelf;

        return self;
    }

    // 0x005d0d00
    void* ReleaseAbstractInStream(AbstractInStream* self, const ReleaseMode mode)
    {
        self->Self = &AbstractInStreamState.AbstractInStreamSelf;

        return self;
    }

    s32 TakeAbstractInStream(AbstractInStream* self) { return STREAM_RESULT_SUCCESS; }

    // 0x005d0d30
    BOOL PutAbstractInStream(AbstractInStream* self, const s32 value) { return FALSE; }

    s32 PeekAbstractInStream(AbstractInStream* self) { return STREAM_RESULT_SUCCESS; }

    s32 ReadAbstractInStream(AbstractInStream* self, void* data, const u32 size) { return STREAM_RESULT_SUCCESS; }

    // 0x005d0d30
    BOOL SeekAbstractInStream(AbstractInStream* self, const s32 offset, const StreamSeek mode) { return FALSE; }

    // 0x005d0d40
    s32 TellAbstractInStream(AbstractInStream* self) { return STREAM_RESULT_END_OF_DATA; }

    BOOL IsAbstractInStreamAvailable(AbstractInStream* self) { return FALSE; }

    s32 IsAbstractInStreamEndReached(AbstractInStream* self) { return STREAM_RESULT_SUCCESS; }
}