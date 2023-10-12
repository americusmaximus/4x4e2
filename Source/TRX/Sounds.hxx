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

#include "Assets.Sounds.hxx"
#include "Native.Basic.hxx"
#include "Sounds.Controllers.hxx"

#define MIN_SOUND_EFFECT_CHANNEL_COUNT 0
#define MAX_SOUND_EFFECT_CHANNEL_COUNT 32

#define MIN_SOUND_LOCK_COUNT 0
#define DEFAULT_SOUND_LOCK_COUNT 1
#define MAX_SOUND_LOCK_COUNT 100

#define MAX_SOUND_THREAD_ACTION_ITERATION_COUNT 100

#define MIN_SOUND_VOLUME (0.0f)
#define MAX_SOUND_VOLUME (1.0f)

namespace Sounds
{
    struct SoundContainer
    {
        SoundMixMode MixMode = SoundMixMode::Advanced; // 0x0067ace8

        struct
        {
            s32* _Count = (s32*)0x00d44234; //TODO
            HANDLE Mutex; // 0x00d44244
        } Lock;

        struct
        {
            u32* _Bits = (u32*)0x0067acd0; // TODO SOUND_BITS_16

            u32* _Channels = (u32*)0x0067acd4; // TODO, default value SOUND_CHANNEL_COUNT_STEREO

            u32* _HZ = (u32*)0x0067acd8; // TODO, default value SOUND_FREQUENCY_22050

            f32* _MaximumSoftWareLatency = (f32*)0x0067acfc; // TODO default value DEFAULT_SOUND_LATENCY
        } Options;

        struct
        {
            f32* _TimeValue = (f32*)0x00d44238; // TODO
            BOOL* _IsActive = (BOOL*)0x00d4423c; // TODO
            BOOL* _IsQuit = (BOOL*)0x00d44240; // TODO
        } Thread;

        struct
        {
            SoundEffect* _Cache = (SoundEffect*)0x00d33c80; // TODO array of 64

            u32 Index; // 0x00d436e0

            struct
            {
                f32 X[2]; // 0x00d43680 // TODO
                f32 Y[2]; // 0x00d43688 // TODO
                f32 Z[2]; // 0x00d43690 // TODO
            } Velocity;

            struct
            {
                struct
                {
                    f32 X[2]; // 0x00d43698 // TODO
                    f32 Y[2]; // 0x00d436a0 // TODO
                    f32 Z[2]; // 0x00d436a8 // TODO
                } XYZ;

                struct
                {
                    f32 X[2]; // 0x00d436b0 // TODO
                    f32 Y[2]; // 0x00d436b8 // TODO
                    f32 Z[2]; // 0x00d436c0 // TODO
                } Top;

                struct
                {
                    f32 X[2]; // 0x00d436c8 // TODO
                    f32 Y[2]; // 0x00d436d0 // TODO
                    f32 Z[2]; // 0x00d436d8 // TODO
                } Front;
            } Orientation;

            struct
            {
                f64 X[2]; // 0x00d43650 // TODO
                f64 Y[2]; // 0x00d43660 // TODO
                f64 Z[2]; // 0x00d43670 // TODO
            } Position;

            struct
            {
                struct
                {
                    f64 X[MAX_SOUND_CHANNEL_COUNT]; // 0x00d44168
                    f64 Y[MAX_SOUND_CHANNEL_COUNT]; // 0x00d441a8
                    f64 Z[MAX_SOUND_CHANNEL_COUNT]; // 0x00d441e8
                } Position; // TODO move around

                // NOTE: used from somewhere unknown
                f32* _Volumes = (f32*)0x00d436e8; // TODO [MAX_SOUND_EFFECT_CHANNEL_COUNT];

                u32* _Count = (u32*)0x0067accc; // TODO, default value MAX_SOUND_EFFECT_CHANNEL_COUNT, move to Options?
            } Channels;
        } Effects;

        struct
        {
            f32 Volume; // 0x00d44228
            f32 Time; // 0x00d4422c
            f32 Damping; // 0x00d44230
        } Environment;

        BOOL* _SoundChannelStates = (BOOL*)0x00d43768; // TODO ARRAY of 32

        s32* _SoundTime1 = (s32*)0x00d44248; // TODO
        s32 UnknownSoundCount1 = 1; // 0x0067acc8 // TODO enumeration?

        AbstractSoundDeviceController** _SoundDeviceController = (AbstractSoundDeviceController**)0x00d440a8; // TODO
        AbstractSoundRecordingDeviceController** _SoundRecordingDeviceController = (AbstractSoundRecordingDeviceController**)0x00d44a8c; // TODO

        SoundSample* _SoundEffectSamples = (SoundSample*)0x00d38a4c; // TODO 64 elements, todo name
        s32* _SoundEffectIndex = (s32*)0x00d38a48; // TODO
        f32* _UnknownSoundEffectValue1 = (f32*)0x00d33c70; // TODO, looks like it is read-only and always zero

        SoundEffectDescriptor* _SoundEffectDescriptors = (SoundEffectDescriptor*)0x00d38688; // TOOD array of 8
        s32* _SoundEffectDescriptorIndex = (s32*)0x00d38680;
    };

    extern SoundContainer SoundState;

    BOOL RestartSoundThread(const f32 value);
    BOOL StopSoundThread(void);
    DWORD WINAPI StartSoundThread(LPVOID);
    f32 AcquireMaximumSoftWareSoundLatency(void);
    f64 AcquireUnknownSoundValue101(const f64 value);
    SoundMixMode AcquireSoundMixMode2(void);
    u32 AcquireSoundChannelCount(void);
    u32 AcquireSoundOutputBitsOptions(void);
    u32 AcquireSoundOutputFrequencyOptions(void);
    void AcquireSoundOutputOptions(u32* bits, u32* channels, u32* hz);
    void LockSounds(void);
    void ReleaseSounds(void);
    void SelectMaximumSoftWareLatency(const f32 value);
    void SelectSoundMixMode(const SoundMixMode mode);
    void UnlockSound1(void);
    void UnlockSound2(void);
    void UnlockSound3(void);
    void UnlockSounds(const s32 value);

    typedef const void(CDECLAPI* FUN_005BC8D0) (SoundEffect*); // TODO
    static FUN_005BC8D0 FUN_005bc8d0 = (FUN_005BC8D0)0x005bc8d0; // TODO
}