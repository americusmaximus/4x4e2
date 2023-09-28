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
#include "Sounds.Basic.hxx"

#define INVALID_SOUND_DEVICE_COUNT (-1)
#define INVALID_SOUND_DEVICE_INDEX (-1)

#define INVALID_SOUND_RECORDING_DEVICE_COUNT (-1)
#define INVALID_SOUND_RECORDING_DEVICE_INDEX (-1)

#define MIN_SOUND_DEVICE_INDEX 0
#define MIN_SOUND_DEVICE_COUNT 0
#define MAX_SOUND_DEVICE_COUNT 8

#define MIN_SOUND_RECORDING_DEVICE_INDEX 0
#define MIN_SOUND_RECORDING_DEVICE_COUNT 0
#define MAX_SOUND_RECORDING_DEVICE_COUNT 8

#define MAX_SOUND_ENUMERATED_DEVICE_COUNT 12

#define MAX_SOUND_DEVICE_NAME_LENGTH 256

namespace Sounds
{
    enum class SoundDeviceType : u32
    {
        Wave = 0,
        DirectSound = 1
    };

    struct SoundDeviceDetail
    {
        BOOL IsPrimary;
        GUID UID;
        char Name[MAX_SOUND_DEVICE_NAME_LENGTH];
        BOOL IsEmulated;
        SoundMixMode MixMode;
    };

    struct SoundDevice
    {
        char Name[MAX_SOUND_DEVICE_NAME_LENGTH];
        SoundDeviceType Type;
        s32 Index;
        SoundMixMode MixMode;
        s32 Unk2;// TODO
        BOOL IsPrimary;
        BOOL IsEmulated;
    };

    struct SoundRecordingDevice
    {
        char Name[MAX_SOUND_DEVICE_NAME_LENGTH];
        SoundDeviceType Type;
        s32 Index;
    };

    struct SoundDeviceContainer
    {
        s32 SoundDeviceCount = INVALID_SOUND_DEVICE_COUNT; // 0x0067ace0
        SoundDevice SoundDevices[MAX_SOUND_DEVICE_COUNT]; // 0x00d437e8

        s32 EnumeratedDeviceCount = INVALID_SOUND_DEVICE_COUNT; // 0x0067ac8c
        s32* _SoundDeviceIndex = (s32*)0x0067ace4; // TODO INVALID_SOUND_DEVICE_INDEX
        SoundDeviceDetail EnumeratedDevices[MAX_SOUND_ENUMERATED_DEVICE_COUNT]; // 0x00d1dbc4

        s32 SoundRecordingDeviceCount = INVALID_SOUND_RECORDING_DEVICE_COUNT; // 0x0067ad00
        s32 SoundRecordingDeviceIndex = INVALID_SOUND_RECORDING_DEVICE_INDEX; // 0x0067ad04
        SoundRecordingDevice SoundRecordingDevices[MAX_SOUND_RECORDING_DEVICE_COUNT]; // 0x00d4424c
    };

    extern SoundDeviceContainer SoundDeviceState;

    BOOL AcquireSoundDeviceCapabilities(const s32 indx, SoundDevice* device);
    s32 AcquireSelectedSoundDeviceIndex(void);
    s32 AcquireSoundDeviceCount(void);
    s32 AcquireSoundDeviceIndex(void);
    void AcquireSoundDevice(const s32 indx, SoundDevice* device);
    void SelectSoundDevice(const s32 indx);

    BOOL AcquireSoundRecordingDeviceCapabilities(const u32 indx, SoundRecordingDevice* device);
    s32 AcquireSelectedSoundRecordingDeviceIndex(void);
    s32 AcquireSoundRecordingDeviceCount(void);
    s32 AcquireSoundRecordingDeviceIndex(void);
    void AcquireSoundRecordingDevice(const s32 indx, SoundRecordingDevice* device);
    void SelectSoundRecordingDevice(const s32 indx);
}