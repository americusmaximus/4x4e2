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
#include "Mathematics.Basic.hxx"
#include "Memory.hxx"
#include "Sounds.Effects.hxx"
#include "Sounds.hxx"
#include "Sounds.Samples.hxx"
#include "Strings.hxx"

#include <math.h>

using namespace Assets::Sounds;
using namespace IO::Streams;
using namespace Logger;
using namespace Mathematics;
using namespace Memory;
using namespace Objects;
using namespace Strings;

namespace Sounds
{
    SoundSampleContainer SoundSampleState;

    // 0x005ba700
    SoundSample* ConstructSoundSample(SoundSample* self)
    {
        ConstructSoundSampleDescriptor(&self->Descriptor);

        ConstructInStreamFile(&self->Stream);

        self->Descriptor.AllocatedMemory1 = NULL;

        self->Descriptor.CacheControl = SoundCacheMode::Normal;

        self->ReferenceCount = 0;
        self->Index = 0;

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
        if (self->ReferenceCount != 0) { LogError("Unable to release sound effect sample, it is currently in use."); }

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

        if (self->Index == 0)
        {
            if (self->Descriptor.AllocatedMemory1 == NULL) { LogError("Unable to lock sound sample, memory buffer is not allocated."); }

            result = (void*)((addr)self->Descriptor.AllocatedMemory1 + (addr)AcquireSoundSampleDescriptorOffset(&self->Descriptor, offset));
        }
        else
        {
            if (*SoundState._SoundDeviceController == NULL) { return NULL; }

            result = (*SoundState._SoundDeviceController)->Self->LockSoundSample(*SoundState._SoundDeviceController, self->Index, offset, length);
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

        if (self->Index != 0 && *SoundState._SoundDeviceController != NULL)
        {
            (*SoundState._SoundDeviceController)->Self->UnlockSoundSample(*SoundState._SoundDeviceController, self->Index, self->Lock.Offset, self->Lock.Length);
        }

        self->Lock.Offset = 0;
        self->Lock.Length = 0;
    }

    // 0x005b8840
    // INLINE
    SoundSample* AcquireCurrentSoundEffectSample(void)
    {
        for (u32 x = 0; x < 64; x++) // TODO constant
        {
            *SoundState._SoundEffectIndex = *SoundState._SoundEffectIndex + 1;

            if (63 < *SoundState._SoundEffectIndex) { *SoundState._SoundEffectIndex = 0; } // TODO constant

            if (SoundState._SoundEffectSamples[*SoundState._SoundEffectIndex].Descriptor.CacheControl == SoundCacheMode::Normal
                && SoundState._SoundEffectSamples[*SoundState._SoundEffectIndex].ReferenceCount == 0
                && SoundState._SoundEffectSamples[*SoundState._SoundEffectIndex].Unk6 == 0
                && SoundState._SoundEffectSamples[*SoundState._SoundEffectIndex].Lock.Length == 0)
            {
                return &SoundState._SoundEffectSamples[*SoundState._SoundEffectIndex];
            }
        }

        return NULL;
    }

    // 0x005bea70
    void DisposeNamedSoundSample(const char* name, const BOOL dispose)
    {
        LockSounds();

        if (dispose) { DisposeNamedSoundEffect(name); }

        for (u32 x = 0; x < 64; x++) // TODO constant
        {
            if (SoundState._SoundEffectSamples[x].ReferenceCount == 0)
            {
                auto sample = &SoundState._SoundEffectSamples[x];

                if (EqualStrings(name, sample->Descriptor.Definition.Name))
                {
                    DisposeSoundSample(sample);
                }
            }
        }

        UnlockSound1();
    }

    // 0x005bcf80
    f64 AcquireSoundSamplePosition(SoundSample* self, const f64 position, const SoundSeek mode)
    {
        auto value = CalculateSoundSampleDescriptorPosition(&self->Descriptor, position, mode, SoundSeek::End);

        if (AcquireUnknownSoundSampleDescriptorValue1(&self->Descriptor) == 0) // TODO constant
        {
            value = Clamp(value, 0.0, 1.0); // TODO constant
        }
        else
        {
            value = value - AcquireUnknownSoundValue101(value);
        }

        return CalculateSoundSampleDescriptorPosition(&self->Descriptor, value, SoundSeek::End, mode);
    }

    // 0x005babb0
    // a.k.a. seek
    void SeekSoundSample(SoundSample* self, const s32 src, const s32 dst)
    {
        if (self->Unk7 < 0) { LogError("Unable to seek sound sample, %s is not streamed.", self->Descriptor.Definition.Name); } // tODO constant

        self->Unk10 = (s32)round(AcquireSoundSamplePosition(self, (f64)src, SoundSeek::Set));

        if (self->Unk10 < 0) { self->Unk10 = 0; }

        if (dst < 0 || self->Length <= dst) { LogError("Unable to seek sound sample, invalid destination position."); }

        self->Position = dst;

        if (self->Decoder != NULL)
        {
            if (!FUN_004aabe0(self->Decoder, self->Unk10)) { LogError("Unable to seek sound sample %s to %d.", self->Descriptor.Definition.Name, self->Unk10); }

            return;
        }

        if (!IsInStreamFileAvailable(&self->Stream)) { LogError("Unable to seek sound sample, no MP3 and no WAV for sample '%s'.", self->Descriptor.Definition.Name); }

        SeekInStreamFile(&self->Stream, AcquireSoundSampleDescriptorOffset(&self->Descriptor, self->Unk10) + self->Unk13, StreamSeek::Set);
    }
}