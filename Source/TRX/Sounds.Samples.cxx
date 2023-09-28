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

#include "Assets.Sounds.hxx"
#include "IO.Streams.hxx"
#include "Logger.hxx"
#include "Memory.hxx"
#include "Sounds.hxx"
#include "Sounds.Samples.hxx"

using namespace Assets::Sounds;
using namespace IO::Streams;
using namespace Logger;
using namespace Memory;
using namespace Objects;

namespace Sounds
{
    SoundSampleContainer SoundSampleState;

    // 0x005ba700
    SoundSample* ConstructSoundSample(SoundSample* self)
    {
        ZeroMemory(&self->Descriptor, sizeof(SoundSampleDescriptor));

        self->Descriptor.ReferenceDistance = 20.0f; // TODO constant

        self->Descriptor.MinimumDistance = *SoundState._UnknownSoundEffectValue1;
        self->Descriptor.MaximumDistance = 10000.0f; // TODO constant

        ConstructInStreamFile(&self->Stream);

        self->Descriptor.AllocatedMemory1 = NULL;

        self->Descriptor.CacheControl = SoundCacheMode::Normal;
        self->Descriptor.Priority = 0;
        self->Descriptor.Offset = 0;

        self->Unk6 = 0;
        self->Unk7 = -1; // TODO constant
        self->Decoder = NULL;

        self->Stream.File.Handle = NULL;

        self->Length = 0;
        self->Unk9 = 0;

        return self;
    }

    // 0x005ba770
    void* ReleaseSoundSample(SoundSample* self, const ReleaseMode mode)
    {
        DisposeSoundSample(self);

        ReleaseInStreamFile(&self->Stream, ReleaseMode::None);

        return self;
    }

    // 0x005ba910
    // a.k.a. freeMemory
    void DisposeSoundSample(SoundSample* self)
    {
        if (self->Descriptor.Priority != 0)
        {
            LogError("Unable to release sound effect sample, it is currently in use.");
        }

        UnlockSoundSample(self);
        ReleaseSoundSampleMemory(self);
        ReleaseSoundDeviceControllerSoundSample(self);

        if (self->Decoder != NULL)
        {
            // TODO ReleaseMemory2(ReleaseSoundDecoder(self->Decoder, ReleaseMode::None));
        }

        self->Decoder = NULL;

        CloseInStreamFile(&self->Stream);

        self->Descriptor.Definition.Name[0] = NULL;
        self->Descriptor.CacheControl = SoundCacheMode::Normal;
        self->Unk6 = 0;
        self->Length = 0;
        self->Unk9 = 0;
        self->Unk7 = -1; // TODO constant
    }

    // 0x005baa20
    void ReleaseSoundSampleMemory(SoundSample* self)
    {
        if (self->Descriptor.AllocatedMemory1 == NULL) { return; }

        ReleaseMemory1(self->Descriptor.AllocatedMemory1);

        self->Descriptor.AllocatedMemory1 = NULL;
    }

    // 0x005baa50
    // a.k.a. lock
    void* LockSoundSample(SoundSample* self, const s32 offset, const s32 length)
    {
        if (self->Lock.Length != 0) { LogError("Unable to lock sound sample, it is already locked."); }

        if (offset < 0 || (self->Length < offset + length)) { LogError("Unable to lock sound sample, provided region is invalid."); }

        void* result = NULL;

        if (self->Descriptor.Offset == 0)
        {
            if (self->Descriptor.AllocatedMemory1 == NULL) { LogError("Unable to lock sound sample, memory buffer is not allocated."); }

            result = (void*)((addr)self->Descriptor.AllocatedMemory1 + (addr)AcquireSoundSampleDescriptorOffset(&self->Descriptor, offset));
        }
        else
        {
            if (*SoundState._SoundDeviceController == NULL) { return NULL; }

            result = (*SoundState._SoundDeviceController)->Self->LockSoundSample(*SoundState._SoundDeviceController,
                self->Descriptor.Offset, offset, length);
        }

        if (result != NULL)
        {
            self->Lock.Offset = offset;
            self->Lock.Length = length;
        }

        return result;
    }

    // 0x005bab50
    void UnlockSoundSample(SoundSample* self)
    {
        if (self->Lock.Length == NULL) { return; }

        if (self->Descriptor.Offset != 0 && *SoundState._SoundDeviceController != NULL)
        {
            (*SoundState._SoundDeviceController)->Self->UnlockSoundSample(*SoundState._SoundDeviceController,
                self->Descriptor.Offset, self->Lock.Offset, self->Lock.Length);
        }

        self->Lock.Offset = 0;
        self->Lock.Length = 0;
    }
}