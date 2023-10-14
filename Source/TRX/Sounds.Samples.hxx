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

#include "Objects.hxx"
#include "Sounds.Basic.hxx"

namespace Sounds
{
    SoundSample* ConstructSoundSample(SoundSample* self);
    void* ReleaseSoundSample(SoundSample* self, const Objects::ReleaseMode mode);

    struct SoundSampleContainer
    {
        // 0x00667a20
        Objects::AbstractObjectInitializer Initializer =
        {
            .Options = 0x20100, // TODO
            .Initialize = (Objects::ABSTRACTOBJECTINITIALIZERINITIALIZE)&ConstructSoundSample,
            .Assign = NULL,
            .Release = (Objects::ABSTRACTOBJECTINITIALIZERRELEASE)&ReleaseSoundSample,
            .Size = sizeof(SoundSample),
            .Name = "$SfxSample$$"
        };
    };

    extern SoundSampleContainer SoundSampleState;

    f64 AcquireSoundSamplePosition(SoundSample* self, const f64 position, const SoundSeek mode);
    f64 CalculateSoundSampleDescriptorPosition(SoundSampleDescriptor* self, const f64 position, const SoundSeek src, const SoundSeek dst);
    s32 AcquireSoundSampleDescriptorOffset(SoundSampleDescriptor* self, const s32 offset);
    s32 AcquireSoundSampleDescriptorValue204(SoundSampleDescriptor* self, const s32 offset);
    SoundSample* AcquireCurrentSoundEffectSample(void);
    SoundSampleDescriptor* ConstructSoundSampleDescriptor(SoundSampleDescriptor* self);
    u32 AcquireUnknownSoundSampleDescriptorValue1(SoundSampleDescriptor* self);
    void DisposeNamedSoundSample(const char* name, const BOOL dispose);
    void DisposeSoundSample(SoundSample* self);
    void LoadSoundSampleDescriptor(SoundSampleDescriptor* self);
    void ReleaseSoundSampleMemory(SoundSample* self);
    void SeekSoundSample(SoundSample*, const s32 src, const s32 dst);
    void UnlockSoundSample(SoundSample* self);
    void* LockSoundSample(SoundSample* self, const s32 offset, const s32 length);
}