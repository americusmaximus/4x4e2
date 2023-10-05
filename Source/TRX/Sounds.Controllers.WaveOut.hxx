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

#include "Native.Basic.hxx"
#include "Sounds.Controllers.hxx"

#include <mmeapi.h>

#define INVALID_WAVEOUT_DEVICE_INDEX (-1)

namespace Sounds
{
    AbstractSoundDeviceController* InitializeSoundWaveOutDeviceController(const s32 indx);

    BOOL ReleaseSoundWaveOutDeviceController(AbstractSoundDeviceController* self);
    BOOL StartSoundWaveOutDeviceController(AbstractSoundDeviceController* self);
    BOOL StopSoundWaveOutDeviceController(AbstractSoundDeviceController* self);
    BOOL ResetSoundWaveOutDeviceController(AbstractSoundDeviceController* self, const u32 bits, const u32 channels, const u32 hz, u32* count);
    BOOL PollSoundWaveOutDeviceController(AbstractSoundDeviceController* self);
    SoundMixMode QuerySoundWaveOutDeviceControllerCapabilities(AbstractSoundDeviceController* self);
    void ApplySoundWaveOutDeviceControllerOptions(AbstractSoundDeviceController* self);
    void SelectSoundWaveOutDeviceControllerPosition(AbstractSoundDeviceController* self, const f64 x, const f64 y, const f64 z);
    void SelectSoundWaveOutDeviceControllerOrientation(AbstractSoundDeviceController* self, const f32 x1, const f32 y1, const f32 z1, const f32 xt, const f32 yt, const f32 zt, const f32 xf, const f32 yf, const f32 zf);
    void SelectSoundWaveOutDeviceControllerVelocity(AbstractSoundDeviceController* self, const f32 x, const f32 y, const f32 z);
    void SelectSoundWaveOutDeviceControllerDistanceFactor(AbstractSoundDeviceController* self, const f32 value);
    void SelectSoundWaveOutDeviceControllerEnvironment(AbstractSoundDeviceController* self, const f32 volume, const f32 time, const f32 damping);
    u32 AllocateSoundWaveOutDeviceControllerSoundSample(AbstractSoundDeviceController* self, const SoundSampleDescriptor* desc);
    void ReleaseSoundWaveOutDeviceControllerSoundSample(AbstractSoundDeviceController* self, const s32 indx);
    void* LockSoundWaveOutDeviceControllerSoundSample(AbstractSoundDeviceController* self, const s32 indx, const s32 offset, const s32 length);
    void UnlockSoundWaveOutDeviceControllerSoundSample(AbstractSoundDeviceController* self, const s32 indx, const s32 offset, const s32 length);
    u32 AllocateSoundWaveOutDeviceControllerSoundEffect(AbstractSoundDeviceController* self, const s32 indx);
    BOOL SelectSoundWaveOutDeviceControllerSoundEffectOptions(AbstractSoundDeviceController* self, SoundEffect* effect, const u32 mode);
    f64 AcquireSoundWaveOutDeviceControllerSoundEffectPosition(AbstractSoundDeviceController* self, SoundEffect* effect);
    BOOL StartSoundWaveOutDeviceControllerSoundEffect(AbstractSoundDeviceController* self, SoundEffect* effect);
    void ReleaseSoundWaveOutDeviceControllerSoundEffect(AbstractSoundDeviceController* self, SoundEffect* effect);
    BOOL AcquireSoundWaveOutDeviceControllerSoundEffectState(AbstractSoundDeviceController* self, SoundEffect* effect);

    // INHERITANCE: AbstractSoundDeviceControllerSelf
    struct SoundWaveOutSoundDeviceControllerSelf
    {
        ABSTRACTSOUNDDEVICECONTROLLERRELEASE Release = (ABSTRACTSOUNDDEVICECONTROLLERRELEASE)&ReleaseSoundWaveOutDeviceController; // 0x005c2640
        ABSTRACTSOUNDDEVICECONTROLLERSTART Start = (ABSTRACTSOUNDDEVICECONTROLLERSTART)&StartSoundWaveOutDeviceController; // 0x005c26f0
        ABSTRACTSOUNDDEVICECONTROLLERSTOP Stop = (ABSTRACTSOUNDDEVICECONTROLLERSTOP)&StopSoundWaveOutDeviceController;// 0x005c27c0
        ABSTRACTSOUNDDEVICECONTROLLERRESET Reset = (ABSTRACTSOUNDDEVICECONTROLLERRESET)&ResetSoundWaveOutDeviceController; // 0x005c2830
        ABSTRACTSOUNDDEVICECONTROLLERPOLL Poll = (ABSTRACTSOUNDDEVICECONTROLLERPOLL)&PollSoundWaveOutDeviceController; // 0x005c2a10
        ABSTRACTSOUNDDEVICECONTROLLERQUERYCAPABILITIES QueryCapabilities = (ABSTRACTSOUNDDEVICECONTROLLERQUERYCAPABILITIES)&QuerySoundWaveOutDeviceControllerCapabilities; // 0x005c02a0
        ABSTRACTSOUNDDEVICECONTROLLERAPPLYOPTIONS ApplyOptions = (ABSTRACTSOUNDDEVICECONTROLLERAPPLYOPTIONS)&ApplySoundWaveOutDeviceControllerOptions; // 0x005c12f0
        ABSTRACTSOUNDDEVICECONTROLLERSELECTPOSITION SelectPosition = (ABSTRACTSOUNDDEVICECONTROLLERSELECTPOSITION)&SelectSoundWaveOutDeviceControllerPosition; // 0x005c12f0
        ABSTRACTSOUNDDEVICECONTROLLERSELECTORIENTATION SelectOrientation = (ABSTRACTSOUNDDEVICECONTROLLERSELECTORIENTATION)SelectSoundWaveOutDeviceControllerOrientation; // 0x005c12f0
        ABSTRACTSOUNDDEVICECONTROLLERSELECTVELOCITY SelectVelocity = (ABSTRACTSOUNDDEVICECONTROLLERSELECTVELOCITY)&SelectSoundWaveOutDeviceControllerVelocity; // 0x005c12f0
        ABSTRACTSOUNDDEVICECONTROLLERSELECTDISTANCEFACTOR SelectDistanceFactor = (ABSTRACTSOUNDDEVICECONTROLLERSELECTDISTANCEFACTOR)&SelectSoundWaveOutDeviceControllerDistanceFactor; // 0x005c12f0
        ABSTRACTSOUNDDEVICECONTROLLERSELECTENVIRONMENT SelectEnvironment = (ABSTRACTSOUNDDEVICECONTROLLERSELECTENVIRONMENT)&SelectSoundWaveOutDeviceControllerEnvironment; // 0x005c12f0
        ABSTRACTSOUNDDEVICECONTROLLERALLOCATESOUNDSAMPLE AllocateSoundSample = (ABSTRACTSOUNDDEVICECONTROLLERALLOCATESOUNDSAMPLE)&AllocateSoundWaveOutDeviceControllerSoundSample; // 0x005c12f0
        ABSTRACTSOUNDDEVICECONTROLLERRELEASESOUNDSAMPLE ReleaseSoundSample = (ABSTRACTSOUNDDEVICECONTROLLERRELEASESOUNDSAMPLE)&ReleaseSoundWaveOutDeviceControllerSoundSample; // 0x005c12f0
        ABSTRACTSOUNDDEVICECONTROLLERLOCKSOUNDSAMPLE LockSoundSample = (ABSTRACTSOUNDDEVICECONTROLLERLOCKSOUNDSAMPLE)&LockSoundWaveOutDeviceControllerSoundSample; // 0x005c12f0
        ABSTRACTSOUNDDEVICECONTROLLERUNLOCKSOUNDSAMPLE UnlockSoundSample = (ABSTRACTSOUNDDEVICECONTROLLERUNLOCKSOUNDSAMPLE)&UnlockSoundWaveOutDeviceControllerSoundSample; // 0x005c12f0
        ABSTRACTSOUNDDEVICECONTROLLERALLOCATESOUNDEFFECT AllocateSoundEffect = (ABSTRACTSOUNDDEVICECONTROLLERALLOCATESOUNDEFFECT)&AllocateSoundWaveOutDeviceControllerSoundEffect; // 0x005c02a0
        ABSTRACTSOUNDDEVICECONTROLLERSELECTSOUNDEFFECTOPTIONS SelectSoundEffectOptions = (ABSTRACTSOUNDDEVICECONTROLLERSELECTSOUNDEFFECTOPTIONS)&SelectSoundWaveOutDeviceControllerSoundEffectOptions; // 0x005c02a0
        ABSTRACTSOUNDDEVICECONTROLLERACQUIRESOUNDEFFECTPOSITION AcquireSoundEffectPosition = (ABSTRACTSOUNDDEVICECONTROLLERACQUIRESOUNDEFFECTPOSITION)&AcquireSoundWaveOutDeviceControllerSoundEffectPosition; // 0x005c1310
        ABSTRACTSOUNDDEVICECONTROLLERSTARTSOUNDEFFECT StartSoundEffect = (ABSTRACTSOUNDDEVICECONTROLLERSTARTSOUNDEFFECT)&StartSoundWaveOutDeviceControllerSoundEffect; // 0x005c02a0
        ABSTRACTSOUNDDEVICECONTROLLERRELEASESOUNDEFFECT ReleaseSoundEffect = (ABSTRACTSOUNDDEVICECONTROLLERRELEASESOUNDEFFECT)&ReleaseSoundWaveOutDeviceControllerSoundEffect; // 0x005c1330
        ABSTRACTSOUNDDEVICECONTROLLERACQUIRESOUNDEFFECTSTATE AcquireSoundEffectState = (ABSTRACTSOUNDDEVICECONTROLLERACQUIRESOUNDEFFECTSTATE)&AcquireSoundWaveOutDeviceControllerSoundEffectState; // 0x005c02a0
    };

    // a.k.a. WavOutDevice
    // INHERITANCE: AbstractSoundDeviceController
    struct SoundWaveOutSoundDeviceController
    {
        AbstractSoundDeviceControllerSelf* Self;
    };

    struct SoundWaveOutSoundControllerContainer
    {
        AbstractSoundDeviceController* _Instance = (AbstractSoundDeviceController*)0x00d44ca4; //TODO
        AbstractSoundDeviceControllerSelf* _SoundWaveOutSoundDeviceControllerSelf = (AbstractSoundDeviceControllerSelf*)0x00667a74; // TODO

        s32* _Index = (s32*)0x0067af90; // INVALID_WAVEOUT_DEVICE_INDEX // TODO
        HWAVEOUT* _Device = (HWAVEOUT*)0x00d44a98; // TODO

        struct
        {
            CHAR** _Data = (CHAR**)0x00d44afc; // TODO [MAX_SOUND_CHANNEL_COUNT]
            WAVEHDR** _Headers = (WAVEHDR**)0x00d44abc; // TODO [MAX_SOUND_CHANNEL_COUNT]
        } Buffers;

        struct
        {
            HGLOBAL* _Data = (HGLOBAL*)0x00d44adc; // TODO [MAX_SOUND_CHANNEL_COUNT]
            HGLOBAL* _Headers = (HGLOBAL*)0x00d44a9c; // TODO [MAX_SOUND_CHANNEL_COUNT]
        } Memory;

        struct
        {
            u32* _Unknown1 = (u32*)0x00d44b28; // TODO
            u32* _Unknown2 = (u32*)0x00d44b2c; // TODO
        } Counters;

        struct
        {
            struct
            {
                u32* _Bits = (u32*)0x00d44b1c; // TODO
                u32* _Channels = (u32*)0x00d44b20; // TODO
                u32* _HZ = (u32*)0x00d44b24; // TODO
            } Active;
        } Mode;
    };

    extern SoundWaveOutSoundControllerContainer SoundWaveOutSoundControllerState;

    BOOL PollSoundWaveOutDeviceControllerBuffer(const u32 indx);
    u32 AcquireSoundWaveOutDeviceControllerUnknownValue1(void);
}