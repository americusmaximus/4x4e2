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
    SoundEffectDescriptor* ConstructSoundEffectDescriptor(SoundEffectDescriptor* self);
    void* ReleaseSoundEffectDescriptor(SoundEffectDescriptor* self, const Objects::ReleaseMode mode);

    struct SoundEffectDescriptorContainer
    {
        // 0x00667a00
        Objects::AbstractObjectInitializer Initializer =
        {
            .Options = 0x20100, // TODO
            .Initialize = (Objects::ABSTRACTOBJECTINITIALIZERINITIALIZE)&ConstructSoundEffectDescriptor,
            .Assign = NULL,
            .Release = (Objects::ABSTRACTOBJECTINITIALIZERRELEASE)&ReleaseSoundEffectDescriptor,
            .Size = sizeof(SoundEffectDescriptor),
            .Name = "$SfxOptions$$"
        };
    };

    extern SoundEffectDescriptorContainer SoundEffectDescriptorState;

    SoundEffect* ConstructSoundEffect(SoundEffect* self);
    void* ReleaseSoundEffect(SoundEffect* self, const Objects::ReleaseMode mode);

    struct SoundEffectContainer
    {
        // 0x006679e0
        Objects::AbstractObjectInitializer Initializer =
        {
            .Options = 0x20100, // TODO
            .Initialize = (Objects::ABSTRACTOBJECTINITIALIZERINITIALIZE)&ConstructSoundEffect,
            .Assign = NULL,
            .Release = (Objects::ABSTRACTOBJECTINITIALIZERRELEASE)&ReleaseSoundEffect,
            .Size = sizeof(SoundEffect),
            .Name = "$SfxSlot$$"
        };
    };

    extern SoundEffectContainer SoundEffectState;

    SoundEffect* AcquireSoundEffect(const u32 indx, const BOOL mode);
    BOOL SelectSoundEffectVolume(const u32 indx, const f32 volume);
    BOOL SelectSoundEffectFrequency(const u32 indx, const f32 hz);
}