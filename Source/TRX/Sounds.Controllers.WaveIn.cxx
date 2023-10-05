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
#include "Native.Basic.hxx"
#include "Sounds.Controllers.WaveIn.hxx"

#define WAVE_FORMAT_NONE 0

#define MAX_SOUND_WAVEIN_MODE_SCORE 99999

using namespace Logger;
using namespace Mathematics;

namespace Sounds
{
    // 0x005c3400
    AbstractSoundRecordingDeviceController* InitializeSoundWaveInDeviceController(const s32 indx)
    {
        if (!ReleaseSoundWaveInDeviceController(SoundWaveInSoundControllerState._Instance)) { return NULL; }

        WAVEINCAPSA caps;

        if (waveInGetDevCapsA(indx, &caps, sizeof(WAVEINCAPSA)) != MMSYSERR_NOERROR) { return NULL; }

        *SoundWaveInSoundControllerState._Index = indx;

        return SoundWaveInSoundControllerState._Instance;
    }

    // 0x005c2b70
    BOOL ReleaseSoundWaveInDeviceController(AbstractSoundRecordingDeviceController* self)
    {
        const auto result = self->Self->Stop(self);

        for (u32 x = 0; x < MAX_SOUND_WAVEIN_BUFFER_COUNT; x++)
        {
            if (SoundWaveInSoundControllerState.Memory._Data[x] != NULL)
            {
                GlobalFree(SoundWaveInSoundControllerState.Memory._Data[x]);
                SoundWaveInSoundControllerState.Memory._Data[x] = NULL;
            }

            SoundWaveInSoundControllerState.Buffers._Data[x] = NULL;

            if (SoundWaveInSoundControllerState.Memory._Headers[x] != NULL)
            {
                GlobalFree(SoundWaveInSoundControllerState.Memory._Headers[x]);
                SoundWaveInSoundControllerState.Memory._Headers[x] = NULL;
            }

            SoundWaveInSoundControllerState.Buffers._Headers[x] = NULL;
        }

        if (*SoundWaveInSoundControllerState._Device == NULL) { return result; }

        if (waveInClose(*SoundWaveInSoundControllerState._Device) != MMSYSERR_NOERROR)
        {
            LogMessage("[SOUND] Unable to perform waveInClose.\n");

            *SoundWaveInSoundControllerState._Device = NULL;

            return FALSE;
        }

        *SoundWaveInSoundControllerState._Device = NULL;

        return result;
    }

    // 0x005c2c20
    BOOL StartSoundWaveInDeviceController(AbstractSoundRecordingDeviceController* self)
    {
        for (u32 x = 0; x < *SoundWaveInSoundControllerState.Counters._Unknown1; x++)
        {
            ZeroMemory(&SoundWaveInSoundControllerState.Buffers._Headers[x], sizeof(WAVEHDR));

            const auto length = (*SoundWaveInSoundControllerState.Mode.Active._Bits >> 3)
                * *SoundWaveInSoundControllerState.Counters._Unknown3 * *SoundWaveInSoundControllerState.Mode.Active._Channels;

            SoundWaveInSoundControllerState.Buffers._Headers[x]->lpData = SoundWaveInSoundControllerState.Buffers._Data[x];
            SoundWaveInSoundControllerState.Buffers._Headers[x]->dwBufferLength = length;
            SoundWaveInSoundControllerState.Buffers._Headers[x]->dwUser = x;

            if (!FillSoundWaveInDeviceController(x))
            {
                self->Self->Stop(self);

                return FALSE;
            }
        }

        *SoundWaveInSoundControllerState.Counters._Unknown2 = *SoundWaveInSoundControllerState.Counters._Unknown1 + -1;
        *SoundWaveInSoundControllerState.Counters._Unknown4 = *SoundWaveInSoundControllerState.Counters._Unknown3;

        if (waveInStart(*SoundWaveInSoundControllerState._Device) != MMSYSERR_NOERROR)
        {
            LogMessage("[SOUND] Unable to perform waveInStart.\n");

            self->Self->Stop(self);

            return FALSE;
        }

        return TRUE;
    }

    // 0x005c2d10
    BOOL StopSoundWaveInDeviceController(AbstractSoundRecordingDeviceController* self)
    {
        BOOL result = TRUE;

        if (*SoundWaveInSoundControllerState._Device != NULL)
        {
            if (waveInReset(*SoundWaveInSoundControllerState._Device) != MMSYSERR_NOERROR)
            {
                LogMessage("[SOUND] Unable to reset Wave In.\n");

                result = FALSE;
            }
        }

        for (u32 x = 0; x < MAX_SOUND_WAVEIN_BUFFER_COUNT; x++)
        {
            if (SoundWaveInSoundControllerState.Buffers._Headers[x] != NULL)
            {
                waveInUnprepareHeader(*SoundWaveInSoundControllerState._Device,
                    SoundWaveInSoundControllerState.Buffers._Headers[x], sizeof(WAVEHDR));
            }
        }

        return result;
    }

    // 0x005c2d70
    // a.k.a. setMode
    BOOL ResetSoundWaveInDeviceController(AbstractSoundRecordingDeviceController* self, const u32 bits, const u32 channels, const u32 hz, const u32* count)
    {
        if (!self->Self->Release(self)) { return FALSE; }

        WAVEINCAPSA caps;

        if (waveInGetDevCapsA(*SoundWaveInSoundControllerState._Index,
            &caps, sizeof(WAVEINCAPSA) != MMSYSERR_NOERROR)) {
            return FALSE;
        }

        *SoundWaveInSoundControllerState.Mode.Desired._HZ = hz;
        *SoundWaveInSoundControllerState.Mode.Desired._Unknown = count != NULL;

        auto actualBits = bits;
        auto actualChannels = channels;
        auto actualFormats = caps.dwFormats;

        while (TRUE)
        {
            s32 mode = INVALID_SOUND_WAVEIN_MODE;
            u32 score = MAX_SOUND_WAVEIN_MODE_SCORE;

            for (u32 x = 0; x < MAX_SOUND_WAVEIN_MODE_COUNT; x++)
            {
                if (SoundWaveInSoundControllerState.Modes[x].Format & actualFormats)
                {
                    u32 current = 0;

                    if (actualBits < SoundWaveInSoundControllerState.Modes[x].Bits) { current = current + 1; }
                    if (SoundWaveInSoundControllerState.Modes[x].Bits < actualBits) { current = current + 2; }

                    if (hz < SoundWaveInSoundControllerState.Modes[x].HZ) { current = current + 1; }
                    if (SoundWaveInSoundControllerState.Modes[x].HZ < hz) { current = current + 2; }

                    if (actualChannels < SoundWaveInSoundControllerState.Modes[x].Channels) { current = current + 2; }
                    if (SoundWaveInSoundControllerState.Modes[x].Channels < actualChannels) { current = current + 3; }

                    if (current < score)
                    {
                        score = current;
                        mode = x;
                    }
                }
            }

            *SoundWaveInSoundControllerState.Mode.Desired._Bits = actualBits;
            *SoundWaveInSoundControllerState.Mode.Desired._Channels = actualChannels;

            if (mode == INVALID_SOUND_WAVEIN_MODE)
            {
                LogMessage("[SOUND] Unable to select Wave In mode.\n");

                return FALSE;
            }

            *SoundWaveInSoundControllerState.Mode.Active._Bits = SoundWaveInSoundControllerState.Modes[mode].Bits;
            *SoundWaveInSoundControllerState.Mode.Active._Channels = SoundWaveInSoundControllerState.Modes[mode].Channels;
            *SoundWaveInSoundControllerState.Mode.Active._HZ = SoundWaveInSoundControllerState.Modes[mode].HZ;

            *SoundWaveInSoundControllerState.Counters._Unknown3 = *SoundWaveInSoundControllerState.Mode.Active._HZ / MAX_SOUND_WAVEIN_BUFFER_COUNT;
            *SoundWaveInSoundControllerState.Counters._Unknown1 = Min<u32>(MAX_SOUND_WAVEIN_BUFFER_COUNT,
                (2 * *SoundWaveInSoundControllerState.Mode.Active._HZ) / *SoundWaveInSoundControllerState.Counters._Unknown3); // TODO

            u32 count = 0;

            for (u32 x = 0; x < *SoundWaveInSoundControllerState.Counters._Unknown1; x++)
            {
                const auto length = (*SoundWaveInSoundControllerState.Mode.Active._Bits >> 3)
                    * *SoundWaveInSoundControllerState.Counters._Unknown3 * *SoundWaveInSoundControllerState.Mode.Active._Channels;

                SoundWaveInSoundControllerState.Memory._Data[x] = GlobalAlloc(GMEM_SHARE | GMEM_MOVEABLE, length);
                if (SoundWaveInSoundControllerState.Memory._Data[x] == NULL) { break; }

                SoundWaveInSoundControllerState.Buffers._Data[x] = (CHAR*)GlobalLock(SoundWaveInSoundControllerState.Memory._Data[x]);
                if (SoundWaveInSoundControllerState.Buffers._Data[x] == NULL) { break; }

                SoundWaveInSoundControllerState.Memory._Headers[x] = (WAVEHDR*)GlobalAlloc(GMEM_SHARE | GMEM_MOVEABLE, sizeof(WAVEHDR));
                if (SoundWaveInSoundControllerState.Memory._Headers[x] == NULL) { break; }

                SoundWaveInSoundControllerState.Buffers._Headers[x] = (WAVEHDR*)GlobalLock(SoundWaveInSoundControllerState.Memory._Headers[x]);
                if (SoundWaveInSoundControllerState.Buffers._Headers[x] == NULL) { break; }

                count = x;
            }

            if (*SoundWaveInSoundControllerState.Counters._Unknown1 <= count)
            {
                const auto alignment = *SoundWaveInSoundControllerState.Mode.Active._Channels
                    * ((*SoundWaveInSoundControllerState.Mode.Active._Bits & 0xffff) >> 3);

                WAVEFORMATEX format =
                {
                    .wFormatTag = WAVE_FORMAT_PCM,
                    .nChannels = (WORD)*SoundWaveInSoundControllerState.Mode.Active._Channels,
                    .nSamplesPerSec = *SoundWaveInSoundControllerState.Mode.Active._HZ,
                    .nAvgBytesPerSec = alignment * *SoundWaveInSoundControllerState.Mode.Active._HZ,
                    .nBlockAlign = (WORD)alignment,
                    .wBitsPerSample = (WORD)*SoundWaveInSoundControllerState.Mode.Active._Bits
                };

                if (waveInOpen(SoundWaveInSoundControllerState._Device,
                    *SoundWaveInSoundControllerState._Index, &format, NULL, NULL, WAVE_FORMAT_NONE) == MMSYSERR_NOERROR)
                {
                    return TRUE;
                }
            }

            if (!self->Self->Release(self)) { return FALSE; }

            actualFormats = actualFormats & ~SoundWaveInSoundControllerState.Modes[mode].Format;

            actualBits = *SoundWaveInSoundControllerState.Mode.Desired._Bits;
            actualChannels = *SoundWaveInSoundControllerState.Mode.Desired._Channels;
        }
    }

    // 0x005c30e0
    // a.k.a. poll
    s32 PollSoundWaveInDeviceController(AbstractSoundRecordingDeviceController* self, void*, void*)
    {
        // TODO: NOT IMPLEMENTED
        // TODO: NOT IMPLEMENTED
        // TODO: NOT IMPLEMENTED
        // TODO: NOT IMPLEMENTED
        // TODO: NOT IMPLEMENTED

        return -1; // TODO constant
    }

    // 0x005c2ac0
    // a.k.a. queueWavInBuffer
    BOOL FillSoundWaveInDeviceController(const u32 indx)
    {
        if (SoundWaveInSoundControllerState.Buffers._Data[indx] == NULL || SoundWaveInSoundControllerState.Buffers._Headers[indx] == NULL) { return FALSE; }

        const auto length = (*SoundWaveInSoundControllerState.Mode.Active._Bits >> 3)
            * *SoundWaveInSoundControllerState.Counters._Unknown3 * *SoundWaveInSoundControllerState.Mode.Active._Channels;

        SoundWaveInSoundControllerState.Buffers._Headers[indx]->dwBufferLength = length;

        if (waveInPrepareHeader(*SoundWaveInSoundControllerState._Device, SoundWaveInSoundControllerState.Buffers._Headers[indx], sizeof(WAVEHDR)) != MMSYSERR_NOERROR)
        {
            LogMessage("[SOUND] Unable to perform waveInPrepareHeader.\n");

            return FALSE;
        }

        if (waveInAddBuffer(*SoundWaveInSoundControllerState._Device, SoundWaveInSoundControllerState.Buffers._Headers[indx], sizeof(WAVEHDR)) != MMSYSERR_NOERROR)
        {
            LogMessage("[SOUND] Unable to perform waveInAddBuffer.\n");

            return FALSE;
        }

        return TRUE;
    }

    // 0x005c2a90
    u32 AcquireSoundWaveInDeviceControllerUnknownValue1(void)
    {
        return (*SoundWaveInSoundControllerState.Mode.Active._Bits >> 3)
            * *SoundWaveInSoundControllerState.Counters._Unknown3
            * *SoundWaveInSoundControllerState.Mode.Active._Channels;
    }
}