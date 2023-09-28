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
#include "Native.hxx"
#include "Sounds.Controllers.hxx"
#include "Sounds.hxx"
#include "Time.hxx"

#include <math.h>

using namespace Logger;
using namespace Mathematics;
using namespace Native;
using namespace Time;

namespace Sounds
{
    SoundContainer SoundState;

    // 0x005bfc60
    SoundMixMode AcquireSoundMixMode2(void)
    {
        return SoundState.MixMode;
    }

    // 0x005c0710
    f32 AcquireMaximumSoftWareSoundLatency(void)
    {
        return *SoundState.Options._MaximumSoftWareLatency;
    }

    // 0x005c0720
    // a.k.a. setMaxSwSoundLatency
    void SelectMaximumSoftWareLatency(const f32 value)
    {
        if (!AcquireSoundDeviceControllerActiveState())
        {
            LogError("Unable to select maximum sound latency while sound is active.");
        }

        *SoundState.Options._MaximumSoftWareLatency = Clamp(value, 0.05f, 2.0f); // TODO constants
    }

    // 0x005c0420
    // a.k.a. lockSound
    void LockSounds(void)
    {
        if (SoundState.Lock.Mutex == NULL)
        {
            SoundState.Lock.Mutex = InitializeMutex();

            if (SoundState.Lock.Mutex == NULL) { LogError("Unable to create sound mutex."); }
        }

        WaitMutex(SoundState.Lock.Mutex);

        *SoundState.Lock._Count = *SoundState.Lock._Count + 1;

        if (MAX_SOUND_LOCK_COUNT <= *SoundState.Lock._Count) { LogError("Sound lock state is imbalanced."); }
    }

    // 0x005be4f0
    // a.k.a. unlockSound
    void UnlockSound1(void)
    {
        if (*SoundState.Lock._Count < 1) { LogError("Unable to unlock sound that was not locked."); } // TODO

        UnlockSound3();
    }

    // 0x005be50c
    void UnlockSound2(void)
    {
        *SoundState.Lock._Count = *SoundState.Lock._Count + -1;

        DisposeMutex(SoundState.Lock.Mutex);
    }

    // 0x005be4ff
    void UnlockSound3(void)
    {
        if (*SoundState.Lock._Count != 1) // TODO
        {
            *SoundState.Lock._Count = *SoundState.Lock._Count + -1;

            DisposeMutex(SoundState.Lock.Mutex);

            return;
        }

        SelectSoundDeviceControllerOptions();

        UnlockSound2();
    }

    // 0x005c03a0
    // a.k.a. killSoundThread
    BOOL StopSoundThread(void)
    {
        *SoundState.Thread._IsQuit = TRUE;

        if (!*SoundState.Thread._IsActive) { return TRUE; }

        for (u32 x = 0; x < MAX_SOUND_THREAD_ACTION_ITERATION_COUNT; x++)
        {
            Wait(0.01); // TODO

            if (!*SoundState.Thread._IsActive) { return TRUE; }
        }

        if (!*SoundState.Thread._IsActive) { return TRUE; }

        LogMessage("[ERROR] [SOUND] Unable to stop sound thread.\n");

        return !*SoundState.Thread._IsActive;
    }

    // 0x005c0300
    BOOL RestartSoundThread(const f32 value)
    {
        if (!StopSoundThread()) { return FALSE; }

        *SoundState.Thread._TimeValue = Max(0.002f, value); //TODO constants

        *SoundState.Thread._IsActive = FALSE;
        *SoundState.Thread._IsQuit = FALSE;

        const auto thread = InitializeThread(StartSoundThread, NULL);

        if (thread != NULL)
        {
            if (SelectThreadWeight(thread, ThreadWeight::Max))
            {
                for (u32 x = 0; x < MAX_SOUND_THREAD_ACTION_ITERATION_COUNT; x++)
                {
                    if (*SoundState.Thread._IsActive) { return TRUE; }

                    Wait(0.01); //TODO constant
                }

                return *SoundState.Thread._IsActive;
            }
        }

        return FALSE;
    }

    // 0x005c02b0
    DWORD WINAPI StartSoundThread(LPVOID)
    {
        *SoundState.Thread._IsActive = TRUE;

        LogMessage("[SOUND] Sound thread starting...\n");

        while (!*SoundState.Thread._IsQuit)
        {
            PollSoundDeviceController();

            if (*SoundState.Thread._IsQuit) { break; }

            Wait(*SoundState.Thread._TimeValue);
        }

        LogMessage("[SOUND] Sound thread exiting...\n");

        *SoundState.Thread._IsActive = FALSE;

        return EXIT_SUCCESS;
    }

    // 0x005bf930
    u32 AcquireSoundChannelCount(void)
    {
        AcquireSoundOutputOptions(NULL, SoundState.Options._ChannelCount, NULL);

        return *SoundState.Options._ChannelCount;
    }

    // 0x005bf8b0
    void AcquireSoundOutputOptions(u32* bits, u32* channels, u32* hz)
    {
        if (bits != NULL) { *bits = *SoundState.Options._Bits; }

        if (channels != NULL) { *channels = *SoundState.Options._ChannelCount; }

        if (hz != NULL) { *hz = *SoundState.Options._HZ; }
    }

    // 0x0060fb9f
    f64 AcquireUnknownSoundValue101(const f64 value)
    {
        auto result = 0.0;

        if (modf(value, &result) < 0.0) { result = result - 1.0; }

        return result;
    }

    // 0x005bcdc0
    s32 AcquireSoundSampleDescriptorValue204(SoundSampleDescriptor* self, const s32 offset)
    {
        return (offset << 3) / (self->Definition.Channels * self->Definition.BitsPerSample);
    }

    // 0x005be340
    // a.k.a. isSfxChannelEnabled
    BOOL AcquireSoundEffectChannelState(const s32 indx)
    {
        if (indx < 0 || 31 < indx) // TODO constant
        {
            LogError("Unable to acquire sound effect channel state, invalid index %d.", indx);
        }

        return SoundState._SoundChannelStates[indx];
    }

    // 0x005be2c0
    // a.k.a. enableSfxChannel
    void SelectSoundEffectChannelState(const s32 indx, const BOOL state)
    {
        if (indx < 0 || 31 < indx) // TODO constant
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

    // 0x005be1a0
    // a.k.a. setSfxChannelVol
    void SelectSoundEffectChannelVolume(const s32 indx, const f32 volume)
    {
        if (indx < 0 || 31 < indx) // TODO constant
        {
            LogError("Unable to select sound effect channel volume, invalid index %d.", indx);
        }

        SoundState.SFX.ChannelVolumes1[indx] = volume;

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

    // 0x005bfc70
    // a.k.a. enableHwSoundMixing
    void SelectSoundMixMode(const SoundMixMode mode)
    {
        if (AcquireSoundDeviceControllerState())
        {
            LogError("Unable to select hardware sound mixing mode while device is active.");
        }

        SoundState.MixMode = mode;
    }

    // 0x005be700
    u32 UpdateSoundEffectPositionCount(const f64 x, const f64 y, const f64 z)
    {
        if (SoundState.UnknownSoundCount1 < 2) { return 0; } // TODO constant

        auto diff = (x - SoundState.Effects.Position.X[0]) * (x - SoundState.Effects.Position.X[0])
            + (y - SoundState.Effects.Position.Y[0]) * (y - SoundState.Effects.Position.Y[0])
            + (z - SoundState.Effects.Position.Z[0]) * (z - SoundState.Effects.Position.Z[0]);

        u32 result = 0;

        for (u32 xx = 1; xx < SoundState.UnknownSoundCount1; xx++)
        {
            const auto dx = x - SoundState.Effects.Position.X[xx];
            const auto dy = y - SoundState.Effects.Position.Y[xx];
            const auto dz = z - SoundState.Effects.Position.Z[xx];

            const auto delta = dx * dx + dy * dy + dz * dz;

            if (delta < diff)
            {
                diff = delta;

                result = result + 1;
            }
        }

        return result;
    }

    // 0x005bed90
    void ReleaseSounds(void)
    {
        LockSounds();
        ReleaseSoundEffectSamples();

        for (u32 x = 0; x < 64; x++) { DisposeSoundSample(&SoundState._SoundEffectSamples[x]); } // TODO constant

        UnlockSound1();
    }

    // 0x005be4d0
    // a.k.a. unlockSound
    void UnlockSounds(const s32 value)
    {
        LockSounds();

        SoundState.UnknownSoundCount1 = value;

        if (value < 2) { SoundState.Effects.Index = 0; } // TODO constant

        if (*SoundState.Lock._Count == 0) { LogError("Unable to unlock unlocked sound."); }

        if (*SoundState.Lock._Count != 1)
        {
            *SoundState.Lock._Count = *SoundState.Lock._Count + -1;

            DisposeMutex(SoundState.Lock.Mutex);

            return;
        }

        SelectSoundDeviceControllerOptions();
        UnlockSound2();
    }
}