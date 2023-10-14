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

#include "Logger.hxx"
#include "Mathematics.Basic.hxx"
#include "Sounds.Effects.hxx"
#include "Sounds.hxx"
#include "Sounds.Samples.hxx"
#include "Strings.hxx"

#include <math.h>
#include <stdio.h>

using namespace Logger;
using namespace Mathematics;
using namespace Objects;
using namespace Strings;

namespace Sounds
{
    SoundEffectDescriptorContainer SoundEffectDescriptorState;
    SoundEffectContainer SoundEffectState;

    // 0x005c23f0
    SoundEffectDescriptor* ConstructSoundEffectDescriptor(SoundEffectDescriptor* self)
    {
        InitializeSoundEffectDescriptor(self);

        return self;
    }

    // 0x005c2440
    void* ReleaseSoundEffectDescriptor(SoundEffectDescriptor* self, const ReleaseMode mode) { return self; }

    // 0x005bd0c0
    void InitializeSoundEffectDescriptor(SoundEffectDescriptor* self)
    {
        self->NextChannelIndex = 0;

        self->Location.X = 0.0;
        self->Location.Y = 0.0;
        self->Location.Z = 0.0;

        self->Unknown102.Single = NULL;
        self->Unknown102.Type = SoundEffectDescriptorUnknownType::None;

        self->Unknown104.Single = NULL;
        self->Unknown104.Type = SoundEffectDescriptorUnknownType::None;

        self->Volume = MAX_SOUND_VOLUME;
        self->HZ = 1.0f; // TODO constant

        self->Velocity.X = 0.0f;
        self->Velocity.Y = 0.0f;
        self->Velocity.Z = 0.0f;

        ZeroMemory(&self->UserData, MAX_SOUND_USER_DATA_COUNT * sizeof(void*)); // TODO

        self->Position = 0.0;
        self->Seek = SoundSeek::Set;

        self->Unknown1008 = 0;

        self->RemainingDelay = -1.0f; // TODO constant

        self->MinimumDistance = 20.0f * SoundDeviceControllerState.DistanceFactor.InverseValue; // TODO constant
        self->AAA01 = *SoundState._UnknownSoundEffectValue1 * SoundDeviceControllerState.DistanceFactor.InverseValue;
        self->MaximumDistance = 10000.0f * SoundDeviceControllerState.DistanceFactor.InverseValue; // TODO constant
    }

    // 0x005c2410
    SoundEffect* ConstructSoundEffect(SoundEffect* self)
    {
        InitializeSoundEffectDescriptor(&self->Descriptor);

        return self;
    }

    // 0x005c2430
    void* ReleaseSoundEffect(SoundEffect* self, const ReleaseMode mode) { return self; }

    // 0x005be230
    // a.k.a. getSfxChannelVol
    f32 AcquireSoundEffectChannelVolume(const s32 indx)
    {
        if (indx < MIN_SOUND_EFFECT_CHANNEL_COUNT || (MAX_SOUND_EFFECT_CHANNEL_COUNT - 1) < indx)
        {
            LogError("Unable to acquire sound effect channel volume, invalid index %d.", indx);
        }

        return SoundState.Effects.Channels._Volumes[indx];
    }

    // 0x005be170
    void ReleaseSoundEffectSamples(void)
    {
        LockSounds();

        for (u32 x = 0; x < 64; x++) // TODO constant
        {
            DisposeSoundEffect(&SoundState.Effects._Cache[x]);
        }

        UnlockSound1();
    }

    typedef const BOOL(CDECLAPI* FUN_005B88B0) (const u32); // TODO
    static FUN_005B88B0 FUN_005b88b0 = (FUN_005B88B0)0x005b88b0; // TODO

    // 0x005bc730
    void DisposeSoundEffect(SoundEffect* self)
    {
        if (*SoundState.Lock._Count < DEFAULT_SOUND_LOCK_COUNT) { LogError("Sound effect must be locked."); }

        if (self->DebugMode) { LogMessage("[SOUND] Releasing sound effect %s.", self->Sample->Descriptor.Definition.Name); }

        if (self->UnknownIndex != 0) // TODO constant
        {
            if (*SoundState._SoundDeviceController != NULL)
            {
                (*SoundState._SoundDeviceController)->Self->ReleaseSoundEffect(*SoundState._SoundDeviceController, self);
            }

            self->UnknownIndex = 0; // TODO constant
        }

        self->DebugMode = FALSE;
        self->AAA03 = 0; // TODO constant

        if (self->Sample != NULL)
        {
            if (self->Sample->ReferenceCount < 1) { LogError("Unable to release sound effect %s. Reference count is out of balance.", self->Sample->Descriptor.Definition.Name); }

            self->Sample->ReferenceCount = self->Sample->ReferenceCount - 1;

            auto sample = self->Sample;

            self->Sample = NULL;

            if (-1 < sample->Unk7) // TODO constant
            {
                if (sample->ReferenceCount != 0)
                {
                    LogError("Reference count for sound sample %s is greater than 0.", sample->Descriptor.Definition.Name);
                }

                if (SoundState.Effects._Cache[sample->Index].Descriptor.NextChannelIndex != sample->Unk7)
                {
                    LogError("Streaming sound effect sample index mismatch on %s.", sample->Descriptor.Definition.Name);
                }

                DisposeSoundSample(sample);
            }

            FUN_005b88b0(0); // TODO constant

            sample->Unk7 = -1; // TODO constant
        }

        self->Options = 0; // TODO constant
    }

    // 0x005bc960
    // a.k.a. pollStream
    BOOL PollSoundEffectStream(SoundEffect* self)
    {
        if (self->UnknownIndex != 0 && *SoundState._SoundDeviceController != NULL && self->Sample != NULL)
        {
            const auto position = (*SoundState._SoundDeviceController)->Self->AcquireSoundEffectPosition(*SoundState._SoundDeviceController, self);

            if (position < 0.0)
            {
                LogMessage("[ERROR] [SOUND] Unable to poll hardware playback position %s.", self->Sample->Descriptor.Definition.Name);

                return FALSE;
            }

            UpdateSoundEffectPosition(self, position);

            return TRUE;
        }

        return FALSE;
    }

    // 0x005bc9f0
    // a.k.a. updatePlaybackPos
    void UpdateSoundEffectPosition(SoundEffect* self, const f64 position)
    {
        if (self->Sample == NULL) { LogError("Unable to update sound effect playback position, sound sample is missing."); }

        if (position < 0.0 || self->Sample->Length < position) { LogError("Unable to update sound effect playback position, invalid position provided."); }

        auto delta = position - self->Position;

        if (delta < 0.0) { delta = self->Sample->Length + delta; }

        if (delta < 0.0 || (self->Sample->Length + 0.001 < delta))
        {
            LogError("Unable to update sound effect playback position, invalid step: %f - %f = %f, sample = %d (%s).",
                position, self->Position, delta, self->Sample->Length, self->Sample->Descriptor.Definition.Name);
        }

        if (self->Descriptor.Position != self->Position || position < self->Descriptor.Position)
        {
            self->Descriptor.Position = self->Descriptor.Position + delta;
        }
        else
        {
            self->Descriptor.Position = position;
        }

        if (self->Descriptor.Position < 0.0) { self->Descriptor.Position = 0.0; }

        if (-1 < self->Sample->Descriptor.Definition.Length
            && self->Sample->Descriptor.Definition.Length <= self->Descriptor.Position)
        {
            if (AcquireUnknownSoundSampleDescriptorValue1(&self->Sample->Descriptor) == 0) // TODO constant
            {
                self->Descriptor.Position = self->Sample->Descriptor.Definition.Length;
            }
            else
            {
                self->Descriptor.Position = self->Descriptor.Position
                    - AcquireUnknownSoundValue101(self->Descriptor.Position / self->Sample->Descriptor.Definition.Length) * self->Sample->Descriptor.Definition.Length;
            }
        }

        self->Position = position;
    }

    // 0x005bd560
    // a.k.a. popSfxOptions
    void PopCurrentSoundEffectDescriptor(void)
    {
        *SoundState._SoundEffectDescriptorIndex = *SoundState._SoundEffectDescriptorIndex - 1;

        if (*SoundState._SoundEffectDescriptorIndex < 0) { LogError("Unable to pop sound effect descriptor, the stack is empty."); }
    }

    // 0x005bd4e0
    // a.k.a. pushSfxOptions
    void PushCurrentSoundEffectDescriptor(void)
    {
        *SoundState._SoundEffectDescriptorIndex = *SoundState._SoundEffectDescriptorIndex + 1;

        if (7 < *SoundState._SoundEffectDescriptorIndex) { LogError("Unable to push sound effect descriptor, the stack is full."); } // TODO constant

        CopyMemory(&SoundState._SoundEffectDescriptors[*SoundState._SoundEffectDescriptorIndex],
            &SoundState._SoundEffectDescriptors[*SoundState._SoundEffectDescriptorIndex - 1], sizeof(SoundEffectDescriptor));
    }

    // 0x005ba2f0
    SoundEffect* AcquireSoundEffect(const u32 indx, const BOOL mode)
    {
        const auto todo = indx >> 6; // TODO constant
        const auto index = indx & 0x3f; // TODO constant

        if (todo != 0 && todo < 0xffffff && index < 64) // TODO constant
        {
            auto effect = &SoundState.Effects._Cache[index];

            if (effect->AAA03 == todo)
            {
                if (mode)
                {
                    LockSounds();

                    if (effect->AAA03 != todo || effect->Sample == NULL)
                    {
                        UnlockSound1();

                        return NULL;
                    }

                    if (effect->UnknownIndex != 0)
                    {
                        if (*SoundState._SoundDeviceController == NULL)
                        {
                            DisposeSoundEffect(effect);
                            UnlockSound1();

                            return NULL;
                        }

                        if (effect->Options & 0x40000000) { return effect; } // TODO constant

                        if (!(*SoundState._SoundDeviceController)->Self->AcquireSoundEffectState(*SoundState._SoundDeviceController, effect))
                        {
                            DisposeSoundEffect(effect);
                            UnlockSound1();

                            return NULL;
                        }
                    }
                }

                return effect;
            }
        }

        return NULL;
    }

    // 0x005bdfc0
    BOOL SelectSoundEffectVolume(const u32 indx, const f32 volume)
    {
        auto effect = AcquireSoundEffect(indx, TRUE);

        if (effect == NULL) { return FALSE; }

        effect->Descriptor.Volume = volume;
        effect->Options = effect->Options | 8; // TODO constant

        UnlockSound1();

        return TRUE;
    }

    // 0x005bdff0
    BOOL SelectSoundEffectFrequency(const u32 indx, const f32 hz)
    {
        auto effect = AcquireSoundEffect(indx, TRUE);

        if (effect == NULL) { return FALSE; }

        effect->Descriptor.HZ = hz;
        effect->Options = effect->Options | 0x10; // TODO constant

        if (effect->UnknownIndex != 0 && *SoundState._SoundDeviceController != NULL)
        {
            (*SoundState._SoundDeviceController)->Self->SelectSoundEffectOptions(*SoundState._SoundDeviceController, effect, 0x10); // TODO constant
        }

        UnlockSound1();

        return TRUE;
    }

    // 0x005be530
    void SelectSoundEffectIndex(const s32 indx)
    {
        LockSounds();

        SoundState.Effects.Index = indx;

        UnlockSound1();
    }

    // 0x005bb4d0
    void ComputeSoundEffectChannelsPosition(SoundEffect* self) // TODO: Distance?
    {
        for (u32 x = 0; x < *SoundState.Options._Channels; x++)
        {
            const auto dx = self->Descriptor.Location.X - SoundState.Effects.Channels.Position.X[x];
            const auto dy = self->Descriptor.Location.Y - SoundState.Effects.Channels.Position.Y[x];
            const auto dz = self->Descriptor.Location.Z - SoundState.Effects.Channels.Position.Z[x];

            self->AAA04[x] = (f32)sqrt(dx * dx + dy * dy + dz * dz);
        }
    }

    // 0x005bd380
    // a.k.a. setNextSfxChannel
    void SelectNextSoundEffectDescriptorChannel(const s32 indx)
    {
        if (indx < MIN_SOUND_EFFECT_CHANNEL_COUNT || (MAX_SOUND_EFFECT_CHANNEL_COUNT - 1) < indx) { LogError("Unable to select sound effect channel, invalid index %d.", indx); }

        SoundState._SoundEffectDescriptors[*SoundState._SoundEffectDescriptorIndex].NextChannelIndex = indx;
    }

    // 0x005bd2f0
    void SelectCurrentSoundEffectDescriptorVolume(const f32 volume)
    {
        SoundState._SoundEffectDescriptors[*SoundState._SoundEffectDescriptorIndex].Volume = volume;
    }

    // 0x005bd410
    void UpdateCurrentSoundEffectDescriptorUnk30(const u32 mode) // TODO name, enum
    {
        SoundState._SoundEffectDescriptors[*SoundState._SoundEffectDescriptorIndex].Unk30 = SoundState._SoundEffectDescriptors[*SoundState._SoundEffectDescriptorIndex].Unk30 | mode;
    }

    // 0x005bd3f0
    void SelectCurrentSoundEffectDescriptorUnk30(const u32 mode) // TODO name, enum
    {
        SoundState._SoundEffectDescriptors[*SoundState._SoundEffectDescriptorIndex].Unk30 = mode;
    }

    // 0x005bd430
    void RemoveCurrentSoundEffectDescriptorUnk30(const u32 mode) // TODO name, enum
    {
        SoundState._SoundEffectDescriptors[*SoundState._SoundEffectDescriptorIndex].Unk30 = SoundState._SoundEffectDescriptors[*SoundState._SoundEffectDescriptorIndex].Unk30 & (~mode);
    }

    // 0x005bd4c0
    // INLINE
    void InitializeCurrentSoundEffectDescriptor(void)
    {
        InitializeSoundEffectDescriptor(&SoundState._SoundEffectDescriptors[*SoundState._SoundEffectDescriptorIndex]);
    }

    // 0x005bd190
    void SelectCurrentSoundEffectDescriptorLocation(const f64 x, const f64 y, const f64 z)
    {
        SoundState._SoundEffectDescriptors[*SoundState._SoundEffectDescriptorIndex].Location.X = x;
        SoundState._SoundEffectDescriptors[*SoundState._SoundEffectDescriptorIndex].Location.Y = y;
        SoundState._SoundEffectDescriptors[*SoundState._SoundEffectDescriptorIndex].Location.Z = z;

        SoundState._SoundEffectDescriptors[*SoundState._SoundEffectDescriptorIndex].Unknown102.Single = NULL;
        SoundState._SoundEffectDescriptors[*SoundState._SoundEffectDescriptorIndex].Unknown104.Type = SoundEffectDescriptorUnknownType::None;
    }

    // 0x005bd250
    void SelectCurrentSoundEffectDescriptorVelocity(const f32 x, const f32 y, const f32 z)
    {
        SoundState._SoundEffectDescriptors[*SoundState._SoundEffectDescriptorIndex].Velocity.X = x;
        SoundState._SoundEffectDescriptors[*SoundState._SoundEffectDescriptorIndex].Velocity.Y = y;
        SoundState._SoundEffectDescriptors[*SoundState._SoundEffectDescriptorIndex].Velocity.Z = z;

        SoundState._SoundEffectDescriptors[*SoundState._SoundEffectDescriptorIndex].Unknown104.Single = NULL;
        SoundState._SoundEffectDescriptors[*SoundState._SoundEffectDescriptorIndex].Unknown104.Type = SoundEffectDescriptorUnknownType::None;
    }

    // 0x005bd220
    void SelectCurrentSoundDescriptorUnknownValues102103(f64x3* value)
    {
        SoundState._SoundEffectDescriptors[*SoundState._SoundEffectDescriptorIndex].Unknown102.Type = SoundEffectDescriptorUnknownType::Double;
        SoundState._SoundEffectDescriptors[*SoundState._SoundEffectDescriptorIndex].Unknown102.Double = value;
    }

    // 0x005bd1f0
    void SelectCurrentSoundDescriptorUnknownValues103102(f32x3* value)
    {
        SoundState._SoundEffectDescriptors[*SoundState._SoundEffectDescriptorIndex].Unknown102.Type = SoundEffectDescriptorUnknownType::Single;
        SoundState._SoundEffectDescriptors[*SoundState._SoundEffectDescriptorIndex].Unknown102.Single = value;
    }

    // 0x005bd460
    void SelectCurrentSoundEffectDistances(const f32 min, const f32 ref, const f32 max)
    {
        SoundState._SoundEffectDescriptors[*SoundState._SoundEffectDescriptorIndex].MinimumDistance = min;
        SoundState._SoundEffectDescriptors[*SoundState._SoundEffectDescriptorIndex].AAA01 = ref;
        SoundState._SoundEffectDescriptors[*SoundState._SoundEffectDescriptorIndex].MaximumDistance = max;
    }

    // 0x005bd290
    void SelectCurrentSoundDescriptorUnknownValues105104(f32x3* value)
    {
        SoundState._SoundEffectDescriptors[*SoundState._SoundEffectDescriptorIndex].Unknown104.Type = SoundEffectDescriptorUnknownType::Single;
        SoundState._SoundEffectDescriptors[*SoundState._SoundEffectDescriptorIndex].Unknown104.Single = value;
    }

    // 0x005bd2c0
    // INLINE
    void SelectCurrentSoundDescriptorUnknownValues104105(f64x3* value)
    {
        SoundState._SoundEffectDescriptors[*SoundState._SoundEffectDescriptorIndex].Unknown104.Type = SoundEffectDescriptorUnknownType::Double;
        SoundState._SoundEffectDescriptors[*SoundState._SoundEffectDescriptorIndex].Unknown104.Double = value;
    }

    // 0x005bd310
    void SelectCurrentSoundEffectDescriptorFrequency(const f32 value)
    {
        SoundState._SoundEffectDescriptors[*SoundState._SoundEffectDescriptorIndex].HZ = value;
    }

    // 0x005bd3d0
    // INLINE
    void SelectCurrentSoundEffectDescriptorRemainingDelay(const f32 value)
    {
        SoundState._SoundEffectDescriptors[*SoundState._SoundEffectDescriptorIndex].RemainingDelay = value;
    }

    // 0x005bd330
    // a.k.a. setNextSfxUserData
    void SelectCurrentSoundEffectDescriptorUserData(const s32 indx, void* value)
    {
        if (indx < MIN_SOUND_USER_DATA_COUNT || (MAX_SOUND_USER_DATA_COUNT - 1) < indx) { LogError("Unable to set sound effect user data, invalid index %d.", indx); }

        SoundState._SoundEffectDescriptors[*SoundState._SoundEffectDescriptorIndex].UserData[indx] = value;
    }

    // 0x005bd490
    void SelectCurrentSoundDescriptorPosition(const f64 value, const SoundSeek mode)
    {
        SoundState._SoundEffectDescriptors[*SoundState._SoundEffectDescriptorIndex].Position = value;
        SoundState._SoundEffectDescriptors[*SoundState._SoundEffectDescriptorIndex].Seek = mode;
    }

    // 0x005be280
    // a.k.a. setNumberOfSfxChannels
    void SelectSoundEffectChannelCount(const u32 value)
    {
        if ((MAX_SOUND_EFFECT_CHANNEL_COUNT + 1) <= value) { LogError("Unable to set sound effect channel count, invalid value %d.", value); }

        *SoundState.Effects.Channels._Count = value;
    }

    // 0x005bb8b0
    // a.k.a. compute
    BOOL ComputeSoundEffect(SoundEffect* self, const f32 volume)
    {
        if (*SoundState.Lock._Count < DEFAULT_SOUND_LOCK_COUNT) { LogError("Sound effect must be locked."); }

        if (self->Sample == NULL) { return FALSE; }

        if (self->DebugMode) { LogMessage("[SOUND] [DBG] Sample: %s\n", self->Sample->Descriptor.Definition.Name); }

        if (self->AAA03 == 0 || self->Descriptor.Position < 0.0) // TODO constant
        {
            LogMessage("[SOUND] Releasing sound effect %s\n", self->Sample->Descriptor.Definition.Name);

            DisposeSoundEffect(self);

            return FALSE;
        }

        if (MIN_SOUND_VOLUME < volume && MIN_SOUND_VOLUME <= self->Volume.Current)
        {
            if (self->Volume.Current <= volume)
            {
                if (fpclassify(self->Volume.Maximum) != FP_ZERO)
                {
                    DisposeSoundEffect(self);

                    return FALSE;
                }

                self->Descriptor.Volume = self->Volume.Minimum;
            }
            else
            {
                self->Descriptor.Volume = self->Descriptor.Volume + (volume / self->Volume.Current) * (self->Volume.Minimum - self->Descriptor.Volume);

                self->Volume.Current = self->Volume.Current - volume;
            }

            self->Options = self->Options | 8; // TODO constant
        }

        ComputeSoundEffectLocationVelocity(self);

        SoundState.Effects.Index = UpdateSoundEffectPositionCount(self->Descriptor.Location.X, self->Descriptor.Location.Y, self->Descriptor.Location.Z);

        if (1 < SoundState.UnknownSoundCount1) { ComputeSoundEffectsPositions(); } // TODO constant

        if ((self->Descriptor).RemainingDelay == -1.0) // TODO constant
        {
            if (self->DebugMode) { LogMessage("  auto computing delay...\n"); }

            AutoCalculateSoundEffectRemainingDelay(self);
        }

        if (self->DebugMode) { LogMessage("  delayRemaining = %7.2fs\n", self->Descriptor.RemainingDelay); }

        self->HZ = self->Descriptor.HZ;

        if (self->DebugMode) { LogMessage("  freq = %5.2f\n", self->HZ); }

        if (fpclassify(volume) == FP_ZERO) { ComputeSoundEffectsPositions(); }

        if ((self->Descriptor.Unk30 & 1) == 0) // TODO constant
        {
            const auto dx = self->Descriptor.Location.X - SoundState.Effects.Position.X[SoundState.Effects.Index];
            const auto dy = self->Descriptor.Location.Y - SoundState.Effects.Position.Y[SoundState.Effects.Index];
            const auto dz = self->Descriptor.Location.Z - SoundState.Effects.Position.Z[SoundState.Effects.Index];

            auto distance = sqrt(dx * dx + dy * dy + dz * dz);

            if (self->DebugMode) { LogMessage("  distToEar = %7.2fs\n", distance); }

            const auto vx = SoundState.Effects.Velocity.X[SoundState.Effects.Index] - self->Descriptor.Velocity.X;
            const auto vy = SoundState.Effects.Velocity.Y[SoundState.Effects.Index] - self->Descriptor.Velocity.Y;
            const auto vz = SoundState.Effects.Velocity.Z[SoundState.Effects.Index] - self->Descriptor.Velocity.Z;

            const auto interim = distance <= 0 ? sqrt(vx * vx + vy * vy + vz * vz) : ((dx * vx + dy * vy + dz * vz) / distance);

            const auto doppler = (interim * SoundDeviceControllerState.DistanceFactor.Value + 1116.4) / 1116.4; // TODO constant

            if (self->DebugMode) { LogMessage("  doppler = %5.2f\n", doppler); }

            const auto clamped = Clamp(doppler, 0.25, 4.0); // TODO constants

            if (self->DebugMode) { LogMessage("  doppler (clamped) = %5.2f\n", clamped); }

            self->HZ = self->HZ * (f32)clamped;
        }
        else if (self->DebugMode) { LogMessage("  non spatialized\n"); }

        ComputeSoundEffectChannelsPosition(self);
        ComputeUnknownSoundEffectValue(self);

        if (self->DebugMode) { LogMessage("  vol = %5.2f\n", self->Descriptor.Volume); }

        ComputeSoundEffectPans(self);

        if (self->DebugMode)
        {
            for (u32 x = 0; x < *SoundState.Options._Channels; x++)
            {
                LogMessage("  channelVol[%d] = %5.2f\n", x, self->Sample->Descriptor.Definition.BitsPerSample * self->Pans[x]);
            }
        }

        self->HZ = Clamp(self->HZ, 0.05f, 20.0f); // TODO constants

        if (self->DebugMode) { LogMessage("  effFreq = %5.2f\n", self->HZ); }

        return TRUE;
    }

    // 0x005b9ac0
    void ComputeSoundEffectsPositions(void)
    {
        if (*SoundState.Options._Channels == SOUND_CHANNEL_COUNT_STEREO)
        {
            SoundState.Effects.Channels.Position.X[0] = SoundState.Effects.Position.X[SoundState.Effects.Index] - (SoundState.Effects.Orientation.XYZ.X[SoundState.Effects.Index] * 0.5f * SoundDeviceControllerState.DistanceFactor.InverseValue); // TODO constant
            SoundState.Effects.Channels.Position.Y[0] = SoundState.Effects.Position.Y[SoundState.Effects.Index] - (SoundState.Effects.Orientation.XYZ.Y[SoundState.Effects.Index] * 0.5f * SoundDeviceControllerState.DistanceFactor.InverseValue); // TODO constant
            SoundState.Effects.Channels.Position.Z[0] = SoundState.Effects.Position.Z[SoundState.Effects.Index] - (SoundState.Effects.Orientation.XYZ.Z[SoundState.Effects.Index] * 0.5f * SoundDeviceControllerState.DistanceFactor.InverseValue); // TODO constant

            SoundState.Effects.Channels.Position.X[1] = SoundState.Effects.Position.X[SoundState.Effects.Index] + (SoundState.Effects.Orientation.XYZ.X[SoundState.Effects.Index] * 0.5f * SoundDeviceControllerState.DistanceFactor.InverseValue); // TODO constant
            SoundState.Effects.Channels.Position.Y[1] = SoundState.Effects.Position.Y[SoundState.Effects.Index] + (SoundState.Effects.Orientation.XYZ.Y[SoundState.Effects.Index] * 0.5f * SoundDeviceControllerState.DistanceFactor.InverseValue); // TODO constant
            SoundState.Effects.Channels.Position.Z[1] = SoundState.Effects.Position.Z[SoundState.Effects.Index] + (SoundState.Effects.Orientation.XYZ.Z[SoundState.Effects.Index] * 0.5f * SoundDeviceControllerState.DistanceFactor.InverseValue); // TODO constant

            return;
        }

        for (u32 x = 0; x < *SoundState.Options._Channels; x++)
        {
            SoundState.Effects.Channels.Position.X[x] = SoundState.Effects.Position.X[SoundState.Effects.Index];
            SoundState.Effects.Channels.Position.Y[x] = SoundState.Effects.Position.Y[SoundState.Effects.Index];
            SoundState.Effects.Channels.Position.Z[x] = SoundState.Effects.Position.Z[SoundState.Effects.Index];
        }
    }

    // 0x005bb810
    // a.k.a. autoCalcDelayRemaining
    void AutoCalculateSoundEffectRemainingDelay(SoundEffect* self)
    {
        if (*SoundState.Lock._Count < DEFAULT_SOUND_LOCK_COUNT) { LogError("Sound effect must be locked."); }

        if ((self->Descriptor.Unk30 & 1) != 0) // TODO constant
        {
            self->Descriptor.RemainingDelay = 0.0f;

            return;
        }

        const auto dx = self->Descriptor.Location.X - SoundState.Effects.Position.X[SoundState.Effects.Index];
        const auto dy = self->Descriptor.Location.Y - SoundState.Effects.Position.Y[SoundState.Effects.Index];
        const auto dz = self->Descriptor.Location.Z - SoundState.Effects.Position.Z[SoundState.Effects.Index];

        self->Descriptor.RemainingDelay = (f32)((1.0 / 1116.4) * sqrt(dx * dx + dy * dy + dz * dz)) * SoundDeviceControllerState.DistanceFactor.Value; // TODO constants
    }

    // 0x005be0f0
    BOOL AttemptSoundEffectDispose(const u32 indx)
    {
        auto effect = AcquireSoundEffect(indx, TRUE);

        if (effect == NULL) { return FALSE; }

        DisposeSoundEffect(effect);
        UnlockSound1();

        return TRUE;
    }

    // 0x005bdc10
    BOOL AttemptSoundEffectAcquire(const u32 indx)
    {
        if (AcquireSoundDeviceControllerMixMode() == SoundMixMode::None) { return AcquireSoundEffect(indx, FALSE) != NULL; }

        if (AcquireSoundEffect(indx, TRUE) == NULL) { return FALSE; }

        UnlockSound1();

        return TRUE;
    }

    // 0x005bb6d0
    void ComputeSoundEffectPans(SoundEffect* self)
    {
        if (*SoundState.Lock._Count < DEFAULT_SOUND_LOCK_COUNT) { LogError("Unable to compute sound effect pans, sounds must be locked."); }

        const auto volume = self->Descriptor.Volume * AcquireSoundEffectChannelVolume(self->Descriptor.NextChannelIndex);

        if ((self->Descriptor.Unk30 & 1) == 0)
        {
            auto distance = self->Descriptor.MinimumDistance;

            for (u32 x = 0; x < *SoundState.Options._Channels; x++)
            {
                auto value = Clamp(self->AAA04[x], self->Descriptor.AAA01, self->Descriptor.MaximumDistance);

                if (distance * 0.05f < value) // TODO constant
                {
                    distance = distance / value;
                }
                else
                {
                    distance = 20.0f; // TODO constant
                }

                self->Pans[x] = volume * distance;
            }

            return;
        }

        for (u32 x = 0; x < *SoundState.Options._Channels; x++) { self->Pans[x] = volume; }
    }

    // 0x005bb340
    void ComputeSoundEffectLocationVelocity(SoundEffect* self)
    {
        if (self->Descriptor.Unknown102.Type == SoundEffectDescriptorUnknownType::Single)
        {
            if (self->Descriptor.Unknown102.Single->X != self->Descriptor.Location.X
                || self->Descriptor.Unknown102.Single->Y != self->Descriptor.Location.Y
                || self->Descriptor.Unknown102.Single->Z != self->Descriptor.Location.Z)
            {
                self->Descriptor.Location.X = self->Descriptor.Unknown102.Single->X;
                self->Descriptor.Location.Y = self->Descriptor.Unknown102.Single->Y;
                self->Descriptor.Location.Z = self->Descriptor.Unknown102.Single->Z;

                self->Options = self->Options | 2; // TODO constant
            }
        }
        else if (self->Descriptor.Unknown102.Type == SoundEffectDescriptorUnknownType::Double)
        {
            if (self->Descriptor.Unknown102.Double->X != self->Descriptor.Location.X
                || self->Descriptor.Unknown102.Double->Y != self->Descriptor.Location.Y
                || self->Descriptor.Unknown102.Double->Z != self->Descriptor.Location.Z)
            {
                self->Descriptor.Location.X = (f32)self->Descriptor.Unknown102.Double->X;
                self->Descriptor.Location.Y = (f32)self->Descriptor.Unknown102.Double->Y;
                self->Descriptor.Location.Z = (f32)self->Descriptor.Unknown102.Double->Z;

                // TODO NOT IMPLEMENTED

                self->Options = self->Options | 2; // TODO constant
            }
        }

        if (self->Descriptor.Unknown104.Type == SoundEffectDescriptorUnknownType::Single)
        {
            if (self->Descriptor.Unknown104.Single->X != self->Descriptor.Velocity.X
                || self->Descriptor.Unknown104.Single->Y != self->Descriptor.Velocity.Y
                || self->Descriptor.Unknown104.Single->Z != self->Descriptor.Velocity.Z)
            {
                self->Descriptor.Velocity.X = self->Descriptor.Unknown104.Single->X;
                self->Descriptor.Velocity.Y = self->Descriptor.Unknown104.Single->Y;
                self->Descriptor.Velocity.Z = self->Descriptor.Unknown104.Single->Z;

                self->Options = self->Options | 4; // TODO constant
            }
        }
        else if (self->Descriptor.Unknown104.Type == SoundEffectDescriptorUnknownType::Double)
        {
            if (self->Descriptor.Unknown104.Double->X != self->Descriptor.Velocity.X
                || self->Descriptor.Unknown104.Double->Y != self->Descriptor.Velocity.Y
                || self->Descriptor.Unknown104.Double->Z != self->Descriptor.Velocity.Z)
            {
                self->Descriptor.Velocity.X = (f32)self->Descriptor.Unknown104.Double->X;
                self->Descriptor.Velocity.Y = (f32)self->Descriptor.Unknown104.Double->Y;
                self->Descriptor.Velocity.Z = (f32)self->Descriptor.Unknown104.Double->Z;

                self->Options = self->Options | 4; // TODO constant
            }
        }
    }

    // 0x005bb540
    void ComputeUnknownSoundEffectValue(SoundEffect* self)
    {
        if (*SoundState.Lock._Count < DEFAULT_SOUND_LOCK_COUNT) { LogError("Unable to compute sound effect TODO, sounds must be locked."); }

        if (*SoundState.Options._Channels == SOUND_CHANNEL_COUNT_NONE || *SoundState.Options._Channels == SOUND_CHANNEL_COUNT_MONO)
        {
            self->AAA12[0] = 0; // TODO constant

            return;
        }

        if ((self->Descriptor.Unk30 & 1) == 0 && self->UnknownIndex == 0
            && AcquireSoundDeviceControllerMixMode() == SoundMixMode::None)
        {
            auto min = S32_MAX;

            for (u32 x = 0; x < *SoundState.Options._Channels; x++)
            {
                auto value = Max(0, (s32)round(self->AAA04[x] * (1.0 / 1116.4) * SoundDeviceControllerState.DistanceFactor.Value * *SoundState.Options._HZ)); // TODO constant

                self->AAA12[x] = value;

                if (value < min) { min = value; }
            }

            for (u32 x = 0; x < *SoundState.Options._Channels; x++)
            {
                self->AAA12[x] = self->AAA12[x] - min;

                while (self->AAA12[x] <= *SoundDeviceControllerState._Unknown4)
                {
                    x = x + 1;

                    if (*SoundState.Options._Channels <= x) { return; }
                }

                self->AAA12[x] = *SoundDeviceControllerState._Unknown4;
            }

            return;
        }

        for (u32 x = 0; x < *SoundState.Options._Channels; x++) { self->AAA12[x] = 0; } // TODO constant
    }

    // 0x005be690
    // INLINE
    BOOL AcquireSoundEffectDistanceState(const f64 x, const f64 y, const f64 z, const f32 distance)
    {
        const auto dx = x - SoundState.Effects.Position.X[SoundState.Effects.Index];
        const auto dy = y - SoundState.Effects.Position.Y[SoundState.Effects.Index];
        const auto dz = z - SoundState.Effects.Position.Z[SoundState.Effects.Index];

        return (dx * dx + dy * dy + dz * dz) <= (distance * distance);
    }

    // 0x005be340
    // a.k.a. isSfxChannelEnabled
    BOOL AcquireSoundEffectChannelState(const s32 indx)
    {
        if (indx < MIN_SOUND_EFFECT_CHANNEL_COUNT || (MAX_SOUND_EFFECT_CHANNEL_COUNT - 1) < indx)
        {
            LogError("Unable to acquire sound effect channel state, invalid index %d.", indx);
        }

        return SoundState._SoundChannelStates[indx];
    }

    // 0x005be2c0
    // a.k.a. enableSfxChannel
    void SelectSoundEffectChannelState(const s32 indx, const BOOL state)
    {
        if (indx < MIN_SOUND_EFFECT_CHANNEL_COUNT || (MAX_SOUND_EFFECT_CHANNEL_COUNT - 1) < indx)
        {
            LogError("Unable to select sound effect channel state, invalid index %d.", indx);
        }

        SoundState._SoundChannelStates[indx] = state;

        if (!state)
        {
            LockSounds();

            for (u32 x = 0; x < 64; x++) // TODO constant
            {
                if (indx == SoundState.Effects._Cache[x].Descriptor.NextChannelIndex)
                {
                    DisposeSoundEffect(&SoundState.Effects._Cache[x]);
                }
            }

            UnlockSound1();
        }
    }

    // 0x005be420
    // INLINE
    void AcquireSoundEffectsState(char* buffer)
    {
        LockSounds();

        u32 index = 0;

        for (u32 x = 0; x < 64; x++) // TODO constant
        {
            const auto effect = &SoundState.Effects._Cache[x];

            if (effect->Sample != NULL && effect->AAA03 != 0) // TODO constant
            {
                PollSoundEffectStream(effect);

                index = index + sprintf(&buffer[index], "sfx slot %d: %s, pos %7.1f/%7.1f\n",
                    x, effect->Sample->Descriptor.Definition.Name, effect->Descriptor.Position, (f32)effect->Sample->Descriptor.Definition.Length);
            }
        }

        UnlockSound1();

        buffer[index] = NULL;
    }

    // 0x005be1a0
    // a.k.a. setSfxChannelVol
    void SelectSoundEffectChannelVolume(const s32 indx, const f32 volume)
    {
        if (indx < MIN_SOUND_EFFECT_CHANNEL_COUNT || (MAX_SOUND_EFFECT_CHANNEL_COUNT - 1) < indx)
        {
            LogError("Unable to select sound effect channel volume, invalid index %d.", indx);
        }

        SoundState.Effects.Channels._Volumes[indx] = volume;

        if (*SoundState._SoundDeviceController == NULL) { return; }

        if (AcquireSoundDeviceControllerMixMode() == SoundMixMode::None) { return; }

        LockSounds();

        for (u32 x = 0; x < 64; x++) // TODO constant
        {
            auto effect = &SoundState.Effects._Cache[x];

            if (effect->AAA03 != 0 && effect->UnknownIndex != 0
                && effect->Descriptor.NextChannelIndex == indx) // TODO constant
            {
                effect->Options = effect->Options | 8; // TODO constant
            }
        }

        UnlockSound1();
    }

    // 0x005be700
    // TODO: Name
    u32 UpdateSoundEffectPositionCount(const f64 x, const f64 y, const f64 z)
    {
        if (SoundState.UnknownSoundCount1 < 2) { return 0; } // TODO constant

        auto diff = (x - SoundState.Effects.Position.X[0]) * (x - SoundState.Effects.Position.X[0])
            + (y - SoundState.Effects.Position.Y[0]) * (y - SoundState.Effects.Position.Y[0])
            + (z - SoundState.Effects.Position.Z[0]) * (z - SoundState.Effects.Position.Z[0]);

        u32 result = 0;

        for (u32 xx = 1; xx < SoundState.UnknownSoundCount1; xx++)
        {
            const auto dx = x - SoundState.Effects.Position.X[xx]; // TODO
            const auto dy = y - SoundState.Effects.Position.Y[xx]; // TODO
            const auto dz = z - SoundState.Effects.Position.Z[xx]; // TODO

            const auto delta = dx * dx + dy * dy + dz * dz;

            if (delta < diff)
            {
                diff = delta;

                result = result + 1;
            }
        }

        return result;
    }

    // 0x005bdd90
    // INLINE
    BOOL AcquireSoundEffectDescriptor(const u32 indx, SoundEffectDescriptor* desc)
    {
        auto effect = AcquireSoundEffect(indx, TRUE);

        if (effect == NULL) { return FALSE; }

        ComputeSoundEffect(effect, MIN_SOUND_VOLUME);

        CopyMemory(desc, &effect->Descriptor, sizeof(SoundEffectDescriptor));

        return TRUE;
    }

    // 0x005bea00
    void DisposeNamedSoundEffect(const char* name)
    {
        LockSounds();

        for (u32 x = 0; x < 64; x++) // TODO constant
        {
            auto sample = SoundState.Effects._Cache[x].Sample;

            if (sample != NULL)
            {
                if (EqualStrings(name, sample->Descriptor.Definition.Name))
                {
                    DisposeSoundEffect(&SoundState.Effects._Cache[x]);
                }
            }
        }

        UnlockSound1();
    }

    // 0x005be050
    // a.k.a. setSfxUserData
    BOOL SelectSoundEffectDescriptorUserData(const u32 indx, const s32 index, void* value)
    {
        if (index < MIN_SOUND_USER_DATA_COUNT || (MAX_SOUND_USER_DATA_COUNT - 1) < index) { LogError("Unable to set sound effect user data, invalid index %d.", index); }

        auto effect = AcquireSoundEffect(indx, TRUE);

        if (effect == NULL) { return FALSE; }

        effect->Descriptor.UserData[index] = value;

        return TRUE;
    }

    // 0x005bdc90
    // INLINE
    BOOL AcquireSoundEffectSoundSampleDescriptor(const u32 indx, SoundSampleDescriptor* desc)
    {
        auto effect = AcquireSoundEffect(indx, TRUE);

        if (effect == NULL) { return FALSE; }

        CopyMemory(desc, &effect->Sample->Descriptor, sizeof(SoundSampleDescriptor));

        UnlockSound1();

        return TRUE;
    }
    
    // 0x005be120
    BOOL SelectSoundEffectVolumeValues(const u32 indx, const f32 min, const f32 volume, const u32 max)
    {
        auto effect = AcquireSoundEffect(indx, TRUE);

        if (effect == NULL) { return FALSE; }

        effect->Volume.Minimum = min;
        effect->Volume.Current = volume;
        effect->Volume.Maximum = (f32)max;

        UnlockSound1();

        return TRUE;
    }

    // 0x005be0c0
    BOOL SelectSoundEffectDebugMode(const u32 indx, const BOOL mode)
    {
        auto effect = AcquireSoundEffect(indx, TRUE);

        if (effect == NULL) { return FALSE; }

        effect->DebugMode = mode;

        UnlockSound1();

        return TRUE;
    }

    // 0x005bdf80
    BOOL SelectSoundEffectUnknown104Single(const u32 indx, f32x3* value)
    {
        auto effect = AcquireSoundEffect(indx, TRUE);

        if (effect == NULL) { return FALSE; }

        effect->Descriptor.Unknown104.Single = value;
        effect->Descriptor.Unknown104.Type = SoundEffectDescriptorUnknownType::Single;

        effect->Options = effect->Options | 4; // TODO constant

        UnlockSound1();

        return TRUE;
    }

    // 0x005bdf80
    BOOL SelectSoundEffectUnknown104Double(const u32 indx, f64x3* value)
    {
        auto effect = AcquireSoundEffect(indx, TRUE);

        if (effect == NULL) { return FALSE; }

        effect->Descriptor.Unknown104.Double = value;
        effect->Descriptor.Unknown104.Type = SoundEffectDescriptorUnknownType::Double;

        effect->Options = effect->Options | 4; // TODO constant

        UnlockSound1();

        return TRUE;
    }

    // 0x005bde80
    BOOL SelectSoundEffectUnknown102Single(const u32 indx, f32x3* value)
    {
        auto effect = AcquireSoundEffect(indx, TRUE);

        if (effect == NULL) { return FALSE; }

        effect->Descriptor.Unknown102.Type = SoundEffectDescriptorUnknownType::Single;
        effect->Descriptor.Unknown102.Single = value;

        effect->Options = effect->Options | 2; // TODO constant

        UnlockSound1();

        return TRUE;
    }

    // 0x005bde80
    BOOL SelectSoundEffectUnknown102Double(const u32 indx, f64x3* value)
    {
        auto effect = AcquireSoundEffect(indx, TRUE);

        if (effect == NULL) { return FALSE; }

        effect->Descriptor.Unknown102.Type = SoundEffectDescriptorUnknownType::Double;
        effect->Descriptor.Unknown102.Double = value;

        ComputeSoundEffect(effect, MIN_SOUND_VOLUME);

        if (effect->UnknownIndex != 0 && *SoundState._SoundDeviceController != NULL)
        {
            (*SoundState._SoundDeviceController)->Self->SelectSoundEffectOptions(*SoundState._SoundDeviceController, effect, 2); // TODO constant
        }

        UnlockSound1();

        return TRUE;
    }

    // 0x005bdee0
    BOOL SelectSoundEffectVelocity(const u32 indx, const f32 x, const f32 y, const f32 z)
    {
        auto effect = AcquireSoundEffect(indx, TRUE);

        if (effect == NULL) { return FALSE; }

        effect->Descriptor.Unknown104.Type = SoundEffectDescriptorUnknownType::None;
        effect->Descriptor.Unknown104.Single = NULL;

        effect->Descriptor.Velocity.X = x;
        effect->Descriptor.Velocity.Y = y;
        effect->Descriptor.Velocity.Z = z;

        effect->Options = effect->Options | 4; // TODO constant

        UnlockSound1();

        return TRUE;
    }

    // 0x005bddd0
    BOOL SelectSoundEffectLocation(const u32 indx, const f64 x, const f64 y, const f64 z)
    {
        auto effect = AcquireSoundEffect(indx, TRUE);

        if (effect == NULL) { return FALSE; }

        effect->Descriptor.Unknown102.Type = SoundEffectDescriptorUnknownType::None;
        effect->Descriptor.Unknown102.Single = NULL;

        effect->Descriptor.Location.X = x;
        effect->Descriptor.Location.Y = y;
        effect->Descriptor.Location.Z = z;

        effect->Options = effect->Options | 2; // TODO constant

        UnlockSound1();

        return TRUE;
    }

    // 0x005bdcd0
    f64 CalculateSoundEffectPosition(const u32 indx, const SoundSeek mode)
    {
        auto effect = AcquireSoundEffect(indx, TRUE);

        if (effect == NULL) { return -1.0; } // TODO constant

        if (effect->UnknownIndex != 0 && *SoundState._SoundDeviceController != NULL)
        {
            if (!PollSoundEffectStream(effect))
            {
                UnlockSound1();

                return -1.0; // TODO constant
            }
        }

        const auto result = CalculateSoundSampleDescriptorPosition(&effect->Sample->Descriptor, effect->Descriptor.Position, effect->Descriptor.Seek, mode);

        UnlockSound1();

        return result;
    }

    // 0x005bbd80
    // a.k.a. mix
    // NOTE: Originally the container is being passed by value.
    void MixSoundEffect(SoundEffect* self, SoundEffectMixContainer container) // TODO
    {
        // TODO NOT IMPLEMENTED
    }
}