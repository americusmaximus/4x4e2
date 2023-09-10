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
#include "Sounds.Controllers.WaveOut.hxx"
#include "Sounds.hxx"

#include <math.h>

#define WAVE_FORMAT_NONE 0

using namespace Logger;

namespace Sounds
{
    // 0x005c3310
    AbstractSoundDeviceController* InitializeSoundWaveOutDeviceController(const s32 indx)
    {
        if (!ReleaseSoundWaveOutDeviceController(SoundWaveOutSoundControllerState._Instance)) { return NULL; }

        WAVEOUTCAPSA caps;

        if (waveOutGetDevCapsA(indx, &caps, sizeof(WAVEOUTCAPSA)) != MMSYSERR_NOERROR) { return NULL; }

        *SoundWaveOutSoundControllerState._Index = indx;

        return SoundWaveOutSoundControllerState._Instance;
    }

    // 0x005c2640
    BOOL ReleaseSoundWaveOutDeviceController(AbstractSoundDeviceController* self)
    {
        const auto result = self->Self->Stop(self);

        for (u32 x = 0; x < MAX_SOUND_WAVEOUT_BUFFER_COUNT; x++)
        {
            if (SoundWaveOutSoundControllerState.Memory._Data[x] != NULL)
            {
                GlobalFree(SoundWaveOutSoundControllerState.Memory._Data[x]);
                SoundWaveOutSoundControllerState.Memory._Data[x] = NULL;
            }

            SoundWaveOutSoundControllerState.Buffers._Data[x] = NULL;

            if (SoundWaveOutSoundControllerState.Memory._Headers[x] != NULL)
            {
                GlobalFree(SoundWaveOutSoundControllerState.Memory._Headers[x]);
                SoundWaveOutSoundControllerState.Memory._Headers[x] = NULL;
            }

            SoundWaveOutSoundControllerState.Buffers._Headers[x] = NULL;
        }

        if (*SoundWaveOutSoundControllerState._Device == NULL) { return result; }

        if (waveOutClose(*SoundWaveOutSoundControllerState._Device) != MMSYSERR_NOERROR)
        {
            LogMessage("[SOUND] Unable to perform waveOutClose.\n");

            *SoundWaveOutSoundControllerState._Device = NULL;

            return FALSE;
        }

        *SoundWaveOutSoundControllerState._Device = NULL;

        return result;
    }

    // 0x005c26f0
    BOOL StartSoundWaveOutDeviceController(AbstractSoundDeviceController* self)
    {
        for (u32 x = 0; x < *SoundWaveOutSoundControllerState.Counters._Unknown2; x++)
        {
            ZeroMemory(SoundWaveOutSoundControllerState.Buffers._Headers[x], sizeof(WAVEHDR));

            const auto length = *SoundWaveOutSoundControllerState.Mode.Active._Channels
                * (*SoundWaveOutSoundControllerState.Mode.Active._Bits >> 3)
                * *SoundWaveOutSoundControllerState.Counters._Unknown1;

            SoundWaveOutSoundControllerState.Buffers._Headers[x]->lpData = SoundWaveOutSoundControllerState.Buffers._Data[x];
            SoundWaveOutSoundControllerState.Buffers._Headers[x]->dwBufferLength = length;
            SoundWaveOutSoundControllerState.Buffers._Headers[x]->dwUser = x;

            if (!PlaySoundWaveOutDeviceControllerBuffer(x))
            {
                self->Self->Stop(self);

                return FALSE;
            }
        }

        const auto value = *SoundWaveOutSoundControllerState.Counters._Unknown1
            / (f32)*SoundWaveOutSoundControllerState.Mode.Active._HZ / 3.0f; // TODO constant

        if (RestartSoundThread(value)) { return TRUE; }

        self->Self->Stop(self);

        return FALSE;
    }

    // 0x005c27c0
    BOOL StopSoundWaveOutDeviceController(AbstractSoundDeviceController* self)
    {
        BOOL result = TRUE;

        if (*SoundWaveOutSoundControllerState._Device != NULL)
        {
            if (waveOutReset(*SoundWaveOutSoundControllerState._Device) != MMSYSERR_NOERROR)
            {
                LogMessage("[SOUND] Unable to perform waveOutReset.\n");

                result = FALSE;
            }
        }

        for (u32 x = 0; x < MAX_SOUND_WAVEOUT_BUFFER_COUNT; x++)
        {
            if (SoundWaveOutSoundControllerState.Buffers._Headers[x] != NULL)
            {
                waveOutUnprepareHeader(*SoundWaveOutSoundControllerState._Device,
                    SoundWaveOutSoundControllerState.Buffers._Headers[x], sizeof(WAVEHDR));
            }
        }

        if (StopSoundThread()) { return result; }

        return FALSE;
    }

    // 0x005c2830
    BOOL ResetSoundWaveOutDeviceController(AbstractSoundDeviceController* self, const u32 bits, const u32 channels, const u32 hz, u32* count)
    {
        if (!self->Self->Release(self)) { return FALSE; }

        *SoundWaveOutSoundControllerState.Mode.Active._Bits = bits;
        *SoundWaveOutSoundControllerState.Mode.Active._Channels = channels;
        *SoundWaveOutSoundControllerState.Counters._Unknown2 = 4; // TODO constant
        *SoundWaveOutSoundControllerState.Mode.Active._HZ = hz;

        {
            auto const value = (u32)roundf(AcquireMaximumSoftWareSoundLatency()
                * *SoundWaveOutSoundControllerState.Mode.Active._HZ / (f32)*SoundWaveOutSoundControllerState.Counters._Unknown2);

            *SoundWaveOutSoundControllerState.Counters._Unknown1 = (value + 0xf) & 0xfffffff0; // TODO
        }

        for (u32 x = 0; x < *SoundWaveOutSoundControllerState.Counters._Unknown2; x++)
        {
            const auto length = (*SoundWaveOutSoundControllerState.Mode.Active._Bits >> 3)
                ** SoundWaveOutSoundControllerState.Counters._Unknown1
                * *SoundWaveOutSoundControllerState.Mode.Active._Channels;

            SoundWaveOutSoundControllerState.Memory._Data[x] = GlobalAlloc(GMEM_SHARE | GMEM_MOVEABLE, length);

            if (SoundWaveOutSoundControllerState.Memory._Data[x] == NULL)
            {
                self->Self->Release(self);

                return FALSE;
            }

            SoundWaveOutSoundControllerState.Buffers._Data[x] = (CHAR*)GlobalLock(SoundWaveOutSoundControllerState.Memory._Data[x]);

            if (SoundWaveOutSoundControllerState.Buffers._Data[x] == NULL)
            {
                self->Self->Release(self);

                return FALSE;
            }

            SoundWaveOutSoundControllerState.Memory._Headers[x] = GlobalAlloc(GMEM_SHARE | GMEM_MOVEABLE, sizeof(WAVEHDR));

            if (SoundWaveOutSoundControllerState.Memory._Headers[x] == NULL)
            {
                self->Self->Release(self);

                return FALSE;
            }

            SoundWaveOutSoundControllerState.Buffers._Headers[x] = (WAVEHDR*)GlobalLock(SoundWaveOutSoundControllerState.Memory._Headers[x]);

            if (SoundWaveOutSoundControllerState.Buffers._Headers[x] == NULL)
            {
                self->Self->Release(self);

                return FALSE;
            }
        }

        const auto alignment = ((*SoundWaveOutSoundControllerState.Mode.Active._Bits & 0xffff) >> 3)
            * *SoundWaveOutSoundControllerState.Mode.Active._Channels;

        WAVEFORMATEX format =
        {
            .wFormatTag = WAVE_FORMAT_PCM,
            .nChannels = (WORD)*SoundWaveOutSoundControllerState.Mode.Active._Channels,
            .nSamplesPerSec = *SoundWaveOutSoundControllerState.Mode.Active._HZ,
            .nAvgBytesPerSec = alignment * *SoundWaveOutSoundControllerState.Mode.Active._HZ,
            .nBlockAlign = (WORD)alignment,
            .wBitsPerSample = (WORD)*SoundWaveOutSoundControllerState.Mode.Active._Bits
        };


        if (waveOutOpen(SoundWaveOutSoundControllerState._Device,
            *SoundWaveOutSoundControllerState._Index, &format, NULL, NULL, WAVE_FORMAT_NONE) != MMSYSERR_NOERROR)
        {
            LogMessage("[SOUND] Unable to perform waveOutOpen.\n");

            self->Self->Release(self);

            return FALSE;
        }

        *count = *SoundWaveOutSoundControllerState.Counters._Unknown1;

        return TRUE;
    }

    // 0x005c2a10
    // a.k.a. poll
    BOOL PollSoundWaveOutDeviceController(AbstractSoundDeviceController* self)
    {
        for (u32 x = 0; x < *SoundWaveOutSoundControllerState.Counters._Unknown2; x++)
        {
            if (SoundWaveOutSoundControllerState.Buffers._Headers[x] == NULL
                || SoundWaveOutSoundControllerState.Buffers._Data[x] == NULL)
            {
                LogMessage("[SOUND] Unable to poll device %d, no data available.\n", x);

                return FALSE;
            }

            if (SoundWaveOutSoundControllerState.Buffers._Headers[x]->dwFlags & WHDR_DONE)
            {
                if (!PlaySoundWaveOutDeviceControllerBuffer(x))
                {
                    LogMessage("[SOUND] Unable to poll device %d, play command failed.\n", x);

                    return FALSE;
                }
            }
        }

        return TRUE;
    }

    // 0x005c02a0
    SoundMixMode QuerySoundWaveOutDeviceControllerCapabilities(AbstractSoundDeviceController* self) { return SoundMixMode::None; }

    // 0x005c12f0
    void ApplySoundWaveOutDeviceControllerOptions(AbstractSoundDeviceController* self) {}

    // 0x005c12f0
    void SelectSoundWaveOutDeviceControllerPosition(AbstractSoundDeviceController* self, const f64 x, const f64 y, const f64 z) {}

    // 0x005c12f0
    void SelectSoundWaveOutDeviceControllerOrientation(AbstractSoundDeviceController* self, const f32 x, const f32 y, const f32 z, const f32 xt, const f32 yt, const f32 zt, const f32 xf, const f32 yf, const f32 zf) {}

    // 0x005c12f0
    void SelectSoundWaveOutDeviceControllerVelocity(AbstractSoundDeviceController* self, const f32 x, const f32 y, const f32 z) {}

    // 0x005c12f0
    void SelectSoundWaveOutDeviceControllerDistanceFactor(AbstractSoundDeviceController* self, const f32 value) {}

    // 0x005c12f0
    void SelectSoundWaveOutDeviceControllerEnvironment(AbstractSoundDeviceController* self, const f32 volume, const f32 time, const f32 damping) {}

    // 0x005c02a0
    u32 AllocateSoundWaveOutDeviceControllerSoundSample(AbstractSoundDeviceController* self, const SoundSampleDescriptor* desc) { return 0; } // TODO constant

    // 0x005c12f0
    void ReleaseSoundWaveOutDeviceControllerSoundSample(AbstractSoundDeviceController* self, const s32 indx) {}

    // 0x005c12f0
    void* LockSoundWaveOutDeviceControllerSoundSample(AbstractSoundDeviceController* self, const s32 indx, const s32 offset, const s32 length) { return NULL; }

    // 0x005c12f0
    void UnlockSoundWaveOutDeviceControllerSoundSample(AbstractSoundDeviceController* self, const s32 indx, const s32 offset, const s32 length) {}

    // 0x005c02a0
    u32 AllocateSoundWaveOutDeviceControllerSoundEffect(AbstractSoundDeviceController* self, const s32 indx) { return 0; } // TODO constant

    // 0x005c02a0
    BOOL SelectSoundWaveOutDeviceControllerSoundEffectOptions(AbstractSoundDeviceController* self, SoundEffect* effect, const u32 mode) { return FALSE; } // TODO enum

    // 0x005c1310
    f64 AcquireSoundWaveOutDeviceControllerSoundEffectPosition(AbstractSoundDeviceController* self, SoundEffect* effect) { return -1.0; } // TODO constant

    // 0x005c02a0
    BOOL StartSoundWaveOutDeviceControllerSoundEffect(AbstractSoundDeviceController* self, SoundEffect* effect) { return FALSE; }

    // 0x005c1330
    void ReleaseSoundWaveOutDeviceControllerSoundEffect(AbstractSoundDeviceController* self, SoundEffect* effect) {}

    // 0x005c02a0
    BOOL AcquireSoundWaveOutDeviceControllerSoundEffectState(AbstractSoundDeviceController* self, SoundEffect* effect) { return FALSE; }

    // 0x005c24c0
    // a.k.a. sendWavOutBuffer
    BOOL PlaySoundWaveOutDeviceControllerBuffer(const u32 indx)
    {
        if (SoundWaveOutSoundControllerState.Buffers._Data[indx] == NULL || SoundWaveOutSoundControllerState.Buffers._Headers[indx] == NULL) { return FALSE; }

        if (waveOutUnprepareHeader(*SoundWaveOutSoundControllerState._Device,
            SoundWaveOutSoundControllerState.Buffers._Headers[indx], sizeof(WAVEHDR)) != MMSYSERR_NOERROR)
        {
            LogMessage("[SOUND] Unable to perform waveOutUnprepareHeader.\n");

            return FALSE;
        }

        // TODO: NOT IMPLEMENTED
        // TODO: NOT IMPLEMENTED
        // TODO: NOT IMPLEMENTED
        // TODO: NOT IMPLEMENTED
        // TODO: NOT IMPLEMENTED

        const auto length = (*SoundWaveOutSoundControllerState.Mode.Active._Bits >> 3)
            * *SoundWaveOutSoundControllerState.Counters._Unknown1
            * *SoundWaveOutSoundControllerState.Mode.Active._Channels;

        SoundWaveOutSoundControllerState.Buffers._Headers[indx]->dwBufferLength = length;

        if (waveOutPrepareHeader(*SoundWaveOutSoundControllerState._Device,
            SoundWaveOutSoundControllerState.Buffers._Headers[indx], sizeof(WAVEHDR)) != MMSYSERR_NOERROR)
        {
            LogMessage("[SOUND] Unable to perform waveOutPrepareHeader.\n");

            return FALSE;
        }

        if (waveOutWrite(*SoundWaveOutSoundControllerState._Device,
            SoundWaveOutSoundControllerState.Buffers._Headers[indx], sizeof(WAVEHDR)) != MMSYSERR_NOERROR)
        {
            LogMessage("[SOUND] Unable to perform waveOutWrite.\n");

            return FALSE;
        }

        return TRUE;
    }
}