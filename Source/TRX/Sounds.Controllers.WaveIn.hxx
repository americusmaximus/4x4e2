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

#include "Basic.hxx"
#include "Native.Basic.hxx"
#include "Sounds.Controllers.hxx"

#include <mmeapi.h>

#define MAX_SOUND_WAVEIN_BUFFER_COUNT 20
#define MAX_SOUND_WAVEIN_MODE_COUNT 12

#define INVALID_SOUND_WAVEIN_MODE (-1)

#define INVALID_WAVEOUT_DEVICE_INDEX (-1)

namespace Sounds
{
    struct SoundWaveInMode
    {
        u32 Format;
        u32 Bits;
        u32 Channels;
        u32 HZ;
    };

    AbstractSoundRecordingDeviceController* InitializeSoundWaveInDeviceController(const s32 indx);

    BOOL ReleaseSoundWaveInDeviceController(AbstractSoundRecordingDeviceController* self);
    BOOL StartSoundWaveInDeviceController(AbstractSoundRecordingDeviceController* self);
    BOOL StopSoundWaveInDeviceController(AbstractSoundRecordingDeviceController* self);
    BOOL ResetSoundWaveInDeviceController(AbstractSoundRecordingDeviceController* self, const u32 bits, const u32 channels, const u32 hz, const u32* count);
    s32 PollSoundWaveInDeviceController(AbstractSoundRecordingDeviceController* self, void*, void*); // TODO

    BOOL FillSoundWaveInDeviceController(const u32 indx);

    // INHERITANCE: AbstractSoundRecordingDeviceControllerSelf
    struct SoundWaveInSoundControllerSelf
    {
        ABSTRACTSOUNDRECORDINGDEVICECONTROLLERRELEASE Release = (ABSTRACTSOUNDRECORDINGDEVICECONTROLLERRELEASE)&ReleaseSoundWaveInDeviceController; // 0x005c2b70
        ABSTRACTSOUNDRECORDINGDEVICECONTROLLERSTART Start = (ABSTRACTSOUNDRECORDINGDEVICECONTROLLERSTART)&StartSoundWaveInDeviceController; // 0x005c2c20
        ABSTRACTSOUNDRECORDINGDEVICECONTROLLERSTOP Stop = (ABSTRACTSOUNDRECORDINGDEVICECONTROLLERSTOP)&StopSoundWaveInDeviceController; // 0x005c2d10
        ABSTRACTSOUNDRECORDINGDEVICECONTROLLERRESET Reset = (ABSTRACTSOUNDRECORDINGDEVICECONTROLLERRESET)&ResetSoundWaveInDeviceController; // 0x005c2d70
        ABSTRACTSOUNDRECORDINGDEVICECONTROLLERPOLL Poll = (ABSTRACTSOUNDRECORDINGDEVICECONTROLLERPOLL)&PollSoundWaveInDeviceController; // 0x005c30e0
    };

    // a.k.a. WavInDevice
    // INHERITANCE: AbstractSoundRecordingDeviceController
    struct SoundWaveInSoundController
    {
        SoundWaveInSoundControllerSelf* Self; // NOTE: Originally statically initialized.
    };

    struct SoundWaveInSoundControllerContainer
    {
        SoundWaveInSoundControllerSelf* _SoundWaveInSoundControllerSelf = (SoundWaveInSoundControllerSelf*)0x00667a5c; // TODO

        AbstractSoundRecordingDeviceController* _Instance = (AbstractSoundRecordingDeviceController*)0x00d44ca8; // TODO

        HWAVEIN* _Device = (HWAVEIN*)0x00d44b30; // TODO
        s32* _Index = (s32*)0x0067af94; // INVALID_WAVEOUT_DEVICE_INDEX // TODO

        struct
        {
            CHAR** _Data = (CHAR**)0x00d44c24; // TODO [MAX_SOUND_WAVEIN_BUFFER_COUNT]
            WAVEHDR** _Headers = (WAVEHDR**)0x00d44b84; // TODO [MAX_SOUND_WAVEIN_BUFFER_COUNT]
        } Buffers;

        struct
        {
            HGLOBAL* _Data = (HGLOBAL*)0x00d44bd4; // TODO [MAX_SOUND_WAVEIN_BUFFER_COUNT]
            HGLOBAL* _Headers = (HGLOBAL*)0x00d44b34; // TODO [MAX_SOUND_WAVEIN_BUFFER_COUNT]
        } Memory;

        struct
        {
            u32* _Unknown1 = (u32*)0x00d44c88; // TODO
            u32* _Unknown2 = (u32*)0x00d44c9c; // TODO
            u32* _Unknown3 = (u32*)0x00d44c84; // TODO
            u32* _Unknown4 = (u32*)0x00d44ca0; // TODO
        } Counters;

        struct
        {
            struct
            {
                u32* _Bits = (u32*)0x00d44c74; // TODO
                u32* _HZ = (u32*)0x00d44c7c; // TODO
                u32* _Channels = (u32*)0x00d44c78; // TODO
            } Active;

            struct
            {
                u32* _Bits = (u32*)0x00d44c8c; // TODO
                u32* _HZ = (u32*)0x00d44c94; // TODO
                u32* _Channels = (u32*)0x00d44c90; // TODO

                u32* _Unknown = (u32*)0x00d44c98; // TODO
            } Desired;
        } Mode;

        // 0x0067af98
        const SoundWaveInMode Modes[MAX_SOUND_WAVEIN_MODE_COUNT] =
        {
            {
                .Format = WAVE_FORMAT_1M08,
                .Bits = 8,
                .Channels = 1,
                .HZ = 11025
            },
            {
                .Format = WAVE_FORMAT_1M16,
                .Bits = 16,
                .Channels = 1,
                .HZ = 11025
            },
            {
                .Format = WAVE_FORMAT_1S08,
                .Bits = 8,
                .Channels = 2,
                .HZ = 11025
            },
            {
                .Format = WAVE_FORMAT_1S16,
                .Bits = 16,
                .Channels = 2,
                .HZ = 11025
            },
            {
                .Format = WAVE_FORMAT_2M08,
                .Bits = 8,
                .Channels = 1,
                .HZ = 22050
            },
            {
                .Format = WAVE_FORMAT_2M16,
                .Bits = 16,
                .Channels = 1,
                .HZ = 22050
            },
            {
                .Format = WAVE_FORMAT_2S08,
                .Bits = 8,
                .Channels = 2,
                .HZ = 22050
            },
            {
                .Format = WAVE_FORMAT_2S16,
                .Bits = 16,
                .Channels = 2,
                .HZ = 22050
            },
            {
                .Format = WAVE_FORMAT_4M08,
                .Bits = 8,
                .Channels = 1,
                .HZ = 44100
            },
            {
                .Format = WAVE_FORMAT_4M16,
                .Bits = 16,
                .Channels = 2,
                .HZ = 44100
            },
            {
                .Format = WAVE_FORMAT_4S08,
                .Bits = 8,
                .Channels = 2,
                .HZ = 44100
            },
            {
                .Format = WAVE_FORMAT_4S16,
                .Bits = 16,
                .Channels = 2,
                .HZ = 44100
            }
        };
    };

    extern SoundWaveInSoundControllerContainer SoundWaveInSoundControllerState;

    u32 AcquireSoundWaveInDeviceControllerUnknownValue1(void);
}