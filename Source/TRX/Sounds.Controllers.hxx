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
#include "Sounds.Basic.hxx"

namespace Sounds
{
    struct SoundEffect;
    struct SoundSampleDescriptor;

    struct AbstractSoundDeviceController;

    typedef const BOOL(CDECLAPI* ABSTRACTSOUNDDEVICECONTROLLERRELEASE) (AbstractSoundDeviceController* self);
    typedef const BOOL(CDECLAPI* ABSTRACTSOUNDDEVICECONTROLLERSTART) (AbstractSoundDeviceController* self);
    typedef const BOOL(CDECLAPI* ABSTRACTSOUNDDEVICECONTROLLERSTOP) (AbstractSoundDeviceController* self);
    typedef const BOOL(CDECLAPI* ABSTRACTSOUNDDEVICECONTROLLERRESET)(AbstractSoundDeviceController* self, const u32 bits, const u16 channels, const u32 hz, u32* count);
    typedef const BOOL(CDECLAPI* ABSTRACTSOUNDDEVICECONTROLLERPOLL) (AbstractSoundDeviceController* self);
    typedef const SoundMixMode(CDECLAPI* ABSTRACTSOUNDDEVICECONTROLLERQUERYCAPABILITIES) (AbstractSoundDeviceController* self);
    typedef const void(CDECLAPI* ABSTRACTSOUNDDEVICECONTROLLERAPPLYOPTIONS) (AbstractSoundDeviceController* self);
    typedef const void(CDECLAPI* ABSTRACTSOUNDDEVICECONTROLLERSELECTPOSITION) (AbstractSoundDeviceController* self, const f64 x, const f64 y, const f64 z);
    typedef const void(CDECLAPI* ABSTRACTSOUNDDEVICECONTROLLERSELECTORIENTATION) (AbstractSoundDeviceController* self, const f32 x, const f32 y, const f32 z, const f32 xt, const f32 yt, const f32 zt, const f32 xf, const f32 yf, const f32 zf);
    typedef const void(CDECLAPI* ABSTRACTSOUNDDEVICECONTROLLERSELECTVELOCITY) (AbstractSoundDeviceController* self, const f32 x, const f32 y, const f32 z);
    typedef const void(CDECLAPI* ABSTRACTSOUNDDEVICECONTROLLERSELECTDISTANCEFACTOR) (AbstractSoundDeviceController* self, const f32 value);
    typedef const void(CDECLAPI* ABSTRACTSOUNDDEVICECONTROLLERSELECTENVIRONMENT) (AbstractSoundDeviceController* self, const f32 volume, const f32 time, const f32 damping);
    typedef const u32(CDECLAPI* ABSTRACTSOUNDDEVICECONTROLLERALLOCATESOUNDSAMPLE) (AbstractSoundDeviceController* self, const SoundSampleDescriptor* desc);
    typedef const void(CDECLAPI* ABSTRACTSOUNDDEVICECONTROLLERRELEASESOUNDSAMPLE) (AbstractSoundDeviceController* self, const s32 indx);
    typedef void* (CDECLAPI* ABSTRACTSOUNDDEVICECONTROLLERLOCKSOUNDSAMPLE) (AbstractSoundDeviceController* self, const s32 indx, const s32 offset, const s32 length);
    typedef const void(CDECLAPI* ABSTRACTSOUNDDEVICECONTROLLERUNLOCKSOUNDSAMPLE) (AbstractSoundDeviceController* self, const s32 indx, const s32 offset, const s32 length);
    typedef const u32(CDECLAPI* ABSTRACTSOUNDDEVICECONTROLLERALLOCATESOUNDEFFECT) (AbstractSoundDeviceController* self, const s32 indx);
    typedef const BOOL(CDECLAPI* ABSTRACTSOUNDDEVICECONTROLLERSELECTSOUNDEFFECTOPTIONS) (AbstractSoundDeviceController* self, SoundEffect* effect, const s32 options); // TODO enum
    typedef const f64(CDECLAPI* ABSTRACTSOUNDDEVICECONTROLLERACQUIRESOUNDEFFECTPOSITION) (void* self, SoundEffect* effect);
    typedef const BOOL(CDECLAPI* ABSTRACTSOUNDDEVICECONTROLLERSTARTSOUNDEFFECT) (AbstractSoundDeviceController* self, SoundEffect* effect);
    typedef const void(CDECLAPI* ABSTRACTSOUNDDEVICECONTROLLERRELEASESOUNDEFFECT) (AbstractSoundDeviceController* self, SoundEffect* effect);
    typedef const BOOL(CDECLAPI* ABSTRACTSOUNDDEVICECONTROLLERACQUIRESOUNDEFFECTSTATE) (AbstractSoundDeviceController* self, SoundEffect* effect);

    struct AbstractSoundDeviceControllerSelf
    {
        ABSTRACTSOUNDDEVICECONTROLLERRELEASE Release;
        ABSTRACTSOUNDDEVICECONTROLLERSTART Start;
        ABSTRACTSOUNDDEVICECONTROLLERSTOP Stop;
        ABSTRACTSOUNDDEVICECONTROLLERRESET Reset;
        ABSTRACTSOUNDDEVICECONTROLLERPOLL Poll;
        ABSTRACTSOUNDDEVICECONTROLLERQUERYCAPABILITIES QueryCapabilities;
        ABSTRACTSOUNDDEVICECONTROLLERAPPLYOPTIONS ApplyOptions;
        ABSTRACTSOUNDDEVICECONTROLLERSELECTPOSITION SelectPosition;
        ABSTRACTSOUNDDEVICECONTROLLERSELECTORIENTATION SelectOrientation;
        ABSTRACTSOUNDDEVICECONTROLLERSELECTVELOCITY SelectVelocity;
        ABSTRACTSOUNDDEVICECONTROLLERSELECTDISTANCEFACTOR SelectDistanceFactor;
        ABSTRACTSOUNDDEVICECONTROLLERSELECTENVIRONMENT SelectEnvironment;
        ABSTRACTSOUNDDEVICECONTROLLERALLOCATESOUNDSAMPLE AllocateSoundSample;
        ABSTRACTSOUNDDEVICECONTROLLERRELEASESOUNDSAMPLE ReleaseSoundSample;
        ABSTRACTSOUNDDEVICECONTROLLERLOCKSOUNDSAMPLE LockSoundSample;
        ABSTRACTSOUNDDEVICECONTROLLERUNLOCKSOUNDSAMPLE UnlockSoundSample;
        ABSTRACTSOUNDDEVICECONTROLLERALLOCATESOUNDEFFECT AllocateSoundEffect;
        ABSTRACTSOUNDDEVICECONTROLLERSELECTSOUNDEFFECTOPTIONS SelectSoundEffectOptions;
        ABSTRACTSOUNDDEVICECONTROLLERACQUIRESOUNDEFFECTPOSITION AcquireSoundEffectPosition;
        ABSTRACTSOUNDDEVICECONTROLLERSTARTSOUNDEFFECT StartSoundEffect;
        ABSTRACTSOUNDDEVICECONTROLLERRELEASESOUNDEFFECT ReleaseSoundEffect;
        ABSTRACTSOUNDDEVICECONTROLLERACQUIRESOUNDEFFECTSTATE AcquireSoundEffectState;
    };

    struct AbstractSoundDeviceController
    {
        AbstractSoundDeviceControllerSelf* Self;
    };

    struct AbstractSoundRecordingDeviceController;

    typedef const BOOL(CDECLAPI* ABSTRACTSOUNDRECORDINGDEVICECONTROLLERRELEASE) (AbstractSoundRecordingDeviceController* self);
    typedef const BOOL(CDECLAPI* ABSTRACTSOUNDRECORDINGDEVICECONTROLLERSTART) (AbstractSoundRecordingDeviceController* self);
    typedef const BOOL(CDECLAPI* ABSTRACTSOUNDRECORDINGDEVICECONTROLLERSTOP) (AbstractSoundRecordingDeviceController* self);
    typedef const BOOL(CDECLAPI* ABSTRACTSOUNDRECORDINGDEVICECONTROLLERRESET)(AbstractSoundRecordingDeviceController* self, const u32 bits, const u16 channels, const u32 hz, u32* count);
    typedef const s32(CDECLAPI* ABSTRACTSOUNDRECORDINGDEVICECONTROLLERPOLL) (AbstractSoundRecordingDeviceController* self, void*, void*); // TODO

    struct AbstractSoundRecordingDeviceControllerSelf
    {
        ABSTRACTSOUNDRECORDINGDEVICECONTROLLERRELEASE Release;
        ABSTRACTSOUNDRECORDINGDEVICECONTROLLERSTART Start;
        ABSTRACTSOUNDRECORDINGDEVICECONTROLLERSTOP Stop;
        ABSTRACTSOUNDRECORDINGDEVICECONTROLLERRESET Reset;
        ABSTRACTSOUNDRECORDINGDEVICECONTROLLERPOLL Poll;
    };

    struct AbstractSoundRecordingDeviceController
    {
        AbstractSoundRecordingDeviceControllerSelf* Self;
    };

    struct SoundDeviceControllerContainer
    {
        BOOL* _IsSoftwareDevice = (BOOL*)0x00d1ef5c; // TODO
        BOOL* _ActiveState = (BOOL*)0x00d440ac; // TODO


        SoundMixMode* _MixMode = (SoundMixMode*)0x00d440b0; //TODO

        void** _UnknownMemory1 = (void**)0x00d440ec; // TODO
        void** _UnknownMemory2 = (void**)0x00d44110; // TODO
        void** _UnknownMemory3 = (void**)0x00d440c8; // TODO

        void** _UnknownArray1 = (void**)0x00d440cc; // TODO array of 8
        void** _UnknownArray2 = (void**)0x00d44114; // TODO array of 8
        void** _UnknownArray3 = (void**)0x00d44134; // TODO array of 8
        void** _UnknownArray4 = (void**)0x00d440f0; // TODO array of 8

        s32* _Unknown1 = (s32*)0x00d440c4; // TODO
        s32* _Unknown2 = (s32*)0x00d440b8; // TODO
        s32* _Unknown3 = (s32*)0x00d440bc; // TODO
        s32* _Unknown4 = (s32*)0x00d440c0; // TODO
    };

    extern SoundDeviceControllerContainer SoundDeviceControllerState;

    BOOL InitializeSoundDeviceController(void);
    BOOL IsSoundDeviceControllerAvailable(void);
    BOOL ReleaseSoundDeviceController(void);
    BOOL StopSoundDeviceController(void);
    void ReleaseSoundDeviceControllerMemory(void);

    BOOL InitializeSoundRecordingDeviceController(void);
    BOOL IsSoundRecordingDeviceControllerAvailable(void);
    BOOL ReleaseSoundRecordingDeviceController(void);
    BOOL StopSoundRecordingDeviceController(void);

    void ReleaseSoundDeviceControllerSoundSample(SoundSample* self);
    BOOL AcquireSoundDeviceContexActiveState(void);
    SoundMixMode AcquireSoundControllerMixMode(void);
    void PollSoundDeviceControllerSoundSamples(const BOOL mode);
    void SelectSoundDeviceControllerOptions(void);
    void PollSoundDeviceController(void);
    BOOL SelectSoundDeviceControllerSoundMode(const u32 bits, const u32 channels, const u32 hz);
}