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
#include "Sounds.hxx"
#include "Sounds.Effects.hxx"

#include <math.h>

using namespace Logger;
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

        self->Unknown1001 = -1.0f; // TODO constant

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
        if (*SoundState.Lock._Count < 1) { LogError("Sound effect must be locked."); } // TODO constant;

        if (self->AAA31 != 0) // TODO constant
        {
            LogMessage("[SOUND] Releasing sound effect %s.", self->Sample->Descriptor.Definition.Name);
        }

        if (self->UnknownIndex != 0) // TODO constant
        {
            if (*SoundState._SoundDeviceController != NULL)
            {
                (*SoundState._SoundDeviceController)->Self->ReleaseSoundEffect(*SoundState._SoundDeviceController, self);
            }

            self->UnknownIndex = 0; // TODO constant
        }

        self->AAA31 = 0; // TODO constant
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
        if (self->Sample == NULL)
        {
            LogError("Unable to update sound effect playback position, sound sample is missing.");
        }

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

        *SoundState.Effects._Index = indx;

        UnlockSound1();
    }

    // 0x005bb4d0
    void UpdateSoundEffectChannelsPosition(SoundEffect* self)
    {
        for (u32 x = 0; x < *SoundState.Options._ChannelCount; x++)
        {
            const auto dx = self->Descriptor.Location.X - SoundState.Effects.Channels.Position._X[x];
            const auto dy = self->Descriptor.Location.Y - SoundState.Effects.Channels.Position._Y[x];
            const auto dz = self->Descriptor.Location.Z - SoundState.Effects.Channels.Position._Z[x];

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
}