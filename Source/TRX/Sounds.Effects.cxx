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

#include <math.h>

using namespace Logger;
using namespace Mathematics;
using namespace Objects;

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

        self->Unknown102 = 0;
        self->Unknown103 = 0;
        self->Unknown104 = 0;
        self->Unknown105 = 0;

        self->Volume = 1.0f; // TODO constant
        self->HZ = 1.0f; // TODO constant

        self->Velocity.X = 0.0f;
        self->Velocity.Y = 0.0f;
        self->Velocity.Z = 0.0f;

        ZeroMemory(&self->Unknown1002, sizeof(self->Unknown1002)); // TODO

        self->Unknown1005 = 0.0;

        self->Unknown1007 = 0;
        self->Unknown1008 = 0;

        self->RemainingDelay = -1.0f; // TODO constant

        self->MinimumDistance = 20.0f; // TODO constant
        self->AAA01 = *SoundState._UnknownSoundEffectValue1;
        self->MaximumDistance = 10000.0f; // TODO constant
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
        if (indx < 0 || 31 < indx) // TODO constant
        {
            LogError("Unable to acquire sound effect channel volume, invalid index %d.", indx);
        }

        return SoundState.SFX.ChannelVolumes1[indx];
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
        if (*SoundState.Lock._Count < 1) { LogError("Sound effect must be locked."); } // TODO constant

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
            if (self->Sample->Descriptor.Priority < 1) // TODO constant
            {
                LogError("Unable to release sound effect %s. Reference count is out of balance.",
                    self->Sample->Descriptor.Definition.Name);
            }

            self->Sample->Descriptor.Priority = self->Sample->Descriptor.Priority - 1;

            auto sample = self->Sample;

            self->Sample = NULL;

            if (-1 < sample->Unk7) // TODO constant
            {
                if (sample->Descriptor.Priority != 0)
                {
                    LogError("Reference count for sound sample %s is greater than 0.", sample->Descriptor.Definition.Name);
                }

                if (SoundState.Effects._Cache[sample->Descriptor.Offset].Descriptor.NextChannelIndex != sample->Unk7)
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

        if (position < 0.0 || self->Sample->Length < position)
        {
            LogError("Unable to update sound effect playback position, invalid position provided.");
        }

        auto delta = position - self->Position;

        if (delta < 0.0) { delta = self->Sample->Length + delta; }

        if (delta < 0.0 || (self->Sample->Length + 0.001 < delta))
        {
            LogError("Unable to update sound effect playback position, invalid step: %f - %f = %f, sample = %d (%s).",
                position, self->Position, delta, self->Sample->Length, self->Sample->Descriptor.Definition.Name);
        }

        if (self->Descriptor.Unknown1005 != self->Position
            || position < self->Descriptor.Unknown1005)
        {
            self->Descriptor.Unknown1005 = self->Descriptor.Unknown1005 + delta;
        }
        else
        {
            self->Descriptor.Unknown1005 = position;
        }

        if (self->Descriptor.Unknown1005 < 0.0)
        {
            self->Descriptor.Unknown1005 = 0.0;
        }

        if (-1 < self->Sample->Descriptor.Definition.Length
            && self->Sample->Descriptor.Definition.Length <= self->Descriptor.Unknown1005)
        {
            if (AcquireUnknownSoundSampleDescriptorValue1(&self->Sample->Descriptor) == 0) // TODO constant
            {
                self->Descriptor.Unknown1005 = self->Sample->Descriptor.Definition.Length;
            }
            else
            {
                self->Descriptor.Unknown1005 = self->Descriptor.Unknown1005
                    - AcquireUnknownSoundValue101(self->Descriptor.Unknown1005 / self->Sample->Descriptor.Definition.Length) * self->Sample->Descriptor.Definition.Length;
            }
        }

        self->Position = position;
    }

    // 0x005bd560
    // a.k.a. popSfxOptions
    void PopSoundEffectDescriptor(void)
    {
        *SoundState._SoundEffectDescriptorIndex = *SoundState._SoundEffectDescriptorIndex - 1;

        if (*SoundState._SoundEffectDescriptorIndex < 0)
        {
            LogError("Unable to pop sound effect descriptor, the stack is empty.");
        }
    }

    // 0x005bd4e0
    // a.k.a. pushSfxOptions
    void PushSoundEffectDescriptor(void)
    {
        *SoundState._SoundEffectDescriptorIndex = *SoundState._SoundEffectDescriptorIndex + 1;

        if (7 < *SoundState._SoundEffectDescriptorIndex) // TODO constant
        {
            LogError("Unable to push sound effect descriptor, the stack is full.");
        }

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
    void ComputeSoundEffectChannelsPosition(SoundEffect* self)
    {
        for (u32 x = 0; x < *SoundState.Options._ChannelCount; x++)
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
        if (indx < 0 || 31 < indx) { LogError("Invalid sound channel index %d.", indx); } // TODO constnats

        SoundState._SoundEffectDescriptors[*SoundState._SoundEffectDescriptorIndex].NextChannelIndex = indx;
    }

    // 0x005bd2f0
    void SelectSoundEffectDescriptorVolume(const f32 volume)
    {
        SoundState._SoundEffectDescriptors[*SoundState._SoundEffectDescriptorIndex].Volume = volume;
    }

    // 0x005bd410
    void UpdateSoundEffectDescriptorUnk30(const u32 mode) // TODO name, enum
    {
        SoundState._SoundEffectDescriptors[*SoundState._SoundEffectDescriptorIndex].Unk30 = SoundState._SoundEffectDescriptors[*SoundState._SoundEffectDescriptorIndex].Unk30 | mode;
    }

    // 0x005bd3f0
    void SelectSoundEffectDescriptorUnk30(const u32 mode) // TODO name, enum
    {
        SoundState._SoundEffectDescriptors[*SoundState._SoundEffectDescriptorIndex].Unk30 = mode;
    }

    // 0x005bb8b0
    // a.k.a. compute
    BOOL ComputeSoundEffect(SoundEffect* self, const f32 value)
    {
        if (*SoundState.Lock._Count < 1) { LogError("Sound effect must be locked."); } // TODO constant

        if (self->Sample == NULL) { return FALSE; }

        if (self->DebugMode) { LogMessage("[SOUND] [DBG] Sample: %s\n", self->Sample->Descriptor.Definition.Name); }

        if (self->AAA03 == 0 || self->Descriptor.Unknown1005 < 0.0)
        {
            LogMessage("[SOUND] Releasing sound effect %s\n", self->Sample->Descriptor.Definition.Name);

            DisposeSoundEffect(self);

            return FALSE;
        }

        if (0.0 < value && 0.0 <= self->AAA29)
        {
            if (self->AAA29 <= value)
            {
                if ((self->AAA30 & 0x7fffffffU) != 0) // TODO constant
                {
                    DisposeSoundEffect(self);

                    return FALSE;
                }

                self->Descriptor.Volume = self->AAA28;
            }
            else
            {
                self->Descriptor.Volume = self->Descriptor.Volume + (value / self->AAA29) * (self->AAA28 - self->Descriptor.Volume);

                self->AAA29 = self->AAA29 - value;
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

        if (isfinite(value)) { ComputeSoundEffectsPositions(); }

        if ((self->Descriptor.Unk30 & 1) == 0) // TODO constant
        {
            const auto dx = self->Descriptor.Location.X - SoundState.Effects.Position.X[SoundState.Effects.Index];
            const auto dy = self->Descriptor.Location.Y - SoundState.Effects.Position.Y[SoundState.Effects.Index];
            const auto dz = self->Descriptor.Location.Z - SoundState.Effects.Position.Z[SoundState.Effects.Index];

            auto distance = sqrt(dx * dx + dy * dy + dz * dz);

            if (self->DebugMode) { LogMessage("  distToEar = %7.2fs\n", distance); }

            const auto vx = SoundState.Effects.Velocity._X[SoundState.Effects.Index] - self->Descriptor.Velocity.X;
            const auto vy = SoundState.Effects.Velocity._Y[SoundState.Effects.Index] - self->Descriptor.Velocity.Y;
            const auto vz = SoundState.Effects.Velocity._Z[SoundState.Effects.Index] - self->Descriptor.Velocity.Z;

            const auto interim = distance <= 0 ? sqrt(vx * vx + vy * vy + vz * vz) : ((dx * vx + dy * vy + dz * vz) / distance);

            const auto doppler = (interim + 1116.4) / 1116.4; // TODO constant

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
            for (u32 x = 0; x < *SoundState.Options._ChannelCount; x++)
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
        if (*SoundState.Options._ChannelCount == 2) // TODO constant
        {
            SoundState.Effects.Channels.Position.X[0] = SoundState.Effects.Position.X[SoundState.Effects.Index] - (SoundState.Effects.Orientation.XYZ.X[SoundState.Effects.Index] * 0.5f); // TODO constant
            SoundState.Effects.Channels.Position.Y[0] = SoundState.Effects.Position.Y[SoundState.Effects.Index] - (SoundState.Effects.Orientation.XYZ.Y[SoundState.Effects.Index] * 0.5f); // TODO constant
            SoundState.Effects.Channels.Position.Z[0] = SoundState.Effects.Position.Z[SoundState.Effects.Index] - (SoundState.Effects.Orientation.XYZ.Z[SoundState.Effects.Index] * 0.5f); // TODO constant

            SoundState.Effects.Channels.Position.X[1] = SoundState.Effects.Position.X[SoundState.Effects.Index] + (SoundState.Effects.Orientation.XYZ.X[SoundState.Effects.Index] * 0.5f); // TODO constant
            SoundState.Effects.Channels.Position.Y[1] = SoundState.Effects.Position.Y[SoundState.Effects.Index] + (SoundState.Effects.Orientation.XYZ.Y[SoundState.Effects.Index] * 0.5f); // TODO constant
            SoundState.Effects.Channels.Position.Z[1] = SoundState.Effects.Position.Z[SoundState.Effects.Index] + (SoundState.Effects.Orientation.XYZ.Z[SoundState.Effects.Index] * 0.5f); // TODO constant
        }
        else
        {
            for (u32 x = 0; x < *SoundState.Options._ChannelCount; x++)
            {
                SoundState.Effects.Channels.Position.X[x] = SoundState.Effects.Position.X[SoundState.Effects.Index];
                SoundState.Effects.Channels.Position.Y[x] = SoundState.Effects.Position.Y[SoundState.Effects.Index];
                SoundState.Effects.Channels.Position.Z[x] = SoundState.Effects.Position.Z[SoundState.Effects.Index];
            }
        }
    }

    // 0x005bb810
    // a.k.a. autoCalcDelayRemaining
    void AutoCalculateSoundEffectRemainingDelay(SoundEffect* self)
    {
        if (*SoundState.Lock._Count < 1) { LogError("Sound effect must be locked."); } // TODO constant

        if ((self->Descriptor.Unk30 & 1) != 0) // TODO constant
        {
            self->Descriptor.RemainingDelay = 0.0f;

            return;
        }

        const auto dx = self->Descriptor.Location.X - SoundState.Effects.Position.X[SoundState.Effects.Index];
        const auto dy = self->Descriptor.Location.Y - SoundState.Effects.Position.Y[SoundState.Effects.Index];
        const auto dz = self->Descriptor.Location.Z - SoundState.Effects.Position.Z[SoundState.Effects.Index];

        self->Descriptor.RemainingDelay = (f32)((1.0 / 1116.4) * sqrt(dx * dx + dy * dy + dz * dz)); // TODO constants
    }

    // 0x005bb6d0
    void ComputeSoundEffectPans(SoundEffect* self)
    {
        if (*SoundState.Lock._Count < 1) { LogError("Sound effect must be locked."); } // TODO constant

        const auto volume = self->Descriptor.Volume * AcquireSoundEffectChannelVolume(self->Descriptor.NextChannelIndex);

        if ((self->Descriptor.Unk30 & 1) == 0)
        {
            auto distance = self->Descriptor.MinimumDistance;

            for (u32 x = 0; x < *SoundState.Options._ChannelCount; x++)
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
        }
        else
        {
            for (u32 x = 0; x < *SoundState.Options._ChannelCount; x++) { self->Pans[x] = volume; }
        }
    }

    // 0x005bb340
    void ComputeSoundEffectLocationVelocity(SoundEffect* self)
    {
        if (self->Descriptor.Unknown103 == 1) // TODO constant
        {
            if (self->Descriptor.Unknown102->X != self->Descriptor.Location.X
                || self->Descriptor.Unknown102->Y != self->Descriptor.Location.Y
                || self->Descriptor.Unknown102->Z != self->Descriptor.Location.Z)
            {
                self->Descriptor.Location.X = self->Descriptor.Unknown102->X;
                self->Descriptor.Location.Y = self->Descriptor.Unknown102->Y;
                self->Descriptor.Location.Z = self->Descriptor.Unknown102->Z;

                self->Options = self->Options | 2; // TODO constant
            }
        }
        else if (self->Descriptor.Unknown103 == 2) // TODO constant
        {
            if (self->Descriptor.Unknown102->X != self->Descriptor.Location.X
                || self->Descriptor.Unknown102->Y != self->Descriptor.Location.Y
                || self->Descriptor.Unknown102->Z != self->Descriptor.Location.Z)
            {
                // TODO
                DebugBreak();

                self->Options = self->Options | 2; // TODO constant
            }
        }

        if (self->Descriptor.Unknown105 == 1) // TODO constant
        {
            if (self->Descriptor.Unknown104->X != self->Descriptor.Velocity.X
                || self->Descriptor.Unknown104->Y != self->Descriptor.Velocity.Y
                || self->Descriptor.Unknown104->Z != self->Descriptor.Velocity.Z)
            {
                self->Descriptor.Velocity.X = self->Descriptor.Unknown104->X;
                self->Descriptor.Velocity.Y = self->Descriptor.Unknown104->Y;
                self->Descriptor.Velocity.Z = self->Descriptor.Unknown104->Z;

                self->Options = self->Options | 4; // TODO constant
            }
        }
        else if (self->Descriptor.Unknown105 == 2) // TODO constant
        {
            // TODO NOT IMPLEMENTED
            DebugBreak();

            self->Options = self->Options | 4; // TODO constant
        }
    }

    // 0x005bb540
    void ComputeUnknownSoundEffectValue(SoundEffect* self)
    {
        if (*SoundState.Lock._Count < 1) { LogError("Sound effect must be locked."); } // TODO constant

        if (1 < *SoundState.Options._ChannelCount) // TODO constant
        {
            if ((self->Descriptor.Unk30 & 1) == 0 && self->UnknownIndex == 0
                && AcquireSoundDeviceControllerMixMode() == SoundMixMode::None)
            {
                auto min = S32_MAX;

                for (u32 x = 0; x < *SoundState.Options._ChannelCount; x++)
                {
                    auto value = Max(0, (s32)round(self->AAA04[x] * (1.0 / 1116.4) * *SoundState.Options._HZ)); // TODO constant

                    self->AAA12[x] = value;

                    if (value < min) { min = value; }
                }

                for (u32 x = 0; x < *SoundState.Options._ChannelCount; x++)
                {
                    self->AAA12[x] = self->AAA12[x] - min;

                    while (self->AAA12[x] <= *SoundDeviceControllerState._Unknown4)
                    {
                        x = x + 1;

                        if (*SoundState.Options._ChannelCount <= x) { return; }
                    }

                    self->AAA12[x] = *SoundDeviceControllerState._Unknown4;
                }
            }
            else
            {
                for (u32 x = 0; x < *SoundState.Options._ChannelCount; x++) { self->AAA12[x] = 0; } // TODO constant
            }
        }
        else
        {
            self->AAA12[0] = 0; // TODO constant
        }
    }
}