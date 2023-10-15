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

#include "Sounds.Options.hxx"
#include "Sounds.Devices.hxx"
#include "Sounds.Effects.hxx"
#include "Sounds.hxx"
#include "Strings.hxx"

#define SOUND_DEVICE_NAME_PROPERTY_NAME "DeviceName"
#define SOUND_RECORDING_DEVICE_NAME_PROPERTY_NAME "RecordingDeviceName"

#define SOUND_ACCELERATION_MODE_PROPERTY_NAME "AccelEnabled"
#define SOUND_MAX_SOFTWARE_LATENCY_PROPERTY_NAME "MaxSwLatency"
#define SOUND_BITS_PROPERTY_NAME "Bits"
#define SOUND_CHANNELS_PROPERTY_NAME "Channels"
#define SOUND_FREQUENCY_PROPERTY_NAME "Hz"
#define SOUND_MUTE_PROPERTY_NAME "Mute"
#define SOUND_EFFECT_CHANNEL_STATE_PROPERTY_NAME "SfxChannel%dEnabled"
#define SOUND_EFFECT_CHANNEL_VOLUME_PROPERTY_NAME "SfxChannel%dVol"

using namespace IO::Ini;
using namespace Sounds;
using namespace Strings;

namespace Sounds::Options
{
    // 0x005c0790
    void LoadSoundOptions(AbstractIni* ini)
    {
        char buffer[MAX_SOUND_DEVICE_NAME_LENGTH];
        ZeroMemory(buffer, MAX_SOUND_DEVICE_NAME_LENGTH);

        ini->Self->Read(ini, SOUND_DEVICE_NAME_PROPERTY_NAME, buffer, MAX_SOUND_DEVICE_NAME_LENGTH);

        ReleaseSoundDeviceController();

        for (auto x = 0; x < AcquireSoundDeviceCount(); x++)
        {
            SoundDevice device;
            AcquireSoundDevice(x, &device);

            if (EqualStrings(device.Name, buffer)) { SelectSoundDevice(x); break; }
        }

        ZeroMemory(buffer, MAX_SOUND_DEVICE_NAME_LENGTH);

        ini->Self->Read(ini, SOUND_RECORDING_DEVICE_NAME_PROPERTY_NAME, buffer, MAX_SOUND_DEVICE_NAME_LENGTH);

        ReleaseSoundRecordingDeviceController();

        for (auto x = 0; x < AcquireSoundRecordingDeviceCount(); x++)
        {
            SoundRecordingDevice device;
            AcquireSoundRecordingDevice(x, &device);

            if (EqualStrings(device.Name, buffer)) { SelectSoundRecordingDevice(x); break; }
        }

        {
            auto value = AcquireSoundMixMode();
            ReadAbstractIniInteger(ini, SOUND_ACCELERATION_MODE_PROPERTY_NAME, (s32*)&value);
            SelectSoundMixMode(value);
        }

        {
            auto value = AcquireMaximumSoftWareSoundLatency();
            ReadAbstractIniReal(ini, SOUND_MAX_SOFTWARE_LATENCY_PROPERTY_NAME, &value);
            SelectMaximumSoftWareLatency(value);
        }

        {
            u32 bits = 0;
            u32 channels = 0;
            u32 hz = 0;

            AcquireSoundOutputOptions(&bits, &channels, &hz);
            ReadAbstractIniInteger(ini, SOUND_BITS_PROPERTY_NAME, (s32*)&bits);
            ReadAbstractIniInteger(ini, SOUND_CHANNELS_PROPERTY_NAME, (s32*)&channels);
            ReadAbstractIniInteger(ini, SOUND_FREQUENCY_PROPERTY_NAME, (s32*)&hz);
            SelectSoundDeviceControllerSoundMode(bits, channels, hz);
        }

        {
            auto value = AcquireSoundDeviceControllerMuteMode();
            ReadAbstractIniInteger(ini, SOUND_MUTE_PROPERTY_NAME, (s32*)&value);
            SelectSoundDeviceControllerMuteMode(value);
        }

        for (u32 x = 0; x < *SoundState.Effects.Channels._Count; x++)
        {
            {
                sprintf(buffer, SOUND_EFFECT_CHANNEL_STATE_PROPERTY_NAME, x);

                auto value = AcquireSoundEffectChannelState(x);
                ReadAbstractIniInteger(ini, buffer, &value);
                SelectSoundEffectChannelState(x, value);
            }

            {
                sprintf(buffer, SOUND_EFFECT_CHANNEL_VOLUME_PROPERTY_NAME, x);

                auto value = AcquireSoundEffectChannelVolume(x);
                ReadAbstractIniReal(ini, buffer, &value);
                SelectSoundEffectChannelVolume(x, value);
            }
        }
    }

    // 0x005c0a90
    void SaveSoundOptions(IO::Ini::AbstractIni* ini)
    {
        char buffer[MAX_SOUND_DEVICE_NAME_LENGTH];
        ZeroMemory(buffer, MAX_SOUND_DEVICE_NAME_LENGTH);

        if (INVALID_SOUND_DEVICE_COUNT < SoundDeviceState.SoundDeviceIndex
            && SoundDeviceState.SoundDeviceIndex < AcquireSoundDeviceCount())
        {
            SoundDevice device;
            AcquireSoundDevice(SoundDeviceState.SoundDeviceIndex, &device);
            strcpy(buffer, device.Name);
        }

        ini->Self->Write(ini, SOUND_DEVICE_NAME_PROPERTY_NAME, buffer);

        {
            u32 bits = 0;
            u32 channels = 0;
            u32 hz = 0;

            AcquireSoundOutputOptions(&bits, &channels, &hz);
            WriteAbstractIniInteger(ini, SOUND_BITS_PROPERTY_NAME, (s32)bits);
            WriteAbstractIniInteger(ini, SOUND_CHANNELS_PROPERTY_NAME, (s32)channels);
            WriteAbstractIniInteger(ini, SOUND_FREQUENCY_PROPERTY_NAME, (s32)hz);
        }

        WriteAbstractIniInteger(ini, SOUND_ACCELERATION_MODE_PROPERTY_NAME, (s32)AcquireSoundMixMode());
        WriteAbstractIniReal(ini, SOUND_MAX_SOFTWARE_LATENCY_PROPERTY_NAME, AcquireMaximumSoftWareSoundLatency());
        WriteAbstractIniInteger(ini, SOUND_MUTE_PROPERTY_NAME, AcquireSoundDeviceControllerMuteMode());

        for (u32 x = 0; x < *SoundState.Effects.Channels._Count; x++)
        {
            {
                sprintf(buffer, SOUND_EFFECT_CHANNEL_STATE_PROPERTY_NAME, x);
                WriteAbstractIniInteger(ini, buffer, AcquireSoundEffectChannelState(x));
            }

            {
                sprintf(buffer, SOUND_EFFECT_CHANNEL_VOLUME_PROPERTY_NAME, x);
                WriteAbstractIniReal(ini, buffer, AcquireSoundEffectChannelVolume(x));
            }
        }
    }
}