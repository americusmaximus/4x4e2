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

#define MAX_SOUND_LOCK_COUNT 100

#define MAX_SOUND_THREAD_ACTION_ITERATION_COUNT 100

namespace Sounds
{
    struct SoundContainer
    {
        SoundMixMode MixMode = SoundMixMode::Advanced; // 0x0067ace8

        struct
        {
            s32* _Count = (s32*)0x00d44234; //TODO
            HANDLE* _Mutex = (HANDLE*)0x00d44244; //TODO
        } Lock;

        struct
        {
            u32* _Bits = (u32*)0x0067acd0; // TODO, default value 16
            u32* _ChannelCount = (u32*)0x0067acd4; // TODO, default value 2
            u32* _HZ = (u32*)0x0067acd8; // TODO, default value 22050

            f32* _MaximumSoftWareLatency = (f32*)0x0067acfc; // TODO default value 0.5f;
        } Options;

        struct
        {
            f32* _TimeValue = (f32*)0x00d44238; //TODO
            BOOL* _IsActive = (BOOL*)0x00d4423c; //TODO
            BOOL* _IsQuit = (BOOL*)0x00d44240; //TODO
        } Thread;

        struct
        {
            SoundEffect* _Cache = (SoundEffect*)0x00d33c80; // TODO array of 64

            u32* _Index = (u32*)0x00d436e0; // TODO

            struct
            {
                f32* _X = (f32*)0x00d43680; // TODO array of 2
                f32* _Y = (f32*)0x00d43688; // TODO array of 2
                f32* _Z = (f32*)0x00d43690; // TODO array of 2
            } Velocity;

            struct
            {
                struct
                {
                    f32* _X = (f32*)0x00d43698; // TODO array of 2
                    f32* _Y = (f32*)0x00d436a0; // TODO array of 2
                    f32* _Z = (f32*)0x00d436a8; // TODO array of 2
                } XYZ;

                struct
                {
                    f32* _X = (f32*)0x00d436b0; // TODO array of 2
                    f32* _Y = (f32*)0x00d436b8; // TODO array of 2
                    f32* _Z = (f32*)0x00d436c0; // TODO array of 2
                } Top;

                struct
                {
                    f32* _X = (f32*)0x00d436c8; // TODO array of 2
                    f32* _Y = (f32*)0x00d436d0; // TODO array of 2
                    f32* _Z = (f32*)0x00d436d8; // TODO array of 2
                } Front;
            } Orientation;

            struct
            {
                f32* _X = (f32*)0x00d43650; // TODO array of 2
                f32* _Y = (f32*)0x00d43660; // TODO array of 2
                f32* _Z = (f32*)0x00d436d8; // TODO array of 2
            } Position;
        } Effects;

        struct
        {
            f32* _Volume = (f32*)0x00d44228; // TODO
            f32* _Time = (f32*)0x00d4422c; // TODO
            f32* _Damping = (f32*)0x00d44230; // TODO
        } Environment;

        struct
        {
            f32* ChannelVolumes1 = (f32*)0x00d436e8; // TODO MAX_SOUND_SFX_CHANNEL_COUNT (32)
        } SFX;

        BOOL* _SoundChannelStates = (BOOL*)0x00d43768; // TODO ARRAY of 32

        s32* _SoundTime1 = (s32*)0x00d621b8; //TODO
        s32* _SoundTimeUnknown1 = (s32*)0x00d61654; //TODO, looks like SoundMixMode

        AbstractSoundDeviceController** _SoundDeviceController = (AbstractSoundDeviceController**)0x00d440a8; // TODO
        AbstractSoundRecordingDeviceController** _SoundRecordingDeviceController = (AbstractSoundRecordingDeviceController**)0x00d44a8c; // TODO

        SoundSample* _SoundEffectSamples = (SoundSample*)0x00d38a4c; // TODO 64 elements, todo name

        f32* _UnknownSoundEffectValue1 = (f32*)0x00d33c70; // TODO, looks like it is read-only and always zero

        SoundEffectDescriptor* _SoundEffectDescriptor = (SoundEffectDescriptor*)0x00d38688; // TOOD array of 8
        s32* _SoundEffectDescriptorIndex = (s32*)0x00d38680;
    };

    extern SoundContainer SoundState;

    void LockSounds(void);
    void UnlockSound1(void);
    void UnlockSound2(void);
    void UnlockSound3(void);

    SoundMixMode AcquireSoundMixMode2(void);
    void SelectSoundMixMode(const SoundMixMode mode);

    BOOL RestartSoundThread(const f32 value);
    DWORD WINAPI StartSoundThread(LPVOID);
    BOOL StopSoundThread(void);

    f32 AcquireMaximumSoftWareSoundLatency(void);
    void SelectMaximumSoftWareLatency(const f32 value);

    u32 AcquireSoundChannelCount(void);
    f32 AcquireSoundEffectChannelVolume(const s32 indx);
    void AcquireSoundOutputOptions(u32* bits, u32* channels, u32* hz);
    f64 AcquireUnknownSoundValue101(const f64 value);
    s32 AcquireSoundSampleDescriptorValue204(SoundSampleDescriptor* self, const s32 offset);
    BOOL AcquireSoundEffectChannelState(const s32 indx);
    void SelectSoundEffectChannelState(const s32 indx, const BOOL value);

    void DisposeSoundSample(SoundSample* self);
    void ReleaseSoundEffectSamples(void);
    void DisposeSoundEffect(SoundEffect* self);
    void ReleaseSoundSampleMemory(SoundSample* self);
    void SelectSoundEffectChannelVolume(const s32 indx, const f32 volume);
    void* LockSoundSample(SoundSample* self, const s32 offset, const s32 length);
    void UnlockSoundSample(SoundSample* self);
    s32 AcquireSoundSampleDescriptorOffset(SoundSampleDescriptor* self, const s32 offset);
    BOOL PollSoundEffectStream(SoundEffect* self);
    void UpdateSoundEffectPosition(SoundEffect* self, const f64 position);
    u32 AcquireUnknownSoundSampleDescriptorValue1(SoundSampleDescriptor* self);
    void InitializeSoundEffectDescriptor(SoundEffectDescriptor* self);
    void PopSoundEffectDescriptor(void);
    void PushSoundEffectDescriptor(void);
    void LoadSoundSampleDescriptor(SoundSampleDescriptor* self);

    typedef const BOOL(CDECLAPI* FUN_005B4D20) (void); // TODO
    static FUN_005B4D20 FUN_005b4d20 = (FUN_005B4D20)0x005b4d20;// TODO

    typedef const void(CDECLAPI* FUN_005B9AC0) (void); // TODO
    static FUN_005B9AC0 FUN_005b9ac0 = (FUN_005B9AC0)0x005b9ac0; // TODO

    typedef const BOOL(CDECLAPI* FUN_005BB8B0) (SoundEffect*, f32); // TODO
    static FUN_005BB8B0 FUN_005bb8b0 = (FUN_005BB8B0)0x005bb8b0; // TODO

    typedef const void(CDECLAPI* FUN_005BC8D0) (SoundEffect*); // TODO
    static FUN_005BC8D0 FUN_005bc8d0 = (FUN_005BC8D0)0x005bc8d0; // TODO
}