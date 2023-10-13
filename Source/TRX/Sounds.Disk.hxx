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

namespace Sounds
{
    struct SoundDisk
    {
        u32 ID;

        BOOL IsOpen;
        BOOL IsPlaying;
        BOOL IsPaused;

        u32 Track;

        u32 PlayFrom;
        u32 PlayTo;

        f32 Volume;
    };

    struct SoundDiskContainer
    {
        SoundDisk* _Instance = (SoundDisk*)0x00d33c50; // TODO

        BOOL IsOpen; // 0x00d33c74
        u32 DeviceID; // 0x00d33c78
    };

    extern SoundDiskContainer SoundDiskState;

    void SDE(SoundDisk* self, const char* command, const u32 code);

    BOOL OpenSoundDiskDevice(void);
    BOOL StopSoundDiskDevice(void);

    BOOL AcquireSoundDiskState(SoundDisk* self, const u32 track);
    BOOL CloseSoundDisk(SoundDisk* self);
    BOOL OpenSoundDisk(SoundDisk* self);
    BOOL PauseSoundDisk(SoundDisk* self);
    BOOL PlaySoundDisk(SoundDisk* self, const u32 track, const f32 position);
    BOOL PlaySoundDiskInBack(const u32 value, const f32 offset);
    BOOL SelectSoundDiskTimeFormat(SoundDisk* self);
    BOOL SelectSoundDiskVolume(SoundDisk* self, const f32 volume);
    BOOL StopSoundDisk(SoundDisk* self);
    SoundDisk* InitializeSoundDisk(SoundDisk* self);
    u32 AcquireSoundDiskTrackLength(SoundDisk* self, const u32 track);
    u32 AcquireSoundDiskTrackPosition(SoundDisk* self, const u32 track);
    void ReleaseSoundDisk(SoundDisk* self);
}