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

#include "App.Windows.hxx"
#include "Direct.Sound.EAX.hxx"
#include "Direct.Sound.VoiceManager.hxx"
#include "Logger.hxx"
#include "Mathematics.Basic.hxx"
#include "Sounds.Controllers.DirectSoundController.hxx"
#include "Sounds.Devices.DirectSound.hxx"
#include "Sounds.Devices.hxx"
#include "Sounds.Effects.hxx"
#include "Sounds.hxx"
#include "Sounds.Samples.hxx"
#include "Time.hxx"

#include <math.h>

#define DS3DMODE_NONE 0
#define DSBLOCK_NONE 0
#define DSBPLAY_NONE 0
#define DSBSTATUS_NONE 0
#define DSBPRIORITY_NONE 0

#define DEFAULT_DIRECT_SOUND_BUFFER_SIZE 1024

using namespace App::Windows;
using namespace Assets::Sounds;
using namespace Logger;
using namespace Mathematics;
using namespace Time;

namespace Sounds
{
    SoundDirectSoundSoundControllerContainer SoundDirectSoundSoundControllerState;

    GUID DSPROPSETID_VoiceManager = { 0x62a69bae, 0xdf9d, 0x11d1, 0x99, 0xa6, 0x00, 0xc0, 0x4f, 0xc9, 0x9d, 0x46 };
    GUID DSPROPSETID_EAX_ReverbProperties = { 0x4a4e6fc1, 0xc341, 0x11d1, 0xb7, 0x3a, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00 };
    
    // 0x005b4b60
    void SoundDirectSoundDeviceControllerConstructor(void)
    {
        SoundDirectSoundSoundControllerState.Instance.Self = (AbstractSoundDeviceControllerSelf*)&SoundDirectSoundSoundControllerState.SoundDirectDeviceSoundControllerSelf;
    }

    // 0x005b7af0
    AbstractSoundDeviceController* InitializeSoundDirectSoundDeviceController(const s32 indx)
    {
        if (!ReleaseSoundDirectSoundDeviceController(&SoundDirectSoundSoundControllerState.Instance)) { return NULL; }

        SoundDevice device;

        if (!EnumerateDirectSoundDevices(indx, &device)) { return NULL; }

        GUID* uid = NULL;

        if (!SoundDeviceState.EnumeratedDevices[indx].IsPrimary)
        {
            uid = &SoundDeviceState.EnumeratedDevices[indx].UID;
        }

        if (DSC(DirectSoundCreate(uid, &SoundDirectSoundSoundControllerState.DirectSound.Instance, NULL),
            "Unable to create Direct Sound instance.") == DS_OK)
        {
            if (DSC(SoundDirectSoundSoundControllerState.DirectSound.Instance->SetCooperativeLevel(*AppWindowState._HWND, DSSCL_PRIORITY),
                "Unable to set Direct Sound cooperative level.") == DS_OK)
            {
                DSBUFFERDESC desc =
                {
                    .dwSize = sizeof(DSBUFFERDESC),
                    .dwFlags = DSBCAPS_CTRL3D | DSBCAPS_PRIMARYBUFFER
                };

                if (DSC(SoundDirectSoundSoundControllerState.DirectSound.Instance->CreateSoundBuffer(
                    &desc, &SoundDirectSoundSoundControllerState.Buffers.Primary.Buffer, NULL),
                    "Unable to create primary sound buffer.") != DS_OK)
                {
                    desc.dwFlags = DSBCAPS_PRIMARYBUFFER;

                    if (DSC(SoundDirectSoundSoundControllerState.DirectSound.Instance->CreateSoundBuffer(
                        &desc, &SoundDirectSoundSoundControllerState.Buffers.Primary.Buffer, NULL),
                        "Unable to create primary sound buffer.") != DS_OK)
                    {
                        ReleaseSoundDirectSoundDeviceController(&SoundDirectSoundSoundControllerState.Instance);

                        return NULL;
                    }
                }

                if (AcquireSoundMixMode() == SoundMixMode::Advanced
                    && SoundDeviceState.EnumeratedDevices[indx].MixMode == SoundMixMode::Advanced)
                {
                    SoundDirectSoundSoundControllerState.Buffers.Primary.Buffer->QueryInterface(
                        IID_IDirectSound3DListener, (void**)&SoundDirectSoundSoundControllerState.DirectSound.Listener);
                }

                return &SoundDirectSoundSoundControllerState.Instance;
            }
        }

        ReleaseSoundDirectSoundDeviceController(&SoundDirectSoundSoundControllerState.Instance);

        return NULL;
    }

    // 0x005b5100
    BOOL ReleaseSoundDirectSoundDeviceController(AbstractSoundDeviceController* self)
    {
        const auto result = self->Self->Stop(self);

        for (u32 x = 0; x < 65; x++) // TODO constant relates to SoundUnk0x18 SoundUnk0x18Array[257]  ???
        {
            self->Self->ReleaseSoundSample(self, x);
        }

        if (SoundDirectSoundSoundControllerState.DirectSound.Listener != NULL)
        {
            SoundDirectSoundSoundControllerState.DirectSound.Listener->Release();
            SoundDirectSoundSoundControllerState.DirectSound.Listener = NULL;
        }

        if (SoundDirectSoundSoundControllerState.Buffers.Secondary.Buffer != NULL)
        {
            SoundDirectSoundSoundControllerState.Buffers.Secondary.Buffer->Release();
            SoundDirectSoundSoundControllerState.Buffers.Secondary.Buffer = NULL;
        }

        if (SoundDirectSoundSoundControllerState.EAX.Instance != NULL)
        {
            SoundDirectSoundSoundControllerState.EAX.Instance->Release();
            SoundDirectSoundSoundControllerState.EAX.Instance = NULL;
        }

        if (SoundDirectSoundSoundControllerState.Buffers.Primary.Buffer != NULL)
        {
            SoundDirectSoundSoundControllerState.Buffers.Primary.Buffer->Release();
            SoundDirectSoundSoundControllerState.Buffers.Primary.Buffer = NULL;
        }

        if (SoundDirectSoundSoundControllerState.DirectSound.Instance != NULL)
        {
            SoundDirectSoundSoundControllerState.DirectSound.Instance->Release();
            SoundDirectSoundSoundControllerState.DirectSound.Instance = NULL;
        }

        return result;
    }

    // 0x005b51d0
    // a.k.a. setSfxPos
    BOOL StartSoundDirectSoundDeviceController(AbstractSoundDeviceController* self)
    {
        f32 time = 0.1f; // TODO constant

        if (SoundDirectSoundSoundControllerState.Buffers.Secondary.Buffer != NULL)
        {
            *SoundDirectSoundSoundControllerState.Buffers.Secondary._ChannelBufferSize =
                (*SoundDirectSoundSoundControllerState.Buffers.Primary._BitsPerSample >> 3)
                * *SoundDirectSoundSoundControllerState.Buffers.Secondary._Count
                * *SoundDirectSoundSoundControllerState.Buffers.Primary._Channels;

            time = 0.5f * (f32)*SoundDirectSoundSoundControllerState.Buffers.Secondary._Count / (f32)*SoundDirectSoundSoundControllerState.Buffers.Primary._HZ; // TODO constant

            *SoundDirectSoundSoundControllerState.Buffers.Secondary._Unknown2 = 0; // TODO constant

            for (u32 x = 0; x < *SoundDirectSoundSoundControllerState.Buffers.Secondary._Unknown1; x++)
            {
                if (!FillSoundDirectSoundDeviceControllerSecondaryBuffer())
                {
                    self->Self->Stop(self);

                    return FALSE;
                }
            }
        }

        if (RestartSoundThread(time))
        {
            if (SoundDirectSoundSoundControllerState.Buffers.Secondary.Buffer == NULL) { return TRUE; }

            if (DSC(SoundDirectSoundSoundControllerState.Buffers.Secondary.Buffer->SetCurrentPosition(0),
                "Unable to set secondary sound buffer playback position to 0.") == DS_OK)
            {
                if (DSC(SoundDirectSoundSoundControllerState.Buffers.Secondary.Buffer->Play(NULL, DSBPRIORITY_NONE, DSBPLAY_LOOPING),
                    "Unable to play secondary sound buffer.") == DS_OK)
                {
                    return TRUE;
                }
            }
        }

        self->Self->Stop(self);

        return FALSE;
    }

    // 0x005b5340
    BOOL StopSoundDirectSoundDeviceController(AbstractSoundDeviceController* self)
    {
        auto result = TRUE;

        if (SoundDirectSoundSoundControllerState.Buffers.Secondary.Buffer != NULL)
        {
            if (DSC(SoundDirectSoundSoundControllerState.Buffers.Secondary.Buffer->Stop(),
                "Unable to stop secondary sound buffer.") != DS_OK)
            {
                result = FALSE;
            }
        }

        if (SoundDirectSoundSoundControllerState.Buffers.Primary.Buffer != NULL)
        {
            if (DSC(SoundDirectSoundSoundControllerState.Buffers.Primary.Buffer->Stop(),
                "Unable to stop primary sound buffer.") != DS_OK)
            {
                result = FALSE;
            }
        }

        for (u32 x = 0; x < 31; x++)// TODO
        {
            if (!StopSoundDirectSoundDeviceControllerSoundSample(x))
            {
                result = FALSE;
            }
        }

        if (StopSoundThread()) { return result; }

        return FALSE;
    }

    // 0x005b5600
    BOOL ResetSoundDirectSoundDeviceController(AbstractSoundDeviceController* self, const u32 bits, const u32 channels, const u32 hz, u32* count)
    {
        if (SoundDirectSoundSoundControllerState.EAX.Instance != NULL)
        {
            SoundDirectSoundSoundControllerState.EAX.Instance->Release();
            SoundDirectSoundSoundControllerState.EAX.Instance = NULL;
        }

        if (SoundDirectSoundSoundControllerState.Buffers.Primary.Buffer != NULL)
        {
            auto actual = channels;

            if (actual == SOUND_CHANNEL_COUNT_MONO)
            {
                if (DSC(SoundDirectSoundSoundControllerState.DirectSound.Instance->SetSpeakerConfig(DSSPEAKER_MONO),
                    "Unable to set speaker configuration.") != DS_OK)
                {
                    if (SoundDirectSoundSoundControllerState.Buffers.Secondary.Buffer != NULL)
                    {
                        SoundDirectSoundSoundControllerState.Buffers.Secondary.Buffer->Release();
                        SoundDirectSoundSoundControllerState.Buffers.Secondary.Buffer = NULL;
                    }

                    return FALSE;
                }
            }
            else if (actual == SOUND_CHANNEL_COUNT_STEREO)
            {
                if (DSC(SoundDirectSoundSoundControllerState.DirectSound.Instance->SetSpeakerConfig(DSSPEAKER_STEREO),
                    "Unable to set speaker configuration.") != DS_OK)
                {
                    if (SoundDirectSoundSoundControllerState.Buffers.Secondary.Buffer != NULL)
                    {
                        SoundDirectSoundSoundControllerState.Buffers.Secondary.Buffer->Release();
                        SoundDirectSoundSoundControllerState.Buffers.Secondary.Buffer = NULL;
                    }

                    return FALSE;
                }
            }
            else if (SOUND_CHANNEL_COUNT_STEREO < actual)
            {
                if (DSC(SoundDirectSoundSoundControllerState.DirectSound.Instance->SetSpeakerConfig(DSSPEAKER_SURROUND),
                    "Unable to set speaker configuration.") != DS_OK)
                {
                    if (SoundDirectSoundSoundControllerState.Buffers.Secondary.Buffer != NULL)
                    {
                        SoundDirectSoundSoundControllerState.Buffers.Secondary.Buffer->Release();
                        SoundDirectSoundSoundControllerState.Buffers.Secondary.Buffer = NULL;
                    }

                    return FALSE;
                }

                actual = SOUND_CHANNEL_COUNT_STEREO;
            }

            WAVEFORMATEX pf =
            {
                .wFormatTag = WAVE_FORMAT_PCM,
                .nChannels = (WORD)actual,
                .nSamplesPerSec = hz,
                .nAvgBytesPerSec = hz * (actual * (bits >> 3)),
                .nBlockAlign = (WORD)(actual * (bits >> 3)),
                .wBitsPerSample = (WORD)bits
            };

            if (DSC(SoundDirectSoundSoundControllerState.Buffers.Primary.Buffer->SetFormat(&pf),
                "Unable to set primary sound buffer format.") == DS_OK)
            {
                if (DSC(SoundDirectSoundSoundControllerState.Buffers.Primary.Buffer->GetFormat(&pf, sizeof(WAVEFORMATEX), NULL),
                    "Unable to get primary sound buffer format.") == DS_OK)
                {
                    *SoundDirectSoundSoundControllerState.Buffers.Primary._BitsPerSample = pf.wBitsPerSample;
                    *SoundDirectSoundSoundControllerState.Buffers.Primary._HZ = pf.nSamplesPerSec;
                    *SoundDirectSoundSoundControllerState.Buffers.Primary._Channels = pf.nChannels;

                    if (SoundDirectSoundSoundControllerState.EAX.Instance != NULL)
                    {
                        SoundDirectSoundSoundControllerState.EAX.Instance->Release();
                        SoundDirectSoundSoundControllerState.EAX.Instance = NULL;
                    }

                    WAVEFORMATEX tbf =
                    {
                        .wFormatTag = WAVE_FORMAT_PCM,
                        .nChannels = SOUND_CHANNEL_COUNT_MONO,
                        .nSamplesPerSec = SOUND_FREQUENCY_22050,
                        .nAvgBytesPerSec = SOUND_FREQUENCY_22050 * SOUND_BYTES_2,
                        .nBlockAlign = SOUND_BYTES_2,
                        .wBitsPerSample = SOUND_BITS_16
                    };

                    DSBUFFERDESC tbd =
                    {
                        .dwSize = sizeof(DSBUFFERDESC),
                        .dwFlags = DSBCAPS_CTRL3D | DSBCAPS_STATIC,
                        .dwBufferBytes = DEFAULT_DIRECT_SOUND_BUFFER_SIZE,
                        .lpwfxFormat = &tbf
                    };

                    IDirectSoundBuffer* tb;

                    if (DSC(SoundDirectSoundSoundControllerState.DirectSound.Instance->CreateSoundBuffer(&tbd, &tb, NULL),
                        "Unable to create temporary secondary sound buffer for property set creation.") == DS_OK)
                    {
                        IDirectSound3DBuffer* buff;

                        tb->QueryInterface(IID_IDirectSound3DBuffer, (void**)&buff);

                        if (buff == NULL)
                        {
                            if (tb != NULL)
                            {
                                tb->Release();
                                tb = NULL;
                            }
                        }
                        else
                        {
                            buff->QueryInterface(IID_IKsPropertySet, (void**)&SoundDirectSoundSoundControllerState.EAX.Instance);

                            if (buff != NULL)
                            {
                                buff->Release();
                                buff = NULL;
                            }

                            if (tb != NULL)
                            {
                                tb->Release();
                                tb = NULL;
                            }
                        }
                    }

                    if (SoundDirectSoundSoundControllerState.EAX.Instance != NULL)
                    {
                        DWORD value = DSPROPERTY_VMANAGER_MODE_AUTO;
                        SoundDirectSoundSoundControllerState.EAX.Instance->Set(DSPROPSETID_VoiceManager, DSPROPERTY_VMANAGER_MODE, NULL, NULL, &value, sizeof(DWORD));
                    }

                    if (AcquireSoundDeviceControllerMixMode() != SoundMixMode::None)
                    {
                        self = NULL;

                        return TRUE;
                    }

                    *SoundDirectSoundSoundControllerState.Buffers.Secondary._Unknown1 = 8; // TODO

                    // TODO
                    const auto c = (u32)roundf((AcquireMaximumSoftWareSoundLatency()
                        * *SoundDirectSoundSoundControllerState.Buffers.Primary._HZ) / (f32)*SoundDirectSoundSoundControllerState.Buffers.Secondary._Unknown1);

                    *SoundDirectSoundSoundControllerState.Buffers.Secondary._Count = (c + 0xf) & 0xfffffff0; // TODO

                    auto align = *SoundDirectSoundSoundControllerState.Buffers.Primary._Channels * (*SoundDirectSoundSoundControllerState.Buffers.Primary._BitsPerSample >> 3);

                    *SoundDirectSoundSoundControllerState.Buffers.Secondary._ChannelBufferSize = align * *SoundDirectSoundSoundControllerState.Buffers.Secondary._Count;

                    const auto size = *SoundDirectSoundSoundControllerState.Buffers.Secondary._Unknown1
                        * *SoundDirectSoundSoundControllerState.Buffers.Secondary._ChannelBufferSize;

                    DSBUFFERDESC sdesc =
                    {
                        .dwSize = sizeof(DSBUFFERDESC),
                        .dwBufferBytes = size,
                        .lpwfxFormat = &pf
                    };

                    if (SoundDirectSoundSoundControllerState.Buffers.Secondary.Buffer != NULL)
                    {
                        SoundDirectSoundSoundControllerState.Buffers.Secondary.Buffer->Release();
                        SoundDirectSoundSoundControllerState.Buffers.Secondary.Buffer = NULL;
                    }

                    if (DSC(SoundDirectSoundSoundControllerState.DirectSound.Instance->CreateSoundBuffer(&sdesc,
                        &SoundDirectSoundSoundControllerState.Buffers.Secondary.Buffer, NULL),
                        "Unable to create secondary sound buffer.") == DS_OK)
                    {
                        *count = *SoundDirectSoundSoundControllerState.Buffers.Secondary._Count;

                        return TRUE;
                    }
                }
            }
        }

        if (SoundDirectSoundSoundControllerState.Buffers.Secondary.Buffer != NULL)
        {
            SoundDirectSoundSoundControllerState.Buffers.Secondary.Buffer->Release();
            SoundDirectSoundSoundControllerState.Buffers.Secondary.Buffer = NULL;
        }

        return FALSE;
    }

    // 0x005b5c20
    BOOL PollSoundDirectSoundDeviceController(AbstractSoundDeviceController* self)
    {
        // Happy path in case the hardware supports mixing.
        if (AcquireSoundDeviceControllerMixMode() != SoundMixMode::None)
        {
            PollSoundDirectSoundSoundControllerSoundEffect();

            return TRUE;
        }

        // In case the hardware does not support the hardware mixing,
        // there is a need to perform sound mixing within the software manually.

        if (SoundDirectSoundSoundControllerState.Buffers.Secondary.Buffer == NULL) { return FALSE; }

        DWORD play, write;

        if (DSC(SoundDirectSoundSoundControllerState.Buffers.Secondary.Buffer->GetCurrentPosition(&play, &write),
            "Unable to acquire secondary sound buffer position.") != DS_OK)
        {
            return FALSE;
        }

        auto value = play / *SoundDirectSoundSoundControllerState.Buffers.Secondary._ChannelBufferSize;

        if (*SoundDirectSoundSoundControllerState.Buffers.Secondary._Unknown1 < value)
        {
            value = *SoundDirectSoundSoundControllerState.Buffers.Secondary._Unknown1 - 1;
        }

        if ((s32)value < 0) { value = 0; } // TODO constant

        do
        {
            if (value == *SoundDirectSoundSoundControllerState.Buffers.Secondary._Unknown2) { return TRUE; }
        } while (FillSoundDirectSoundDeviceControllerSecondaryBuffer());

        return FALSE;
    }

    // 0x005b5d00
    SoundMixMode QuerySoundDirectSoundDeviceControllerCapabilities(AbstractSoundDeviceController* self)
    {
        if (SoundDirectSoundSoundControllerState.DirectSound.Instance == NULL) { return SoundMixMode::None; }

        if (SoundDirectSoundSoundControllerState.DirectSound.Listener == NULL)
        {
            if (AcquireSoundMixMode() == SoundMixMode::None) { return SoundMixMode::None; }

            DSCAPS caps = { .dwSize = sizeof(DSCAPS) };

            if (DSC(SoundDirectSoundSoundControllerState.DirectSound.Instance->GetCaps(&caps),
                "Unable to query DirectSound capabilities.") == DS_OK)
            {
                return SoundMixMode::Simple;
            }

            return SoundMixMode::None;
        }

        return SoundMixMode::Advanced;
    }

    // 0x005b79e0
    void ApplySoundDirectSoundDeviceControllerOptions(AbstractSoundDeviceController* self)
    {
        if (SoundDirectSoundSoundControllerState.DirectSound.Listener != NULL)
        {
            DSC(SoundDirectSoundSoundControllerState.DirectSound.Listener->CommitDeferredSettings(),
                "Unable to commit sound buffer settings.");
        }
    }

    // 0x005b5dc0
    void SelectSoundDirectSoundDeviceControllerPosition(AbstractSoundDeviceController* self, const f64 x, const f64 y, const f64 z)
    {
        if (SoundDirectSoundSoundControllerState.DirectSound.Listener != NULL)
        {
            SoundDirectSoundSoundControllerState.DirectSound.Listener->SetPosition((f32)x, (f32)y, (f32)z, TRUE);
        }
    }

    // 0x005b5e00
    void SelectSoundDirectSoundDeviceControllerOrientation(AbstractSoundDeviceController* self, const f32 x, const f32 y, const f32 z, const f32 xt, const f32 yt, const f32 zt, const f32 xf, const f32 yf, const f32 zf)
    {
        if (SoundDirectSoundSoundControllerState.DirectSound.Listener != NULL)
        {
            SoundDirectSoundSoundControllerState.DirectSound.Listener->SetOrientation(xf, yf, zf, xt, yt, zt, TRUE);
        }
    }

    // 0x005b5e30
    void SelectSoundDirectSoundDeviceControllerVelocity(AbstractSoundDeviceController* self, const f32 x, const f32 y, const f32 z)
    {
        if (SoundDirectSoundSoundControllerState.DirectSound.Listener != NULL)
        {
            SoundDirectSoundSoundControllerState.DirectSound.Listener->SetVelocity(x, y, z, TRUE);
        }
    }

    // 0x005b5e60
    void SelectSoundDirectSoundDeviceControllerDistanceFactor(AbstractSoundDeviceController* self, const f32 value)
    {
        if (SoundDirectSoundSoundControllerState.DirectSound.Listener != NULL)
        {
            SoundDirectSoundSoundControllerState.DirectSound.Listener->SetDistanceFactor(value * 0.3048f, FALSE); // TODO
        }
    }

    // 0x005b7990
    void SelectSoundDirectSoundDeviceControllerEnvironment(AbstractSoundDeviceController* self, const f32 volume, const f32 time, const f32 damping)
    {
        if (SoundDirectSoundSoundControllerState.EAX.Instance == NULL) { return; }

        EAX_REVERBPROPERTIES value =
        {
            .environment = EAX_ENVIRONMENT_GENERIC,
            .fVolume = volume,
            .fDecayTime = time,
            .fDamping = damping
        };

        SoundDirectSoundSoundControllerState.EAX.Instance->Set(DSPROPSETID_EAX_ReverbProperties,
            DSPROPERTY_EAX_REVERBPROPERTY::DSPROPERTY_EAX_ALL,
            NULL, 0, (void**)&value, sizeof(EAX_REVERBPROPERTIES));
    }

    // 0x005b5e90
    // a.k.a. allocateSample
    u32 AllocateSoundDirectSoundDeviceControllerSoundSample(AbstractSoundDeviceController* self, const SoundSampleDescriptor* desc)
    {
        if (SoundDirectSoundSoundControllerState.DirectSound.Instance == NULL) { return 0; } // TODO constant

        u32 indx = 1; // TODO constant

        for (u32 x = indx; x < 65; x++) // TODO constant
        {
            if (SoundDirectSoundSoundControllerState.SoundUnk0x18Array[x].Buffer == NULL) { break; }

            indx = indx + 1;
        }

        if (64 < indx) { return 0; } // TODO constant

        WAVEFORMATEX bf = { .cbSize = sizeof(WAVEFORMATEX) };

        DSBUFFERDESC bd =
        {
            .dwSize = sizeof(DSBUFFERDESC),
            .dwFlags = *SoundDeviceControllerState._IsSoftwareDevice
                ? (DWORD)(DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY | DSBCAPS_LOCSOFTWARE)
                : (DWORD)(DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY),
            .lpwfxFormat = &bf
        };

        bd.dwFlags = bd.dwFlags | (SoundDirectSoundSoundControllerState.DirectSound.Listener == NULL ? DSBCAPS_CTRLPAN : DSBCAPS_CTRL3D);

        if (desc->Definition.Type == SoundSampleType::WAVYADPCM)
        {
            bf.wFormatTag = WAVE_FORMAT_YAMAHA_ADPCM;
            bf.nChannels = desc->Definition.Channels;
            bf.nSamplesPerSec = desc->Definition.HZ;
            bf.nBlockAlign = desc->Definition.Channels * (desc->Definition.BitsPerSample >> 3) / 2; // TODO constant
            bf.nAvgBytesPerSec = desc->Definition.HZ * desc->Definition.Channels * (desc->Definition.BitsPerSample >> 3) / 2; // TODO constant
            bf.wBitsPerSample = 4; // TODO constant

            bd.dwBufferBytes = desc->Definition.Length * desc->Definition.Channels / 2; // TODO constant
        }
        else
        {
            bf.wFormatTag = WAVE_FORMAT_PCM;
            bf.nChannels = desc->Definition.Channels;
            bf.nSamplesPerSec = desc->Definition.HZ;
            bf.nBlockAlign = desc->Definition.Channels * (desc->Definition.BitsPerSample >> 3);
            bf.nAvgBytesPerSec = desc->Definition.HZ * desc->Definition.Channels * (desc->Definition.BitsPerSample >> 3);
            bf.wBitsPerSample = desc->Definition.BitsPerSample;

            bd.dwBufferBytes = desc->Definition.Length * desc->Definition.Channels * (desc->Definition.BitsPerSample >> 3);
        }

        if (DSC(SoundDirectSoundSoundControllerState.DirectSound.Instance->CreateSoundBuffer(
            &bd, &SoundDirectSoundSoundControllerState.SoundUnk0x18Array[indx].Buffer, NULL),
            "Unable to create secondary sound buffer.") != DS_OK)
        {
            return 0; // TODO: constant
        }

        if (SoundDirectSoundSoundControllerState.SoundUnk0x18Array[indx].Buffer != NULL)
        {
            SoundDirectSoundSoundControllerState.SoundUnk0x18Array[indx].Unk2 = 256 * desc->Definition.Channels * (desc->Definition.BitsPerSample >> 3); // TODO constant
            SoundDirectSoundSoundControllerState.SoundUnk0x18Array[indx].Unk3 = bd.dwBufferBytes;
            SoundDirectSoundSoundControllerState.SoundUnk0x18Array[indx].Audio = NULL;

            return indx;
        }

        return 0; // TODO: constant
    }

    // 0x005b61a0
    void ReleaseSoundDirectSoundDeviceControllerSoundSample(AbstractSoundDeviceController* self, const s32 indx)
    {
        if (SoundDirectSoundSoundControllerState.SoundUnk0x18Array[indx].Buffer != NULL)
        {
            SoundDirectSoundSoundControllerState.SoundUnk0x18Array[indx].Buffer->Release();
            SoundDirectSoundSoundControllerState.SoundUnk0x18Array[indx].Buffer = NULL;
        }

        SoundDirectSoundSoundControllerState.SoundUnk0x18Array[indx].Unk2 = 0;
        SoundDirectSoundSoundControllerState.SoundUnk0x18Array[indx].Unk3 = 0;

        SoundDirectSoundSoundControllerState.SoundUnk0x18Array[indx].Audio = NULL;
        SoundDirectSoundSoundControllerState.SoundUnk0x18Array[indx].AudioSize = 0;
    }

    // 0x005b6200
    // a.k.a. lockSample
    void* LockSoundDirectSoundDeviceControllerSoundSample(AbstractSoundDeviceController* self, const s32 indx, const s32 offset, const s32 length)
    {
        if (indx < 1 || 64 < indx
            || SoundDirectSoundSoundControllerState.SoundUnk0x18Array[indx].Buffer == NULL
            || SoundDirectSoundSoundControllerState.SoundUnk0x18Array[indx].Unk2 < 1) // TODO constants
        {
            return NULL;
        }

        if (SoundDirectSoundSoundControllerState.SoundUnk0x18Array[indx].Audio != NULL)
        {
            LogError("Unable to lock sound sample, it is already locked.");
        }

        auto offsetValue = offset * SoundDirectSoundSoundControllerState.SoundUnk0x18Array[indx].Unk2;
        auto offsetMask = offsetValue >> 0x1f;// TODO
        offsetValue = (offsetValue + offsetMask * (-0x100)) - (offsetMask << 7 < 0); // TODO

        auto lenOffset = (((offset >> 24) << 16) | (offset >> 8)) & 0xffffffe; // TODO
        auto len = (length + 0x1fU) & 0xffffffe0; // TODO
        auto lenVal = SoundDirectSoundSoundControllerState.SoundUnk0x18Array[indx].Unk3 - lenOffset;// TODO

        if (lenVal < len) { len = lenVal; }

        if (DSC(SoundDirectSoundSoundControllerState.SoundUnk0x18Array[indx].Buffer->Lock(
            lenOffset, len, &SoundDirectSoundSoundControllerState.SoundUnk0x18Array[indx].Audio,
            (LPDWORD)&SoundDirectSoundSoundControllerState.SoundUnk0x18Array[indx].AudioSize, NULL, NULL, DSBLOCK_NONE),
            "Unable to lock hardware sound buffer.") != DS_OK)
        {
            return NULL;
        }

        if ((SoundDirectSoundSoundControllerState.SoundUnk0x18Array[indx].Unk3 + -0x20) <= (len + lenOffset)) // TODO
        {
            const auto ptr = (void*)((addr)SoundDirectSoundSoundControllerState.SoundUnk0x18Array[indx].Audio
                + ((addr)SoundDirectSoundSoundControllerState.SoundUnk0x18Array[indx].Unk3 - lenOffset) + -0x20); // TODO

            ZeroMemory(ptr, 0x20); // TODO
        }

        return (void*)((addr)SoundDirectSoundSoundControllerState.SoundUnk0x18Array[indx].Audio + ((offsetValue >> 8) - lenOffset));
    }

    // 0x005b6360
    // a.k.a. unlockSample
    void UnlockSoundDirectSoundDeviceControllerSoundSample(AbstractSoundDeviceController* self, const s32 indx, const s32 offset, const s32 length)
    {
        if (indx < 1 || 64 < indx
            || SoundDirectSoundSoundControllerState.SoundUnk0x18Array[indx].Buffer == NULL
            || SoundDirectSoundSoundControllerState.SoundUnk0x18Array[indx].Unk2 < 1) // TODO constants
        {
            LogError("Unable to unlock sound sample, invalid index %d.", indx);
        }

        if (SoundDirectSoundSoundControllerState.SoundUnk0x18Array[indx].Audio != NULL)
        {
            DSC(SoundDirectSoundSoundControllerState.SoundUnk0x18Array[indx].Buffer->Unlock(
                SoundDirectSoundSoundControllerState.SoundUnk0x18Array[indx].Audio, SoundDirectSoundSoundControllerState.SoundUnk0x18Array[indx].AudioSize, NULL, 0),
                "Unable to unlock hardware sound buffer.");

            SoundDirectSoundSoundControllerState.SoundUnk0x18Array[indx].Audio = NULL;
        }
    }

    // 0x005b6440
    // a.k.a. allocateSfx
    u32 AllocateSoundDirectSoundDeviceControllerSoundEffect(AbstractSoundDeviceController* self, const s32 indx)
    {
        if (indx < 1 || 64 < indx
            || SoundDirectSoundSoundControllerState.SoundUnk0x18Array[indx].Buffer == NULL
            || SoundDirectSoundSoundControllerState.SoundUnk0x18Array[indx].Unk2 < 1) // TODO: constants
        {
            LogError("Unable to allocate sound effect, invalid index %d.", indx);
        }

        if (SoundDirectSoundSoundControllerState.DirectSound.Instance == NULL)
        {
            LogError("Unable to allocate sound effect, Direct Sound is not initialized.");
        }

        u32 index = 1;

        for (u32 x = index; x < 31; x++) // TODO constants
        {
            if (SoundDirectSoundSoundControllerState.SoundUnk0x24Array[x].Buffer == NULL) { break; }

            index = index + 1;
        }

        if (30 < index) // TODO constants
        {
            // TODO FUN_0042f410((int *)PTR_DAT_00671410,s_allocateSfx_-_006573f6);

            return 0; // TODO constants
        }

        u32 ai = 0; // TODO name

        for (ai = SoundDirectSoundSoundControllerState.SoundUnk0x18Array[indx].Unk1;
            ai != 0 && SoundDirectSoundSoundControllerState.SoundUnk0x24Array[ai].Buffer != SoundDirectSoundSoundControllerState.SoundUnk0x18Array[indx].Buffer;
            ai = SoundDirectSoundSoundControllerState.SoundUnk0x24Array[ai].Unk5) {
        }// TODO constants

        if (ai == 0) // TODO constants
        {
            SoundDirectSoundSoundControllerState.SoundUnk0x24Array[index].Buffer = SoundDirectSoundSoundControllerState.SoundUnk0x18Array[indx].Buffer;

            SoundDirectSoundSoundControllerState.SoundUnk0x24Array[index].Unk3 = indx;

            if (SoundDirectSoundSoundControllerState.SoundUnk0x18Array[indx].Unk1 != 0) // TODO constants
            {
                SoundDirectSoundSoundControllerState.SoundUnk0x24Array[SoundDirectSoundSoundControllerState.SoundUnk0x18Array[indx].Unk1].Unk4 = index;
            }
        }
        else
        {
            if (DSC(SoundDirectSoundSoundControllerState.DirectSound.Instance->DuplicateSoundBuffer(
                SoundDirectSoundSoundControllerState.SoundUnk0x18Array[indx].Buffer, &SoundDirectSoundSoundControllerState.SoundUnk0x24Array[index].Buffer),
                "Unable to duplicate sound buffer.") != DS_OK)
            {
                return 0; // TODO constants
            }

            if (SoundDirectSoundSoundControllerState.SoundUnk0x24Array[index].Buffer == NULL) { return 0; } // TODO constants

            SoundDirectSoundSoundControllerState.SoundUnk0x24Array[SoundDirectSoundSoundControllerState.SoundUnk0x18Array[indx].Unk1].Unk4 = index;
            SoundDirectSoundSoundControllerState.SoundUnk0x24Array[index].Unk3 = indx;
        }

        SoundDirectSoundSoundControllerState.SoundUnk0x24Array[index].Unk4 = 0;
        SoundDirectSoundSoundControllerState.SoundUnk0x24Array[index].Unk5 = SoundDirectSoundSoundControllerState.SoundUnk0x18Array[indx].Unk1;

        SoundDirectSoundSoundControllerState.SoundUnk0x18Array[indx].Unk1 = index;

        if (SoundDirectSoundSoundControllerState.DirectSound.Listener != NULL)
        {
            DSC(SoundDirectSoundSoundControllerState.SoundUnk0x24Array[index].Buffer->QueryInterface(
                IID_IDirectSound3DBuffer, (void**)&SoundDirectSoundSoundControllerState.SoundUnk0x24Array[index].AdvancedBuffer),
                "Unable to create a 3D sound buffer from a secondary sound buffer.");

            if (SoundDirectSoundSoundControllerState.SoundUnk0x24Array[index].AdvancedBuffer == NULL)
            {
                StopSoundDirectSoundDeviceControllerSoundSample(index);

                return 0; // TODO constants
            }
        }

        SoundDirectSoundSoundControllerState.SoundUnk0x24Array[index].HZ = -1; // TODO constant
        SoundDirectSoundSoundControllerState.SoundUnk0x24Array[index].Unk9 = -1; // TODO constant
        SoundDirectSoundSoundControllerState.SoundUnk0x24Array[index].Pan = DSBPAN_CENTER;
        SoundDirectSoundSoundControllerState.SoundUnk0x24Array[index].Volume = DSBVOLUME_MAX;

        return index;
    }

    // 0x005569c0
    // a.k.a. setSfxOpt
    BOOL SelectSoundDirectSoundDeviceControllerSoundEffectOptions(AbstractSoundDeviceController* self, SoundEffect* effect, const u32 mode) // TODO enum
    {
        const auto indx = effect->UnknownIndex;

        if (indx < 1 || 30 < indx || SoundDirectSoundSoundControllerState.SoundUnk0x24Array[indx].Buffer == NULL) // TODO constants
        {
            LogError("Unable to select sound effect options, invalid index %d.", indx);
        }

        if (SoundDirectSoundSoundControllerState.SoundUnk0x24Array[indx].Buffer == NULL) { return FALSE; }

        auto result = TRUE;

        if (SoundDirectSoundSoundControllerState.SoundUnk0x24Array[indx].AdvancedBuffer != NULL)
        {
            auto volume = MIN_SOUND_VOLUME;
            auto minimumDistance = 0.0f;
            auto maximumDistance = 0.0f;

            if (mode & 0x88) // TODO constants
            {
                minimumDistance = effect->Descriptor.MinimumDistance;
                maximumDistance = effect->Descriptor.MaximumDistance;

                const auto value = AcquireSoundEffectChannelVolume(effect->Descriptor.NextChannelIndex);

                volume = value;

                if ((effect->Descriptor.Unk30 & 1) == 0 && MIN_SOUND_VOLUME < value) // TODO constant
                {
                    volume = MAX_SOUND_VOLUME;
                    minimumDistance = minimumDistance * value;
                    maximumDistance = maximumDistance * value;
                }
            }

            if (mode & 2) // TODO constants
            {
                if (DSC(SoundDirectSoundSoundControllerState.SoundUnk0x24Array[indx].AdvancedBuffer->SetPosition(
                    (f32)effect->Descriptor.Location.X, (f32)effect->Descriptor.Location.Y, (f32)effect->Descriptor.Location.Z, TRUE),
                    "Unable to set secondary sound hardware buffer position.") != DS_OK)
                {
                    result = FALSE;
                }
            }

            if (mode & 4) // TODO constants
            {
                if (DSC(SoundDirectSoundSoundControllerState.SoundUnk0x24Array[indx].AdvancedBuffer->SetVelocity(
                    effect->Descriptor.Velocity.X, effect->Descriptor.Velocity.Y, effect->Descriptor.Velocity.Z, TRUE),
                    "Unable to set secondary sound hardware buffer velocity.") != DS_OK)
                {
                    result = FALSE;
                }
            }

            if (mode & 8) // TODO constants
            {
                auto value = AcquireSoundDirectSoundSoundControllerVolume(effect->Descriptor.Volume * volume);

                auto v1 = value - SoundDirectSoundSoundControllerState.SoundUnk0x24Array[indx].Volume;
                auto v2 = v1 >> 0x1f;// TODO

                if (mode == U32_MAX || 1 < ((v1 ^ v2) - v2)) // TODO
                {
                    if (DSC(SoundDirectSoundSoundControllerState.SoundUnk0x24Array[indx].Buffer->SetVolume(value),
                        "Unable to set secondary sound hardware buffer volume.") != DS_OK)
                    {
                        result = FALSE;
                    }
                }

                SoundDirectSoundSoundControllerState.SoundUnk0x24Array[indx].Volume = value;
            }

            if (mode & 0x10) // TODO constants
            {
                auto value = AcquireSoundDirectSoundSoundControllerFrequency(effect->Sample->Descriptor.Definition.HZ * effect->Descriptor.HZ);

                auto v1 = value - SoundDirectSoundSoundControllerState.SoundUnk0x24Array[indx].HZ;
                auto v2 = v1 >> 0x1f;// TODO

                if (mode == U32_MAX || 1 < ((v1 ^ v2) - v2)) // TODO
                {
                    if (DSC(SoundDirectSoundSoundControllerState.SoundUnk0x24Array[indx].Buffer->SetFrequency(value),
                        "Unable to set secondary sound hardware buffer frequency.") != DS_OK)
                    {
                        result = FALSE;
                    }
                }

                SoundDirectSoundSoundControllerState.SoundUnk0x24Array[indx].HZ = value;
            }

            if (mode & 0x40)
            {
                auto options = DS3DMODE_NORMAL;

                if (effect->Descriptor.Unk30 & 1) // TODO constants
                {
                    options = DS3DMODE_DISABLE;
                }

                if (mode == U32_MAX || options != SoundDirectSoundSoundControllerState.SoundUnk0x24Array[indx].Unk9)
                {
                    if (DSC(SoundDirectSoundSoundControllerState.SoundUnk0x24Array[indx].AdvancedBuffer->SetMode(options, DS3D_DEFERRED),
                        "Unable to set sound buffer into spatial mode.") != DS_OK)
                    {
                        result = FALSE;
                    }

                    SoundDirectSoundSoundControllerState.SoundUnk0x24Array[indx].Unk9 = options;
                }
            }

            if (mode & 0x80) // TODO constants
            {
                if (DSC(SoundDirectSoundSoundControllerState.SoundUnk0x24Array[indx].AdvancedBuffer->SetMinDistance(minimumDistance, TRUE),
                    "Unable to set secondary sound hardware buffer minimum distance.") != DS_OK)
                {
                    result = FALSE;
                }

                if (DSC(SoundDirectSoundSoundControllerState.SoundUnk0x24Array[indx].AdvancedBuffer->SetMaxDistance(maximumDistance, TRUE),
                    "Unable to set secondary sound hardware buffer maximum distance.") != DS_OK)
                {
                    result = FALSE;
                }
            }

            if (mode & 0x100) // TODO constants
            {
                if (DSC(SoundDirectSoundSoundControllerState.SoundUnk0x24Array[indx].Buffer->SetCurrentPosition((u32)round(effect->Position)),
                    "Unable to set secondary sound hardware buffer playback position.") != DS_OK)
                {
                    result = FALSE;
                }
            }

            effect->Options = effect->Options & (~mode | 0x40000000);// TODO

            if (!result)
            {
                LogMessage("[ERROR] [SOUND] Unable to set secondary sound hardware buffer options.\n");
            }

            return result;
        }

        if (mode & 0xca) // TODO constants
        {
            auto maximum = F32_MIN;
            const auto count = AcquireSoundChannelCount();

            for (u32 x = 0; x < count; x++) // TODO constants
            {
                if (maximum < effect->Pans[x])
                {
                    maximum = effect->Pans[x];
                }
            }

            // The value in lPan is measured in hundredths of a decibel(dB), in the range of DSBPAN_LEFT to DSBPAN_RIGHT.
            // These values are currently defined in Dsound.h as - 10,000 and 10,000 respectively.
            // The value DSBPAN_LEFT means the right channel is attenuated by 100 dB.
            // The value DSBPAN_RIGHT means the left channel is attenuated by 100 dB.
            // The neutral value is DSBPAN_CENTER, defined as zero.This value of 0 in the lplPan parameter'
            // means that both channels are at full volume(they are attenuated by 0 decibels).
            // At any setting other than DSBPAN_CENTER, one of the channels is at full volume and
            // the other is attenuated.

            auto val = DSBPAN_CENTER;

            if (count == 2 && 0.0f < maximum) // TODO constant
            {
                if (effect->Pans[1] <= effect->Pans[0])
                {
                    if (effect->Pans[0] < effect->Pans[1])
                    {
                        if (0.0f < effect->Pans[1]) // TODO constant
                        {
                            val = (s32)round(AcquireUnknownSoundValue101(-868.589 * (f64)log2f(effect->Pans[0] / effect->Pans[1]))); // TODO constants

                            if (val < DSBPAN_CENTER || DSBPAN_LEFT < val)
                            {
                                val = DSBPAN_RIGHT;
                            }
                        }
                        else
                        {
                            val = DSBPAN_LEFT;
                        }
                    }
                }
                else if (effect->Pans[0] <= 0.0f) // TODO constant
                {
                    val = DSBPAN_RIGHT;
                }
                else
                {
                    val = (s32)round(AcquireUnknownSoundValue101(868.589 * (f64)log2f(effect->Pans[1] / effect->Pans[0]))); // TODO constants

                    if (val < DSBPAN_RIGHT || DSBPAN_CENTER < val)
                    {
                        val = DSBPAN_CENTER;
                    }
                }
            }

            {
                const auto value = AcquireSoundDirectSoundSoundControllerVolume(maximum);

                auto v1 = value - SoundDirectSoundSoundControllerState.SoundUnk0x24Array[indx].Volume;

                auto v2 = v1 >> 0x1f;// TODO

                if (mode != U32_MAX || 1 < ((v1 ^ v2) - v2)) // TODO
                {
                    if (DSC(SoundDirectSoundSoundControllerState.SoundUnk0x24Array[indx].Buffer->SetVolume(value),
                        "Unable to set secondary sound hardware buffer volume.") != DS_OK)
                    {
                        result = FALSE;
                    }
                }

                SoundDirectSoundSoundControllerState.SoundUnk0x24Array[indx].Volume = value;
            }

            {
                auto v1 = val - SoundDirectSoundSoundControllerState.SoundUnk0x24Array[indx].Pan;

                auto v2 = v1 >> 0x1f;// TODO

                if (mode != U32_MAX || 1 < ((v1 ^ v2) - v2)) // TODO
                {
                    if (DSC(SoundDirectSoundSoundControllerState.SoundUnk0x24Array[indx].Buffer->SetPan(val),
                        "Unable to set secondary sound hardware buffer pan.") != DS_OK)
                    {
                        result = FALSE;
                    }
                }

                SoundDirectSoundSoundControllerState.SoundUnk0x24Array[indx].Pan = val;
            }
        }

        if (mode & 0x96)//TODO constant
        {
            auto value = AcquireSoundDirectSoundSoundControllerFrequency(effect->Sample->Descriptor.Definition.HZ * effect->HZ);

            auto v1 = value - SoundDirectSoundSoundControllerState.SoundUnk0x24Array[indx].HZ;
            auto v2 = v1 >> 0x1f;// TODO

            if (mode != U32_MAX || 1 < ((v1 ^ v2) - v2)) // TODO
            {
                if (DSC(SoundDirectSoundSoundControllerState.SoundUnk0x24Array[indx].Buffer->SetFrequency(value),
                    "Unable to set secondary sound hardware buffer frequency.") != DS_OK)
                {
                    result = FALSE;
                }
            }

            SoundDirectSoundSoundControllerState.SoundUnk0x24Array[indx].HZ = value;
        }

        if (mode & 0x100)//TODO constant
        {
            if (DSC(SoundDirectSoundSoundControllerState.SoundUnk0x24Array[indx].Buffer->SetCurrentPosition((u32)round(effect->Position)),
                "Unable to set secondary sound hardware buffer playback position.") != DS_OK) // TODO constant
            {
                result = FALSE;
            }
        }

        effect->Options = effect->Options & (~mode | 0x40000000);// TODO

        if (!result) { LogMessage("[ERROR] [SOUND] Unable to set secondary sound hardware buffer options.\n"); }

        return result;
    }

    // 0x005b7140
    // a.k.a. getSfxPlaybackPos
    f64 AcquireSoundDirectSoundDeviceControllerSoundEffectPosition(AbstractSoundDeviceController* self, SoundEffect* effect)
    {
        const auto indx = effect->UnknownIndex;

        if (indx < 1 || 30 < indx || SoundDirectSoundSoundControllerState.SoundUnk0x24Array[indx].Buffer == NULL) // TODO constant
        {
            LogError("Unable to get sound effect playback position, invalid index.");
        }

        if (effect->Sample != NULL)
        {
            DWORD play = 0;
            DWORD write = 0;

            if (DSC(SoundDirectSoundSoundControllerState.SoundUnk0x24Array[indx].Buffer->GetCurrentPosition(&play, &write),
                "Unable to acquire secondary sound buffer playback position.") != DS_OK)
            {
                return -1.0; // TODO constant
            }

            return (f64)AcquireSoundSampleDescriptorValue204(&effect->Sample->Descriptor, play);
        }

        return -1.0; // TODO constant
    }

    // 0x005b7270
    // a.k.a. startSfx
    BOOL StartSoundDirectSoundDeviceControllerSoundEffect(AbstractSoundDeviceController* self, SoundEffect* effect)
    {
        const auto indx = effect->UnknownIndex;

        if (indx < 1 || 30 < indx || SoundDirectSoundSoundControllerState.SoundUnk0x24Array[indx].Buffer == NULL)
        {
            LogError("Unable to start sound effect, invalid index %d.", indx);
        }

        if (!self->Self->SelectSoundEffectOptions(self, effect, U32_MAX)) { return FALSE; } // TODO constant

        if (effect->Sample == NULL) { LogError("Unable to start sound effect, sound sample is missing."); }

        if (effect->Sample->Descriptor.LoopMode == SoundLoopMode::Unknown2 || 0 < effect->Sample->Descriptor.Unk6) // TODO constants
        {
            LogError("Unable to start sound effect, exotic jump sequences not allowed for hardware mixed sounds.");
        }

        DWORD options = DSBPLAY_NONE;

        if (effect->Sample->Descriptor.LoopMode == SoundLoopMode::Looping && -1 < effect->Sample->Descriptor.Unk6) // TODO constants
        {
            options = options | DSBPLAY_LOOPING;
        }

        if (effect->Sample->Length != effect->Sample->Descriptor.Definition.Length)
        {
            options = options | DSBPLAY_LOOPING;
        }

        const auto result = SoundDirectSoundSoundControllerState.SoundUnk0x24Array[indx].Buffer->Play(NULL, DSBPRIORITY_NONE, options);

        if (result == DSERR_BADFORMAT)
        {
            if (DSC(SoundDirectSoundSoundControllerState.SoundUnk0x24Array[indx].Buffer->SetFrequency(DSBFREQUENCY_ORIGINAL),
                "Unable to set secondary hardware sound buffer frequency.") != DS_OK)
            {
                return FALSE;
            }

            if (DSC(SoundDirectSoundSoundControllerState.SoundUnk0x24Array[indx].Buffer->SetVolume(DSBVOLUME_MIN),
                "Unable to set secondary hardware sound buffer volume.") != DS_OK)
            {
                return FALSE;
            }

            if (DSC(SoundDirectSoundSoundControllerState.SoundUnk0x24Array[indx].Buffer->Play(NULL, DSBPRIORITY_NONE, DSBPLAY_NONE),
                "Unable to play secondary hardware sound buffer.") != DS_OK)
            {
                return FALSE;
            }

            if (SoundDirectSoundSoundControllerState.SoundUnk0x24Array[indx].HZ != -1 &&
                SoundDirectSoundSoundControllerState.SoundUnk0x24Array[indx].HZ != effect->Sample->Descriptor.Definition.HZ) // TODO constants
            {
                const auto hz = AcquireSoundDirectSoundSoundControllerFrequency(effect->Sample->Descriptor.Definition.HZ * effect->Descriptor.HZ);

                if (1 < (u32)(hz - SoundDirectSoundSoundControllerState.SoundUnk0x24Array[indx].HZ))
                {
                    if (DSC(SoundDirectSoundSoundControllerState.SoundUnk0x24Array[indx].Buffer->SetFrequency(hz),
                        "Unable to set secondary hardware sound buffer frequency.") != DS_OK)
                    {
                        return FALSE;
                    }

                    SoundDirectSoundSoundControllerState.SoundUnk0x24Array[indx].HZ = hz;
                }
            }

            if (DSC(SoundDirectSoundSoundControllerState.SoundUnk0x24Array[indx].Buffer->SetCurrentPosition(0),
                "Unable to set secondary hardware sound buffer playback position.") != DS_OK)
            {
                return FALSE;
            }

            if (DSC(SoundDirectSoundSoundControllerState.SoundUnk0x24Array[indx].Buffer->SetVolume(SoundDirectSoundSoundControllerState.SoundUnk0x24Array[indx].Volume),
                "Unable to set secondary hardware sound buffer volume.") != DS_OK)
            {
                return FALSE;
            }
        }
        else if (DSC(result, "Unable to play secondary hardware sound buffer.") != DS_OK)
        {
            return FALSE;
        }

        effect->Options = effect->Options & 0xBFFFFFFF; // TODO constants

        return TRUE;
    }

    // 0x005b76f0
    // a.k.a. killSfx
    void ReleaseSoundDirectSoundDeviceControllerSoundEffect(AbstractSoundDeviceController* self, SoundEffect* effect)
    {
        const auto indx = effect->UnknownIndex;

        if (indx < MIN_ACTIVE_SOUND_BUFFER_POOL_INDEX || (MAX_ACTIVE_SOUND_BUFFER_POOL_SIZE - 1) < indx
            || SoundDirectSoundSoundControllerState.SoundUnk0x24Array[indx].Buffer == NULL)
        {
            LogMessage("Unable to release sound effect, invalid index %d.\n", indx);

            return;
        }

        StopSoundDirectSoundDeviceControllerSoundSample(indx);
    }

    // 0x005b7090
    BOOL AcquireSoundDirectSoundDeviceControllerSoundEffectState(AbstractSoundDeviceController* self, SoundEffect* effect)
    {
        const auto indx = effect->UnknownIndex;

        if (indx < MIN_ACTIVE_SOUND_BUFFER_POOL_INDEX || (MAX_ACTIVE_SOUND_BUFFER_POOL_SIZE - 1) < indx
            || SoundDirectSoundSoundControllerState.SoundUnk0x24Array[indx].Buffer == NULL) {
            return FALSE;
        }

        DWORD status = DSBSTATUS_NONE;

        if (DSC(SoundDirectSoundSoundControllerState.SoundUnk0x24Array[indx].Buffer->GetStatus(&status),
            "Unable to get active sound effect buffer status.") != DS_OK)
        {
            return FALSE;
        }

        if (status & DSBSTATUS_PLAYING) { return TRUE; }

        return FALSE;
    }

    // 0x005b50c0
    s32 AcquireSoundDirectSoundSoundControllerFrequency(const f32 value)
    {
        return Clamp((s32)roundf(value), DSBFREQUENCY_MIN, DSBFREQUENCY_MAX);
    }

    // 0x005b5020
    s32 AcquireSoundDirectSoundSoundControllerVolume(const f32 value)
    {
        if (value <= MIN_SOUND_VOLUME) { return DSBVOLUME_MIN; }

        if (MAX_SOUND_VOLUME <= value) { return DSBVOLUME_MAX; }

        const auto result = (s32)round(AcquireUnknownSoundValue101((f64)log2f(value) * 868.589)); // TODO constant

        return Clamp(result, DSBVOLUME_MIN, DSBVOLUME_MAX);
    }

    // 0x005c1660
    // a.k.a. pollSfx
    void PollSoundDirectSoundSoundControllerSoundEffect(void)
    {
        if (*SoundState.Lock._Count < DEFAULT_SOUND_LOCK_COUNT) { LogError("Unable to poll sound effect, sound sample is not locked."); }

        auto time1 = AcquireTime(); // TODO type, name
        auto time2 = time1 - *SoundState._SoundTime1; // TODO type, name

        if (time2 < 0 || 0x1cccb < time2) // TODO constants
        {
            *SoundState._SoundTime1 = time1;

            if (SoundState.UnknownSoundCount1 < 2) { ComputeSoundEffectsPositions(); } // TODO constants

            f32 value = MIN_SOUND_VOLUME;

            if (AcquireSoundDeviceControllerMixMode() != SoundMixMode::None)
            {
                value = Clamp(time2 * 0.0000008477105f, MIN_SOUND_VOLUME, 0.25f); // TODO constants
            }

            for (u32 x = 0; x < 64; x++) // TODO: constants
            {
                auto effect = &SoundState.Effects._Cache[x];

                if (ComputeSoundEffect(effect, value)) { FUN_005bc8d0(effect); }
            }
        }
    }

    // 0x005b4ef0
    BOOL StopSoundDirectSoundDeviceControllerSoundSample(const s32 indx)
    {
        auto result = TRUE;

        if (SoundDirectSoundSoundControllerState.SoundUnk0x24Array[indx].Buffer != NULL)
        {
            result = DSC(SoundDirectSoundSoundControllerState.SoundUnk0x24Array[indx].Buffer->Stop(),
                "Unable to stop secondary sound effect buffer.") == DS_OK;

            if (SoundDirectSoundSoundControllerState.SoundUnk0x24Array[indx].AdvancedBuffer != NULL)
            {
                SoundDirectSoundSoundControllerState.SoundUnk0x24Array[indx].AdvancedBuffer->Release();
                SoundDirectSoundSoundControllerState.SoundUnk0x24Array[indx].AdvancedBuffer = NULL;
            }

            if (SoundDirectSoundSoundControllerState.SoundUnk0x24Array[indx].Unk5 != 0) // TODO constant
            {
                SoundDirectSoundSoundControllerState.SoundUnk0x24Array[SoundDirectSoundSoundControllerState.SoundUnk0x24Array[indx].Unk5].Unk4 = SoundDirectSoundSoundControllerState.SoundUnk0x24Array[indx].Unk4;
            }

            if (indx == SoundDirectSoundSoundControllerState.SoundUnk0x18Array[SoundDirectSoundSoundControllerState.SoundUnk0x24Array[indx].Unk3].Unk1)
            {
                SoundDirectSoundSoundControllerState.SoundUnk0x18Array[SoundDirectSoundSoundControllerState.SoundUnk0x24Array[indx].Unk3].Unk1 = SoundDirectSoundSoundControllerState.SoundUnk0x24Array[indx].Unk5;
            }
            else
            {
                SoundDirectSoundSoundControllerState.SoundUnk0x24Array[SoundDirectSoundSoundControllerState.SoundUnk0x24Array[indx].Unk4].Unk5 = SoundDirectSoundSoundControllerState.SoundUnk0x24Array[indx].Unk5;
            }

            if (SoundDirectSoundSoundControllerState.SoundUnk0x24Array[indx].Buffer != NULL
                && SoundDirectSoundSoundControllerState.SoundUnk0x24Array[indx].Buffer != SoundDirectSoundSoundControllerState.SoundUnk0x18Array[SoundDirectSoundSoundControllerState.SoundUnk0x24Array[indx].Unk3].Buffer)
            {
                SoundDirectSoundSoundControllerState.SoundUnk0x24Array[indx].Buffer->Release();
                SoundDirectSoundSoundControllerState.SoundUnk0x24Array[indx].Buffer = NULL;
            }
        }

        SoundDirectSoundSoundControllerState.SoundUnk0x24Array[indx].Buffer = NULL;

        SoundDirectSoundSoundControllerState.SoundUnk0x24Array[indx].Unk5 = 0;
        SoundDirectSoundSoundControllerState.SoundUnk0x24Array[indx].Unk4 = 0;
        SoundDirectSoundSoundControllerState.SoundUnk0x24Array[indx].Unk3 = 0;

        return result;
    }

    typedef const void(CDECLAPI* FUN_005C1340) (void**, u32, u32, u32, u32, u32); // TODO
    static FUN_005C1340 FUN_005c1340 = (FUN_005C1340)0x005c1340; // TODO
    
    // 0x005b4d20
    BOOL FillSoundDirectSoundDeviceControllerSecondaryBuffer(void)
    {
        if (SoundDirectSoundSoundControllerState.Buffers.Secondary.Buffer == NULL) { return FALSE; }

        void* audio1;
        DWORD audio1size;
        void* audio2;
        DWORD audio2size;

        const auto off = *SoundDirectSoundSoundControllerState.Buffers.Secondary._Unknown2
            * *SoundDirectSoundSoundControllerState.Buffers.Secondary._ChannelBufferSize;

        if (DSC(SoundDirectSoundSoundControllerState.Buffers.Secondary.Buffer->Lock(off,
            *SoundDirectSoundSoundControllerState.Buffers.Secondary._ChannelBufferSize,
            &audio1, &audio1size, &audio2, &audio2size, DSBLOCK_NONE),
            "Unable to lock secondary sound buffer.") != DS_OK)
        {
            return FALSE;
        }

        if (audio2 == NULL && audio1size == *SoundDirectSoundSoundControllerState.Buffers.Secondary._ChannelBufferSize)
        {
            void* data[MAX_SOUND_CHANNEL_COUNT];

            for (u32 x = 0; x < *SoundDirectSoundSoundControllerState.Buffers.Primary._Channels; x++)
            {
                const auto offset = x * (*SoundDirectSoundSoundControllerState.Buffers.Primary._BitsPerSample >> 3);
                data[x] = (void*)((addr)audio1 + (addr)offset);
            }

            // TODO NOT IMPLEMENTED !!!  FillSoundDeviceControllerBuffer(data,
            FUN_005c1340(data,
                *SoundDirectSoundSoundControllerState.Buffers.Primary._BitsPerSample,
                *SoundDirectSoundSoundControllerState.Buffers.Primary._Channels,
                *SoundDirectSoundSoundControllerState.Buffers.Primary._HZ,
                *SoundDirectSoundSoundControllerState.Buffers.Secondary._Count,
                (*SoundDirectSoundSoundControllerState.Buffers.Primary._BitsPerSample >> 3) * *SoundDirectSoundSoundControllerState.Buffers.Primary._Channels);

            *SoundDirectSoundSoundControllerState.Buffers.Secondary._Unknown2 = *SoundDirectSoundSoundControllerState.Buffers.Secondary._Unknown2 + 1;

            if (*SoundDirectSoundSoundControllerState.Buffers.Secondary._Unknown1 <= *SoundDirectSoundSoundControllerState.Buffers.Secondary._Unknown2)
            {
                *SoundDirectSoundSoundControllerState.Buffers.Secondary._Unknown2 = 0;
            }

            if (DSC(SoundDirectSoundSoundControllerState.Buffers.Secondary.Buffer->Unlock(audio1, audio1size, audio2, audio2size),
                "Unable to unlock secondary sound buffer.") != DS_OK)
            {
                return FALSE;
            }

            return TRUE;
        }

        return FALSE;
    }

    // 0x005b5430
    // INLINE
    void InitializeSoundDirectSoundDeviceControllerEnvironment(void)
    {
        if (SoundDirectSoundSoundControllerState.EAX.Instance != NULL)
        {
            SoundDirectSoundSoundControllerState.EAX.Instance->Release();
            SoundDirectSoundSoundControllerState.EAX.Instance = NULL;
        }

        WAVEFORMATEX format =
        {
            .wFormatTag = WAVE_FORMAT_PCM,
            .nChannels = SOUND_CHANNEL_COUNT_MONO,
            .nSamplesPerSec = SOUND_FREQUENCY_22050,
            .nAvgBytesPerSec = SOUND_FREQUENCY_22050 * SOUND_BYTES_2,
            .nBlockAlign = SOUND_BYTES_2,
            .wBitsPerSample = SOUND_BITS_16
        };

        DSBUFFERDESC desc =
        {
            .dwSize = sizeof(DSBUFFERDESC),
            .dwFlags = DSBCAPS_CTRL3D | DSBCAPS_STATIC,
            .dwBufferBytes = DEFAULT_DIRECT_SOUND_BUFFER_SIZE,
            .lpwfxFormat = &format
        };

        IDirectSoundBuffer* buffer;

        if (DSC(SoundDirectSoundSoundControllerState.DirectSound.Instance->CreateSoundBuffer(&desc, &buffer, NULL),
            "Unable to create temporary secondary sound buffer for property set creation.") == DS_OK)
        {
            IDirectSound3DBuffer* buff;

            buffer->QueryInterface(IID_IDirectSound3DBuffer, (void**)&buff);

            if (buff == NULL)
            {
                if (buffer != NULL)
                {
                    buffer->Release();
                    buffer = NULL;
                }
            }
            else
            {
                buff->QueryInterface(IID_IKsPropertySet, (void**)&SoundDirectSoundSoundControllerState.EAX.Instance);

                if (buff != NULL)
                {
                    buff->Release();
                    buff = NULL;
                }

                if (buffer != NULL)
                {
                    buffer->Release();
                    buffer = NULL;
                }
            }
        }
    }
}