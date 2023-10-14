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
#include "Memory.hxx"
#include "Native.hxx"
#include "Sounds.Controllers.DirectSoundController.hxx"
#include "Sounds.Controllers.hxx"
#include "Sounds.Controllers.WaveIn.hxx"
#include "Sounds.Controllers.WaveOut.hxx"
#include "Sounds.Devices.hxx"
#include "Sounds.Effects.hxx"
#include "Sounds.hxx"
#include "Sounds.Samples.hxx"
#include "Time.hxx"

#include <math.h>

using namespace Logger;
using namespace Memory;
using namespace Native;
using namespace Time;

namespace Sounds
{
    SoundDeviceControllerContainer SoundDeviceControllerState;

    // 0x005bfc50
    SoundMixMode AcquireSoundDeviceControllerMixMode(void)
    {
        return *SoundDeviceControllerState._MixMode;
    }

    // 0x005bdc60
    BOOL AcquireSoundDeviceControllerMuteMode(void)
    {
        return SoundDeviceControllerState.MuteMode;
    }

    // 0x005bdc70
    BOOL SelectSoundDeviceControllerMuteMode(const BOOL mode)
    {
        SoundDeviceControllerState.MuteMode = mode;

        if (!mode) { return mode; }

        ReleaseSoundEffectSamples();

        if (*SoundState._SoundDeviceController != NULL)
        {
            if (!(*SoundState._SoundDeviceController)->Self->Stop(*SoundState._SoundDeviceController)) { return FALSE; }
        }

        *SoundDeviceControllerState._ActiveState = FALSE;

        ReleaseSoundDeviceControllerMemory();

        return TRUE;
    }

    // 0x005bfc40
    BOOL AcquireSoundDeviceControllerActiveState(void)
    {
        return *SoundDeviceControllerState._ActiveState;
    }

    // 0x005bfc30
    BOOL AcquireSoundDeviceControllerState(void)
    {
        return *SoundState._SoundDeviceController != NULL;
    }

    // 0x005bfcb0
    BOOL InitializeSoundDeviceController(void)
    {
        if (!ReleaseSoundDeviceController()) { return FALSE; }

        SelectSoundDevice(*SoundDeviceState._SoundDeviceIndex);

        if (INVALID_SOUND_DEVICE_INDEX < *SoundDeviceState._SoundDeviceIndex)
        {
            const auto type = SoundDeviceState.SoundDevices[*SoundDeviceState._SoundDeviceIndex].Type;
            const auto index = SoundDeviceState.SoundDevices[*SoundDeviceState._SoundDeviceIndex].Index;

            switch (type)
            {
            case SoundDeviceType::Wave: { *SoundState._SoundDeviceController = InitializeSoundWaveOutDeviceController(index); break; }
            case SoundDeviceType::DirectSound: { *SoundState._SoundDeviceController = InitializeSoundDirectSoundDeviceController(index); break; }
            }

            if (*SoundState._SoundDeviceController == NULL) { return FALSE; }

            *SoundDeviceControllerState._MixMode = (*SoundState._SoundDeviceController)->Self->QueryCapabilities(*SoundState._SoundDeviceController);

            return TRUE;
        }

        return FALSE;
    }

    // 0x005bfd60
    BOOL ReleaseSoundDeviceController(void)
    {
        if (!StopSoundDeviceController()) { return FALSE; }

        for (u32 x = 0; x < 64; x++) // TODO constant
        {
            DisposeSoundSample(&SoundState._SoundEffectSamples[x]);
        }

        if (*SoundState._SoundDeviceController != NULL)
        {
            if ((*SoundState._SoundDeviceController)->Self->Release(*SoundState._SoundDeviceController))
            {
                *SoundState._SoundDeviceController = NULL;

                return TRUE;
            }

            return FALSE;
        }

        return TRUE;
    }

    // 0x005b9f10
    void ReleaseSoundDeviceControllerMemory(void)
    {
        if (SoundDeviceControllerState.UnknownMemory1 != NULL)
        {
            ReleaseMemory1(SoundDeviceControllerState.UnknownMemory1);
            SoundDeviceControllerState.UnknownMemory1 = NULL;
        }

        if (SoundDeviceControllerState.UnknownMemory2 != NULL)
        {
            ReleaseMemory1(SoundDeviceControllerState.UnknownMemory2);
            SoundDeviceControllerState.UnknownMemory2 = NULL;
        }

        if (SoundDeviceControllerState.UnknownMemory3 != NULL)
        {
            ReleaseMemory1(SoundDeviceControllerState.UnknownMemory3);
            SoundDeviceControllerState.UnknownMemory3 = NULL;
        }

        for (u32 x = 0; x < 8; x++) // TODO constant
        {
            SoundDeviceControllerState._UnknownArray1[x] = NULL; // TODO
            SoundDeviceControllerState._UnknownArray2[x] = NULL; // TODO
            SoundDeviceControllerState._UnknownArray3[x] = NULL; // TODO
            SoundDeviceControllerState._UnknownArray4[x] = NULL; // TODO
        }

        *SoundDeviceControllerState._Unknown1 = 0; // TODO
        *SoundDeviceControllerState._Unknown2 = 0; // TODO
        *SoundDeviceControllerState._Unknown3 = 0; // TODO
        *SoundDeviceControllerState._Unknown4 = 0; // TODO
    }

    // 0x005bf7d0
    BOOL StopSoundDeviceController(void)
    {
        ReleaseSoundEffectSamples();

        if (*SoundState._SoundDeviceController != NULL)
        {
            if (!(*SoundState._SoundDeviceController)->Self->Stop(*SoundState._SoundDeviceController)) { return FALSE; }
        }

        *SoundDeviceControllerState._ActiveState = FALSE;

        ReleaseSoundDeviceControllerMemory();

        return TRUE;
    }

    // 0x005c0070
    BOOL AcquireSoundRecordingDeviceControllerState(void)
    {
        return *SoundState._SoundRecordingDeviceController != NULL;
    }

    // 0x005bffd0
    BOOL InitializeSoundRecordingDeviceController(void)
    {
        if (!ReleaseSoundRecordingDeviceController()) { return FALSE; }

        SelectSoundRecordingDevice(SoundDeviceState.SoundRecordingDeviceIndex);

        if (INVALID_SOUND_RECORDING_DEVICE_INDEX < SoundDeviceState.SoundRecordingDeviceIndex)
        {
            if (SoundDeviceState.SoundRecordingDevices[SoundDeviceState.SoundRecordingDeviceIndex].Type == SoundDeviceType::Wave)
            {
                *SoundState._SoundRecordingDeviceController = InitializeSoundWaveInDeviceController(SoundDeviceState.SoundRecordingDevices[SoundDeviceState.SoundRecordingDeviceIndex].Index);
            }

            if (*SoundState._SoundRecordingDeviceController != NULL) { return TRUE; }
        }

        return FALSE;
    }

    // 0x005c0030
    BOOL ReleaseSoundRecordingDeviceController(void)
    {
        if (!StopSoundRecordingDeviceController()) { return FALSE; }

        if (*SoundState._SoundRecordingDeviceController != NULL)
        {
            if ((*SoundState._SoundRecordingDeviceController)->Self->Release(*SoundState._SoundRecordingDeviceController))
            {
                *SoundState._SoundRecordingDeviceController = NULL;

                return TRUE;
            }

            return FALSE;
        }

        return TRUE;
    }

    // 0x005ba9d0
    void ReleaseSoundDeviceControllerSoundSample(SoundSample* self)
    {
        if (self->Index == 0) { return; }

        if (*SoundState._SoundDeviceController != NULL)
        {
            (*SoundState._SoundDeviceController)->Self->ReleaseSoundSample(*SoundState._SoundDeviceController, self->Index);
        }

        self->Index = 0;
    }

    // 0x005c0260
    BOOL StopSoundRecordingDeviceController(void)
    {
        LockSounds();

        if (*SoundState._SoundRecordingDeviceController != NULL)
        {
            auto controller = *SoundState._SoundRecordingDeviceController;

            if (!controller->Self->Stop(controller))
            {
                UnlockSound1();

                return FALSE;
            }
        }

        UnlockSound1();

        return TRUE;
    }

    // 0x005c0690
    void PollSoundDeviceController(void)
    {
        if (*SoundState._SoundDeviceController == NULL) { return; }

        if (AcquireSoundDeviceControllerActiveState())
        {
            if (WaitMutex2(SoundState.Lock.Mutex, *SoundState.Thread._TimeValue * 3.0)) // TODO constant
            {
                *SoundState.Lock._Count = *SoundState.Lock._Count + 1;

                (*SoundState._SoundDeviceController)->Self->Poll(*SoundState._SoundDeviceController);

                PollSoundDeviceControllerSoundSamples(FALSE);

                *SoundState.Lock._Count = *SoundState.Lock._Count - 1;

                DisposeMutex(SoundState.Lock.Mutex);
            }
        }
    }

    typedef const BOOL(CDECLAPI* FUN_005BAD50) (SoundSample*, const f32, const f32); // TODO
    static FUN_005BAD50 FUN_005bad50 = (FUN_005BAD50)0x005bad50; // TODO

    // 0x005c1760
    void PollSoundDeviceControllerSoundSamples(const BOOL mode)
    {
        LockSounds();

        f32 start = mode ? 1.8f : 1.7f; // TODO constant
        f32 end = mode ? 2.0f : 1.9f; // TODO constant

        for (u32 x = 0; x < 64; x++) { FUN_005bad50(&SoundState._SoundEffectSamples[x], start, end); } // TODO constant

        UnlockSound1();
    }

    // 0x005c04b0
    void SelectSoundDeviceControllerOptions(void)
    {
        if (*SoundState._SoundDeviceController == NULL) { return; }

        if (AcquireSoundDeviceControllerMixMode() == SoundMixMode::None) { return; }

        for (u32 x = 0; x < 64; x++) // TODO constant
        {
            auto effect = &SoundState.Effects._Cache[x];

            if (effect->UnknownIndex == 0) { continue; } // TODO constant

            if (*SoundDeviceControllerState._MixMode == SoundMixMode::Simple)
            {
                effect->Options = effect->Options | *SoundDeviceControllerState._Options;
            }

            if (effect->Descriptor.Unk30 & 1) // TODO constant
            {
                effect->Options = effect->Options & 0xdffffff9; // TODO constant
            }

            if (effect->Options == 0) { continue; } // TODO constant

            ComputeSoundEffect(effect, MIN_SOUND_VOLUME);

            if (!(*SoundState._SoundDeviceController)->Self->SelectSoundEffectOptions(*SoundState._SoundDeviceController, effect, effect->Options))
            {
                DisposeSoundEffect(effect);
            }
            else if (effect->Options & 0x40000000) // TODO constant
            {
                if ((*SoundState._SoundDeviceController)->Self->StartSoundEffect(*SoundState._SoundDeviceController, effect))
                {
                    effect->Options = effect->Options & 0xbfffffff; // TODO constant
                }
                else
                {
                    DisposeSoundEffect(effect);
                }
            }

        }

        if ((*SoundDeviceControllerState._Options & 2) != 0) // TODO constant
        {
            (*SoundState._SoundDeviceController)->Self->SelectPosition(*SoundState._SoundDeviceController,
                SoundState.Effects.Position.X[0],
                SoundState.Effects.Position.Y[0],
                SoundState.Effects.Position.Z[0]);
        }

        if ((*SoundDeviceControllerState._Options & 0x20000000) != 0) // TODO constant
        {
            (*SoundState._SoundDeviceController)->Self->SelectOrientation(*SoundState._SoundDeviceController,
                SoundState.Effects.Orientation.XYZ.X[0],
                SoundState.Effects.Orientation.XYZ.Y[0],
                SoundState.Effects.Orientation.XYZ.Z[0],
                SoundState.Effects.Orientation.Top.X[0],
                SoundState.Effects.Orientation.Top.Y[0],
                SoundState.Effects.Orientation.Top.Z[0],
                SoundState.Effects.Orientation.Front.X[0],
                SoundState.Effects.Orientation.Front.Y[0],
                SoundState.Effects.Orientation.Front.Z[0]);
        }

        if ((*SoundDeviceControllerState._Options & 4) != 0) // TODO constant
        {
            (*SoundState._SoundDeviceController)->Self->SelectVelocity(*SoundState._SoundDeviceController,
                SoundState.Effects.Velocity.X[0],
                SoundState.Effects.Velocity.Y[0],
                SoundState.Effects.Velocity.Z[0]);
        }

        *SoundDeviceControllerState._Options = 0;  // TODO constant

        (*SoundState._SoundDeviceController)->Self->ApplyOptions(*SoundState._SoundDeviceController);
    }

    // 0x005bf810
    // a.k.a. setSoundOutputMode
    BOOL SelectSoundDeviceControllerSoundMode(const u32 bits, const u32 channels, const u32 hz)
    {
        if (AcquireSoundDeviceControllerActiveState()) { LogError("Unable to select sound output mode, the device is already active."); }

        *SoundState.Options._Bits = bits;
        *SoundState.Options._Channels = channels;
        *SoundState.Options._HZ = hz;

        if (*SoundState._SoundDeviceController != NULL)
        {
            u32 count = 0; // TODO constant

            if (!(*SoundState._SoundDeviceController)->Self->Reset(*SoundState._SoundDeviceController, bits, channels, hz, &count)) { return FALSE; }

            if (0 < count) { AllocateSoundDeviceControllerMixBuffers(count, 4); } // TODO constant
        }

        return TRUE;
    }

    // 0x005bf5d0
    BOOL StartSoundDeviceController(void)
    {
        if (!AcquireSoundDeviceControllerState()) { return FALSE; }

        if (AcquireSoundDeviceControllerMuteMode()) { return StopSoundDeviceController(); }

        if (AcquireSoundDeviceControllerActiveState()) { return TRUE; }

        LockSounds();

        if (SelectSoundDeviceControllerSoundMode(*SoundState.Options._Bits, *SoundState.Options._Channels, *SoundState.Options._HZ))
        {
            if (0 < *SoundDeviceControllerState._Unknown1)
            {
                for (u32 x = 0; x < *SoundState.Options._Channels; x++)
                {
                    ZeroMemory(SoundDeviceControllerState._UnknownArray4[x],
                        *SoundDeviceControllerState._Unknown4 * *SoundDeviceControllerState._Unknown1 * 4); // TODO constant
                }
            }

            *SoundDeviceControllerState._Unknown2 = 0; // TODO constant
            *SoundDeviceControllerState._Unknown3 = 0; // TODO constant

            (*SoundState._SoundDeviceController)->Self->SelectPosition(*SoundState._SoundDeviceController,
                SoundState.Effects.Position.X[SoundState.Effects.Index],
                SoundState.Effects.Position.Y[SoundState.Effects.Index],
                SoundState.Effects.Position.Z[SoundState.Effects.Index]);

            (*SoundState._SoundDeviceController)->Self->SelectOrientation(*SoundState._SoundDeviceController,
                SoundState.Effects.Orientation.XYZ.X[SoundState.Effects.Index],
                SoundState.Effects.Orientation.XYZ.Y[SoundState.Effects.Index],
                SoundState.Effects.Orientation.XYZ.Z[SoundState.Effects.Index],
                SoundState.Effects.Orientation.Top.X[SoundState.Effects.Index],
                SoundState.Effects.Orientation.Top.Y[SoundState.Effects.Index],
                SoundState.Effects.Orientation.Top.Z[SoundState.Effects.Index],
                SoundState.Effects.Orientation.Front.X[SoundState.Effects.Index],
                SoundState.Effects.Orientation.Front.Y[SoundState.Effects.Index],
                SoundState.Effects.Orientation.Front.Z[SoundState.Effects.Index]);

            (*SoundState._SoundDeviceController)->Self->SelectVelocity(*SoundState._SoundDeviceController,
                SoundState.Effects.Velocity.X[SoundState.Effects.Index],
                SoundState.Effects.Velocity.Y[SoundState.Effects.Index],
                SoundState.Effects.Velocity.Z[SoundState.Effects.Index]);

            (*SoundState._SoundDeviceController)->Self->SelectDistanceFactor(*SoundState._SoundDeviceController, SoundDeviceControllerState.DistanceFactor.Value);

            (*SoundState._SoundDeviceController)->Self->SelectEnvironment(*SoundState._SoundDeviceController,
                SoundState.Environment.Volume, SoundState.Environment.Time, SoundState.Environment.Damping);

            *SoundState._SoundTime1 = AcquireTime();

            if ((*SoundState._SoundDeviceController)->Self->Start(*SoundState._SoundDeviceController))
            {
                *SoundDeviceControllerState._ActiveState = TRUE;

                UnlockSound1();

                return TRUE;
            }
        }

        UnlockSound1();

        return FALSE;
    }

    // 0x005be620
    void SelectSoundDeviceControllerVelocity(const f32 x, const f32 y, const f32 z)
    {
        LockSounds();

        SoundState.Effects.Velocity.X[SoundState.Effects.Index] = x;
        SoundState.Effects.Velocity.Y[SoundState.Effects.Index] = y;
        SoundState.Effects.Velocity.Z[SoundState.Effects.Index] = z;

        *SoundDeviceControllerState._Options = *SoundDeviceControllerState._Options | 4; // TODO constant

        UnlockSound1();
    }

    // 0x005be5a0
    void SelectSoundDeviceControllerOrientation(const f32 x, const f32 y, const f32 z, const f32 xt, const f32 yt, const f32 zt, const f32 xf, const f32 yf, const f32 zf)
    {
        LockSounds();

        SoundState.Effects.Orientation.XYZ.X[SoundState.Effects.Index] = x;
        SoundState.Effects.Orientation.XYZ.Y[SoundState.Effects.Index] = y;
        SoundState.Effects.Orientation.XYZ.Z[SoundState.Effects.Index] = z;

        SoundState.Effects.Orientation.Top.X[SoundState.Effects.Index] = xt;
        SoundState.Effects.Orientation.Top.Y[SoundState.Effects.Index] = yt;
        SoundState.Effects.Orientation.Top.Z[SoundState.Effects.Index] = zt;

        SoundState.Effects.Orientation.Front.X[SoundState.Effects.Index] = xf;
        SoundState.Effects.Orientation.Front.Y[SoundState.Effects.Index] = yf;
        SoundState.Effects.Orientation.Front.Z[SoundState.Effects.Index] = zf;

        *SoundDeviceControllerState._Options = *SoundDeviceControllerState._Options | 0x20000000; // TODO constant

        UnlockSound1();
    }

    // 0x005be540
    void SelectSoundDeviceControllerPosition(const f64 x, const f64 y, const f64 z)
    {
        LockSounds();

        SoundState.Effects.Position.X[SoundState.Effects.Index] = x;
        SoundState.Effects.Position.Y[SoundState.Effects.Index] = y;
        SoundState.Effects.Position.Z[SoundState.Effects.Index] = z;

        *SoundDeviceControllerState._Options = *SoundDeviceControllerState._Options | 2; // TODO constant

        UnlockSound1();
    }

    // 0x005b9d50
    // a.k.a. allocMixBuffers
    void AllocateSoundDeviceControllerMixBuffers(const u32 count, const s32 value)
    {
        *SoundDeviceControllerState._Unknown4 = 1; // TODO constant
        *SoundDeviceControllerState._Unknown1 = value;

        for (u32 x = 1; *SoundDeviceControllerState._Unknown4 < count; x++)
        {
            *SoundDeviceControllerState._Unknown4 = *SoundDeviceControllerState._Unknown4 * 2; // TODO constant
        }

        *SoundDeviceControllerState._Unknown2 = 0;
        *SoundDeviceControllerState._Unknown3 = 0;

        SoundDeviceControllerState.UnknownMemory1 = ReallocateMemory(SoundDeviceControllerState.UnknownMemory1, *SoundDeviceControllerState._Unknown4 * 4 * value * *SoundState.Options._Channels); // TODO constant
        SoundDeviceControllerState.UnknownMemory2 = ReallocateMemory(SoundDeviceControllerState.UnknownMemory2, 2 * *SoundDeviceControllerState._Unknown4 * 4 * *SoundState.Options._Channels); // TODO constants
        SoundDeviceControllerState.UnknownMemory3 = ReallocateMemory(SoundDeviceControllerState.UnknownMemory3, *SoundDeviceControllerState._Unknown4 * 4 * *SoundState.Options._Channels); // TODO constant

        if (SoundDeviceControllerState.UnknownMemory1 == NULL || SoundDeviceControllerState.UnknownMemory2 == NULL || SoundDeviceControllerState.UnknownMemory3 == NULL) { LogError("Unable to allocate sound mix buffers memory."); }

        for (u32 x = 0; x < *SoundState.Options._Channels; x++)
        {
            SoundDeviceControllerState._UnknownArray4[x] = (void*)((addr)SoundDeviceControllerState.UnknownMemory1 + (addr)(x * *SoundDeviceControllerState._Unknown4 * *SoundDeviceControllerState._Unknown1 * 4)); // TODO constant
            SoundDeviceControllerState._UnknownArray1[x] = (f32*)((addr)SoundDeviceControllerState.UnknownMemory3 + (addr)(x * *SoundDeviceControllerState._Unknown4 * sizeof(f32)));
            SoundDeviceControllerState._UnknownArray2[x] = (void*)((addr)SoundDeviceControllerState.UnknownMemory2 + (addr)(x * *SoundDeviceControllerState._Unknown4 * 4 * 2)); // TODO constant
            SoundDeviceControllerState._UnknownArray3[x] = (void*)((addr)SoundDeviceControllerState.UnknownMemory2 + (addr)(x * *SoundDeviceControllerState._Unknown4 * 4 * 2)); // TODO constant
        }
    }

    typedef const void(CDECLAPI* FUN_005BBD80) (SoundEffect*, SoundEffectMixContainer); // TODO
    static FUN_005BBD80 FUN_005bbd80 = (FUN_005BBD80)0x005bbd80; // TODO

    // 0x005c1340
    void FillSoundDeviceControllerBuffer(void** data, const u32 bits, const u32 channels, const u32 hz, const u32 count, const u32 length)
    {
        if (*SoundState.Lock._Count < DEFAULT_SOUND_LOCK_COUNT) { LogError("Unable to fill sound buffer, sounds must be locked."); }

        for (u32 x = 0; x < channels; x++) { if (SoundDeviceControllerState._UnknownArray4[x] == NULL) { return; } }

        if (bits != *SoundState.Options._Bits || channels != *SoundState.Options._Channels || hz != *SoundState.Options._HZ) { return; }

        ComputeSoundEffectsPositions();

        void* values[MAX_SOUND_CHANNEL_COUNT];

        for (u32 x = 0; x < channels; x++) { values[x] = data[x]; }

        s32 value = 0; // TODO name
        for (u32 x = count; 0 < x; x = x - value) // TODO: make this loop more readable.
        {
            // Check if the next mixing buffer has no data.
            if (*SoundDeviceControllerState._Unknown2 < 1)
            {
                if (*SoundState.Lock._Count < DEFAULT_SOUND_LOCK_COUNT) { LogError("Unable to fill sound channel buffer, sounds must be locked."); }

                if (0 < *SoundDeviceControllerState._Unknown2) { LogError("Next mixing buffer contains data."); }

                {
                    const auto chunk = *SoundDeviceControllerState._Unknown4 * 4; // TODO constant
                    const auto len = chunk * (*SoundDeviceControllerState._Unknown1 - 1); // TODO constant

                    for (u32 xx = 0; xx < *SoundState.Options._Channels; xx++)
                    {
                        memmove(SoundDeviceControllerState._UnknownArray4[xx],
                            (void*)((addr)SoundDeviceControllerState._UnknownArray4[xx] + (addr)chunk), len);

                        ZeroMemory((void*)((addr)SoundDeviceControllerState._UnknownArray4[xx] + (addr)len), chunk);
                    }
                }

                SoundEffectMixContainer container =
                {
                    .Length = *SoundDeviceControllerState._Unknown4,
                    .Channels = *SoundState.Options._Channels,
                    .HZ = *SoundState.Options._HZ
                };

                for (u32 xx = 0; xx < *SoundState.Options._Channels; xx++) { container.Data[xx] = (f32*)SoundDeviceControllerState._UnknownArray4[xx]; }

                for (u32 xx = 0; xx < 64; xx++) { ComputeSoundEffect(&SoundState.Effects._Cache[xx], *SoundDeviceControllerState._Unknown4 / (f32)*SoundState.Options._HZ); }

                for (u32 xx = 0; xx < 64; xx++)
                {
                    // TODO MixSoundEffect(&SoundState.Effects._Cache[xx], container);
                    FUN_005bbd80(&SoundState.Effects._Cache[xx], container);
                } // TODO

                *SoundDeviceControllerState._Unknown2 = *SoundDeviceControllerState._Unknown4;
                *SoundDeviceControllerState._Unknown5 = *SoundDeviceControllerState._Unknown5 + 1;
                *SoundDeviceControllerState._Unknown3 = 0;
            }

            value = *SoundDeviceControllerState._Unknown2;
            if (x < *SoundDeviceControllerState._Unknown2) { value = x; }

            for (u32 xx = 0; xx < channels; xx++)
            {
                auto payload = (f32*)((addr)SoundDeviceControllerState._UnknownArray4[xx] + (addr)(*SoundDeviceControllerState._Unknown3 * 4)); // TODO constants

                ConvertSoundControllerMixBuffer(payload, values[xx], bits, value, length);

                values[xx] = (void*)((addr)values[xx] + (addr)(value * length));
            }

            *SoundDeviceControllerState._Unknown2 = *SoundDeviceControllerState._Unknown2 - value;
            *SoundDeviceControllerState._Unknown3 = *SoundDeviceControllerState._Unknown3 + value;
        }
    }

    // 0x005be660
    void SelectSoundDeviceControllerDistanceFactor(const f32 value)
    {
        SoundDeviceControllerState.DistanceFactor.InverseValue = DEFAULT_DISTANCE_FACTOR_VALUE / value;
        SoundDeviceControllerState.DistanceFactor.Value = value;

        if (*SoundState._SoundDeviceController != NULL)
        {
            (*SoundState._SoundDeviceController)->Self->SelectDistanceFactor(*SoundState._SoundDeviceController, value);
        }
    }

    // 0x005bfde0
    void SelectSoundDeviceControllerEnvironment(const f32 volume, const f32 time, const f32 damping)
    {
        SoundState.Environment.Volume = volume;
        SoundState.Environment.Time = time;
        SoundState.Environment.Damping = damping;

        if (*SoundState._SoundDeviceController != NULL)
        {
            (*SoundState._SoundDeviceController)->Self->SelectEnvironment(*SoundState._SoundDeviceController, volume, time, damping);
        }
    }

    // 0x005ba170
    // a.k.a. convertMixBufToOutput
    void ConvertSoundControllerMixBuffer(const f32* input, void* output, const u32 bits, const u32 count, const u32 pitch)
    {
        if (bits != SOUND_BITS_8 && bits != SOUND_BITS_16) { LogError("Unable to convert mix buffer, invalid bits %d.", bits); } // TODO consts

        if (bits == SOUND_BITS_8) // Convert the float inputs to a range of [0..255].
        {
            for (u32 x = 0; x < count; x++)
            {
                const auto value = input[x];

                auto out = (u8*)((addr)output + (addr)(x * pitch));

                if (value < -1.0f) // TODO constant
                {
                    out[0] = U8_MIN;
                }
                else if (value < 1.0f) // TODO constant
                {
                    out[0] = (u8)round(value * 127.0f + 128.0f); // TODO constants
                }
                else
                {
                    out[0] = U8_MAX - 1;
                }
            }
        }
        else if (bits == SOUND_BITS_16) // Convert the float inputs to a range of [-32767..32767].
        {
            for (u32 x = 0; x < count; x++)
            {
                const auto value = input[x];

                auto out = (s16*)((addr)output + (addr)(x * pitch));

                if (value < -1.0f) // TODO constant
                {
                    out[0] = S16_MIN + 1;
                }
                else if (value < 1.0f) // TODO constant
                {
                    out[0] = (s16)round(value * 32766.0f); // TODO constants
                }
                else
                {
                    out[0] = S16_MAX;
                }
            }
        }
    }

    // 0x005bfdd0
    s32 AcquireSoundDeviceControllerUnknown5(void)
    {
        return *SoundDeviceControllerState._Unknown5;
    }
}