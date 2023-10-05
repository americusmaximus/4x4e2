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

    BOOL AcquireSoundEffectChannelState(const s32 indx);
    BOOL AcquireSoundEffectDistanceState(const f64 x, const f64 y, const f64 z, const f32 distance);
    BOOL AttemptSoundEffectAcquire(const u32 indx);
    BOOL AttemptSoundEffectDispose(const u32 indx);
    BOOL ComputeSoundEffect(SoundEffect* self, const f32 value);
    BOOL PollSoundEffectStream(SoundEffect* self);
    BOOL SelectSoundEffectFrequency(const u32 indx, const f32 hz);
    BOOL SelectSoundEffectVolume(const u32 indx, const f32 volume);
    f32 AcquireSoundEffectChannelVolume(const s32 indx);
    SoundEffect* AcquireSoundEffect(const u32 indx, const BOOL mode);
    u32 UpdateSoundEffectPositionCount(const f64 x, const f64 y, const f64 z);
    void AutoCalculateSoundEffectRemainingDelay(SoundEffect* self);
    void ComputeSoundEffectChannelsPosition(SoundEffect* self);
    void ComputeSoundEffectLocationVelocity(SoundEffect* self);
    void ComputeSoundEffectPans(SoundEffect* self);
    void ComputeSoundEffectsPositions(void);
    void ComputeUnknownSoundEffectValue(SoundEffect* self); // TODO name
    void DisposeSoundEffect(SoundEffect* self);
    void InitializeCurrentSoundEffectDescriptor(void);
    void InitializeSoundEffectDescriptor(SoundEffectDescriptor* self);
    void MixSoundEffect(SoundEffect* self, SoundEffectMixContainer container); // TODO
    void PopCurrentSoundEffectDescriptor(void);
    void PushCurrentSoundEffectDescriptor(void);
    void ReleaseSoundEffectSamples(void);
    void RemoveCurrentSoundEffectDescriptorUnk30(const u32 mode); // TODO name, enum
    void SelectCurrentSoundDescriptorPosition(const f64 value, const s32 mode); // TODO name, params
    void SelectCurrentSoundDescriptorUnknownValues102103(f64x3* value);
    void SelectCurrentSoundDescriptorUnknownValues103102(f32x3* value);
    void SelectCurrentSoundDescriptorUnknownValues104105(f64x3* value);
    void SelectCurrentSoundDescriptorUnknownValues105104(f32x3* value);
    void SelectCurrentSoundEffectDescriptorFrequency(const f32 value);
    void SelectCurrentSoundEffectDescriptorLocation(const f64 x, const f64 y, const f64 z);
    void SelectCurrentSoundEffectDescriptorRemainingDelay(const f32 value);
    void SelectCurrentSoundEffectDescriptorUnk30(const u32 mode); // TODO name, enum
    void SelectCurrentSoundEffectDescriptorUserData(const s32 indx, void* value);
    void SelectCurrentSoundEffectDescriptorVelocity(const f32 x, const f32 y, const f32 z);
    void SelectCurrentSoundEffectDescriptorVolume(const f32 volume);
    void SelectCurrentSoundEffectDistances(const f32 min, const f32 ref, const f32 max);
    void SelectNextSoundEffectDescriptorChannel(const s32 indx);
    void SelectSoundEffectChannelCount(const u32 value);
    void SelectSoundEffectChannelState(const s32 indx, const BOOL value);
    void SelectSoundEffectChannelVolume(const s32 indx, const f32 volume);
    void SelectSoundEffectIndex(const s32 indx);
    void UpdateCurrentSoundEffectDescriptorUnk30(const u32 mode); // TODO name, enum
    void UpdateSoundEffectPosition(SoundEffect* self, const f64 position);
    void AcquireSoundEffectsState(char* buffer);
}