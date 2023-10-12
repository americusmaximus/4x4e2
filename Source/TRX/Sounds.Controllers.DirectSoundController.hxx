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

#include "Direct.Sound.hxx"
#include "Sounds.Controllers.hxx"

#define INVALID_ACTIVE_SOUND_BUFFER_POOL_INDEX 0
#define MIN_ACTIVE_SOUND_BUFFER_POOL_INDEX 1
#define MAX_ACTIVE_SOUND_BUFFER_POOL_SIZE 31

namespace Sounds
{
    struct SoundUnk0x18 // TODO
    {
        IDirectSoundBuffer* Buffer;

        s32 Unk1; // TODO
        s32 Unk2; // TODO
        u32 Unk3; // TODO

        void* Audio; // TODO
        u32 AudioSize; // TODO
    };

    struct SoundUnk0x24 // TODO
    {
        IDirectSoundBuffer* Buffer;
        IDirectSound3DBuffer* AdvancedBuffer;

        s32 Unk3; // TODO
        s32 Unk4; // TODO
        s32 Unk5; // TODO
        s32 Volume;
        s32 Pan;
        s32 HZ;
        s32 Unk9; // TODO
    };

    void SoundDirectSoundDeviceControllerConstructor(void);
    AbstractSoundDeviceController* InitializeSoundDirectSoundDeviceController(const s32 indx);

    BOOL ReleaseSoundDirectSoundDeviceController(AbstractSoundDeviceController* self);
    BOOL StartSoundDirectSoundDeviceController(AbstractSoundDeviceController* self);
    BOOL StopSoundDirectSoundDeviceController(AbstractSoundDeviceController* self);
    BOOL ResetSoundDirectSoundDeviceController(AbstractSoundDeviceController* self, const u32 bits, const u32 channels, const u32 hz, u32* count);
    BOOL PollSoundDirectSoundDeviceController(AbstractSoundDeviceController* self);
    SoundMixMode QuerySoundDirectSoundDeviceControllerCapabilities(AbstractSoundDeviceController* self);
    void ApplySoundDirectSoundDeviceControllerOptions(AbstractSoundDeviceController* self);
    void SelectSoundDirectSoundDeviceControllerPosition(AbstractSoundDeviceController* self, const f64 x, const f64 y, const f64 z);
    void SelectSoundDirectSoundDeviceControllerOrientation(AbstractSoundDeviceController* self, const f32 x, const f32 y, const f32 z, const f32 xt, const f32 yt, const f32 zt, const f32 xf, const f32 yf, const f32 zf);
    void SelectSoundDirectSoundDeviceControllerVelocity(AbstractSoundDeviceController* self, const f32 x, const f32 y, const f32 z);
    void SelectSoundDirectSoundDeviceControllerDistanceFactor(AbstractSoundDeviceController* self, const f32 value);
    void SelectSoundDirectSoundDeviceControllerEnvironment(AbstractSoundDeviceController* self, const f32 volume, const f32 time, const f32 damping);
    u32 AllocateSoundDirectSoundDeviceControllerSoundSample(AbstractSoundDeviceController* self, const SoundSampleDescriptor* desc);
    void ReleaseSoundDirectSoundDeviceControllerSoundSample(AbstractSoundDeviceController* self, const s32 indx);
    void* LockSoundDirectSoundDeviceControllerSoundSample(AbstractSoundDeviceController* self, const s32 indx, const s32 offset, const s32 length);
    void UnlockSoundDirectSoundDeviceControllerSoundSample(AbstractSoundDeviceController* self, const s32 indx, const s32 offset, const s32 length);
    u32 AllocateSoundDirectSoundDeviceControllerSoundEffect(AbstractSoundDeviceController* self, const s32 indx);
    BOOL SelectSoundDirectSoundDeviceControllerSoundEffectOptions(AbstractSoundDeviceController* self, SoundEffect* effect, const u32 mode);
    f64 AcquireSoundDirectSoundDeviceControllerSoundEffectPosition(AbstractSoundDeviceController* self, SoundEffect* effect);
    BOOL StartSoundDirectSoundDeviceControllerSoundEffect(AbstractSoundDeviceController* self, SoundEffect* effect);
    void ReleaseSoundDirectSoundDeviceControllerSoundEffect(AbstractSoundDeviceController* self, SoundEffect* effect);
    BOOL AcquireSoundDirectSoundDeviceControllerSoundEffectState(AbstractSoundDeviceController* self, SoundEffect* effect);

    // INHERITANCE: AbstractSoundDeviceControllerSelf
    struct SoundDirectSoundDeviceControllerSelf
    {
        ABSTRACTSOUNDDEVICECONTROLLERRELEASE Release = (ABSTRACTSOUNDDEVICECONTROLLERRELEASE)&ReleaseSoundDirectSoundDeviceController; // 0x005b5100
        ABSTRACTSOUNDDEVICECONTROLLERSTART Start = (ABSTRACTSOUNDDEVICECONTROLLERSTART)&StartSoundDirectSoundDeviceController; // 0x005b51d0
        ABSTRACTSOUNDDEVICECONTROLLERSTOP Stop = (ABSTRACTSOUNDDEVICECONTROLLERSTOP)&StopSoundDirectSoundDeviceController; // 0x005b5340
        ABSTRACTSOUNDDEVICECONTROLLERRESET Reset = (ABSTRACTSOUNDDEVICECONTROLLERRESET)&ResetSoundDirectSoundDeviceController; // 0x005b5600
        ABSTRACTSOUNDDEVICECONTROLLERPOLL Poll = (ABSTRACTSOUNDDEVICECONTROLLERPOLL)&PollSoundDirectSoundDeviceController; // 0x005b5c20
        ABSTRACTSOUNDDEVICECONTROLLERQUERYCAPABILITIES QueryCapabilities = (ABSTRACTSOUNDDEVICECONTROLLERQUERYCAPABILITIES)&QuerySoundDirectSoundDeviceControllerCapabilities; // 0x005b5d00
        ABSTRACTSOUNDDEVICECONTROLLERAPPLYOPTIONS ApplyOptions = (ABSTRACTSOUNDDEVICECONTROLLERAPPLYOPTIONS)&ApplySoundDirectSoundDeviceControllerOptions; // 0x005b79e0
        ABSTRACTSOUNDDEVICECONTROLLERSELECTPOSITION SelectPosition = (ABSTRACTSOUNDDEVICECONTROLLERSELECTPOSITION)&SelectSoundDirectSoundDeviceControllerPosition; // 0x005b5dc0
        ABSTRACTSOUNDDEVICECONTROLLERSELECTORIENTATION SelectOrientation = (ABSTRACTSOUNDDEVICECONTROLLERSELECTORIENTATION)SelectSoundDirectSoundDeviceControllerOrientation; // 0x005b5e00
        ABSTRACTSOUNDDEVICECONTROLLERSELECTVELOCITY SelectVelocity = (ABSTRACTSOUNDDEVICECONTROLLERSELECTVELOCITY)&SelectSoundDirectSoundDeviceControllerVelocity; // 0x005b5e30
        ABSTRACTSOUNDDEVICECONTROLLERSELECTDISTANCEFACTOR SelectDistanceFactor = (ABSTRACTSOUNDDEVICECONTROLLERSELECTDISTANCEFACTOR)&SelectSoundDirectSoundDeviceControllerDistanceFactor; // 0x005b5e60
        ABSTRACTSOUNDDEVICECONTROLLERSELECTENVIRONMENT SelectEnvironment = (ABSTRACTSOUNDDEVICECONTROLLERSELECTENVIRONMENT)&SelectSoundDirectSoundDeviceControllerEnvironment; // 0x005b7990
        ABSTRACTSOUNDDEVICECONTROLLERALLOCATESOUNDSAMPLE AllocateSoundSample = (ABSTRACTSOUNDDEVICECONTROLLERALLOCATESOUNDSAMPLE)&AllocateSoundDirectSoundDeviceControllerSoundSample; // 0x005b5e90
        ABSTRACTSOUNDDEVICECONTROLLERRELEASESOUNDSAMPLE ReleaseSoundSample = (ABSTRACTSOUNDDEVICECONTROLLERRELEASESOUNDSAMPLE)&ReleaseSoundDirectSoundDeviceControllerSoundSample; // 0x005b61a0
        ABSTRACTSOUNDDEVICECONTROLLERLOCKSOUNDSAMPLE LockSoundSample = (ABSTRACTSOUNDDEVICECONTROLLERLOCKSOUNDSAMPLE)&LockSoundDirectSoundDeviceControllerSoundSample; // 0x005b6200
        ABSTRACTSOUNDDEVICECONTROLLERUNLOCKSOUNDSAMPLE UnlockSoundSample = (ABSTRACTSOUNDDEVICECONTROLLERUNLOCKSOUNDSAMPLE)&UnlockSoundDirectSoundDeviceControllerSoundSample; // 0x005b6360
        ABSTRACTSOUNDDEVICECONTROLLERALLOCATESOUNDEFFECT AllocateSoundEffect = (ABSTRACTSOUNDDEVICECONTROLLERALLOCATESOUNDEFFECT)&AllocateSoundDirectSoundDeviceControllerSoundEffect; // 0x005b6440
        ABSTRACTSOUNDDEVICECONTROLLERSELECTSOUNDEFFECTOPTIONS SelectSoundEffectOptions = (ABSTRACTSOUNDDEVICECONTROLLERSELECTSOUNDEFFECTOPTIONS)&SelectSoundDirectSoundDeviceControllerSoundEffectOptions; // 0x005b6700
        ABSTRACTSOUNDDEVICECONTROLLERACQUIRESOUNDEFFECTPOSITION AcquireSoundEffectPosition = (ABSTRACTSOUNDDEVICECONTROLLERACQUIRESOUNDEFFECTPOSITION)&AcquireSoundDirectSoundDeviceControllerSoundEffectPosition; // 0x005b7140
        ABSTRACTSOUNDDEVICECONTROLLERSTARTSOUNDEFFECT StartSoundEffect = (ABSTRACTSOUNDDEVICECONTROLLERSTARTSOUNDEFFECT)&StartSoundDirectSoundDeviceControllerSoundEffect; // 0x005b7270
        ABSTRACTSOUNDDEVICECONTROLLERRELEASESOUNDEFFECT ReleaseSoundEffect = (ABSTRACTSOUNDDEVICECONTROLLERRELEASESOUNDEFFECT)&ReleaseSoundDirectSoundDeviceControllerSoundEffect; // 0x005b76f0
        ABSTRACTSOUNDDEVICECONTROLLERACQUIRESOUNDEFFECTSTATE AcquireSoundEffectState = (ABSTRACTSOUNDDEVICECONTROLLERACQUIRESOUNDEFFECTSTATE)&AcquireSoundDirectSoundDeviceControllerSoundEffectState; // 0x005b7090
    };

    // a.k.a. DirectSoundDevice
    // INHERITANCE: AbstractSoundDeviceController
    struct SoundDirectSoundDeviceController
    {
        AbstractSoundDeviceControllerSelf* Self; // NOTE: Originally statically initialized.
    };

    struct SoundDirectSoundSoundControllerContainer
    {
        AbstractSoundDeviceController Instance; // 0x00d1f3bc
        SoundDirectSoundDeviceControllerSelf SoundDirectDeviceSoundControllerSelf; // 0x0066792c

        struct
        {
            IKsPropertySet* Instance; // 0x00d1e924
        } EAX;

        struct
        {
            IDirectSound* Instance; // 0x00d1e914
            IDirectSound3DListener* Listener; // 0x00d1e920
        } DirectSound;

        struct
        {
            struct
            {
                u32* _BitsPerSample = (u32*)0x00d1e928; //TODO
                u32* _HZ = (u32*)0x00d1e92c; //TODO
                u32* _Channels = (u32*)0x00d1e930; //TODO

                IDirectSoundBuffer* Buffer; // 0x00d1e918
            } Primary;

            struct
            {
                // TODO: these two do not go for only secondary buffer
                u32* _Unknown1 = (u32*)0x00d1e93c; // TODO, some sort of count
                u32* _Unknown2 = (u32*)0x00d1e940; //TODO some sort of count

                u32* _Count = (u32*)0x00d1e934; // TODO
                s32* _ChannelBufferSize = (s32*)0x00d1e938; // TODO, type!, name: BufferSize

                IDirectSoundBuffer* Buffer; // 0x00d1e91c
            } Secondary;

        } Buffers;

        SoundUnk0x18 SoundUnk0x18Array[65]; // 0x00d1e944 // TODO
        SoundUnk0x24 SoundUnk0x24Array[31]; // 0x00d1ef60 // TODO
    };

    extern SoundDirectSoundSoundControllerContainer SoundDirectSoundSoundControllerState;

    BOOL FillSoundDirectSoundDeviceControllerSecondaryBuffer(void);
    BOOL StopSoundDirectSoundDeviceControllerSoundSample(const s32 indx);
    s32 AcquireSoundDirectSoundSoundControllerFrequency(const f32 value);
    s32 AcquireSoundDirectSoundSoundControllerVolume(const f32 value);
    void PollSoundDirectSoundSoundControllerSoundEffect(void);
}