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

#include "Direct.Sound.hxx"
#include "Logger.hxx"
#include "Sounds.Devices.DirectSound.hxx"
#include "Sounds.Devices.hxx"
#include "Sounds.hxx"

#include <stdio.h>

#define DS_INCOMPLETE 0x08780014

using namespace Logger;

namespace Sounds
{
    // 0x005b7730
    BOOL CALLBACK EnumerateDirectSoundDevicesCallback(const LPGUID guid, const LPCSTR description, const LPCSTR module, const LPVOID context)
    {
        if ((MAX_SOUND_ENUMERATED_DEVICE_COUNT - 1) < SoundDeviceState.EnumeratedDeviceCount) { return FALSE; }

        IDirectSound* instance;

        if (DSC(DirectSoundCreate(guid, &instance, NULL), "Unable to initialize Direct Sound.") != DS_OK) { return TRUE; }

        if (instance == NULL) { return TRUE; }

        DSCAPS caps = { .dwSize = sizeof(DSCAPS) };

        DSC(instance->GetCaps(&caps), "Unable to query sound device capabilities.");

        instance->Release();

        if (SoundDeviceState.EnumeratedDeviceCount == MIN_SOUND_DEVICE_COUNT)
        {
            auto device = &SoundDeviceState.EnumeratedDevices[SoundDeviceState.EnumeratedDeviceCount];

            ZeroMemory(device, sizeof(SoundDeviceDetail));

            if (guid == NULL)
            {
                device->IsPrimary = TRUE;
            }
            else
            {
                device->IsPrimary = FALSE;
                device->UID = *guid;
            }

            device->MixMode = SoundMixMode::None;

            if (caps.dwFlags & DSCAPS_EMULDRIVER)
            {
                device->IsEmulated = TRUE;
            }
            else
            {
                device->IsEmulated = FALSE;
                device->MixMode = SoundMixMode::Simple;

                if (caps.dwFreeHwMixingStaticBuffers != 0)
                {
                    device->MixMode = SoundMixMode::Advanced;
                }

                if (caps.dwFreeHw3DStaticBuffers == 0)
                {
                    *SoundDeviceControllerState._IsSoftwareDevice = TRUE;

                    LogMessage("[SOUND] No onboard RAM detected. Forcing sound samples to load in system memory.\n");
                }
                else
                {
                    LogMessage("[SOUND] Onboard RAM detected. Sound samples will be loaded into available onboard memory.\n");
                }
            }

            strncpy(device->Name, description, MAX_SOUND_DEVICE_NAME_LENGTH - 1);

            SoundDeviceState.EnumeratedDeviceCount = SoundDeviceState.EnumeratedDeviceCount + 1;
        }

        return TRUE;
    }

    // 0x005b7a40
    BOOL EnumerateDirectSoundDevices(const s32 indx, SoundDevice* device)
    {
        if (SoundDeviceState.EnumeratedDeviceCount < MIN_SOUND_DEVICE_COUNT)
        {
            SoundDeviceState.EnumeratedDeviceCount = MIN_SOUND_DEVICE_COUNT;

            DirectSoundEnumerateA(EnumerateDirectSoundDevicesCallback, NULL);
        }

        if (INVALID_SOUND_DEVICE_COUNT < indx && indx < SoundDeviceState.EnumeratedDeviceCount)
        {
            const auto dev = &SoundDeviceState.EnumeratedDevices[indx];

            device->Index = indx;
            device->Type = SoundDeviceType::DirectSound;
            device->Unk2 = 0; // TODO constant
            device->MixMode = dev->MixMode;
            device->IsEmulated = dev->IsEmulated;
            device->IsPrimary = dev->IsPrimary;

            sprintf(device->Name, "DirectSound: %s", dev->Name);

            return TRUE;
        }

        return FALSE;
    }
}

namespace Sounds
{
    namespace Checks
    {
        const char* Convert(const LRESULT code)
        {
            switch (code)
            {
            case DS_INCOMPLETE: { return "The method succeeded, but not all the optional effects were obtained."; }
            case DS_NO_VIRTUALIZATION: { return "The buffer was created, but another 3D algorithm was substituted."; }
            case DS_OK: { return "The method succeeded."; }
            case DSERR_ACCESSDENIED: { return "The request failed because access was denied."; }
            case DSERR_ALLOCATED: { return "The request failed because resources, such as a priority level, were already in use by another caller."; }
            case DSERR_ALREADYINITIALIZED: { return "The object is already initialized."; }
            case DSERR_BADFORMAT: { return "The specified wave format is not supported."; }
            case DSERR_BADSENDBUFFERGUID: { return "The GUID specified in an audio path file does not match a valid mix-in buffer."; }
            case DSERR_BUFFERLOST: { return "The buffer memory has been lost and must be restored."; }
            case DSERR_BUFFERTOOSMALL: { return "The buffer size is not great enough to enable effects processing."; }
            case DSERR_CONTROLUNAVAIL: { return "The buffer control (volume, pan, and so on) requested by the caller is not available."; }
            case DSERR_DS8_REQUIRED: { return "A DirectSound object of class CLSID_DirectSound8 or later is required for the requested functionality. "; }
            case DSERR_FXUNAVAILABLE: { return "The effects requested could not be found on the system, or they are in the wrong order or in the wrong location; for example, an effect expected in hardware was found in software."; }
            case DSERR_GENERIC: { return "An undetermined error occurred inside the DirectSound subsystem."; }
            case DSERR_INVALIDCALL: { return "This function is not valid for the current state of this object."; }
            case DSERR_INVALIDPARAM: { return "An invalid parameter was passed to the returning function."; }
            case DSERR_NOAGGREGATION: { return "The object does not support aggregation."; }
            case DSERR_NODRIVER: { return "No sound driver is available for use, or the given GUID is not a valid DirectSound device ID."; }
            case DSERR_NOINTERFACE: { return "The requested COM interface is not available."; }
            case DSERR_OBJECTNOTFOUND: { return "The requested object was not found."; }
            case DSERR_OTHERAPPHASPRIO: { return "Another application has a higher priority level, preventing this call from succeeding."; }
            case DSERR_OUTOFMEMORY: { return "The DirectSound subsystem could not allocate sufficient memory to complete the caller's request."; }
            case DSERR_PRIOLEVELNEEDED: { return "A cooperative level of DSSCL_PRIORITY or higher is required."; }
            case DSERR_SENDLOOP: { return "A circular loop of send effects was detected."; }
            case DSERR_UNINITIALIZED: { return "The Initialize method has not been called or has not been called successfully before other methods were called."; }
            case DSERR_UNSUPPORTED: { return "The function called is not supported at this time."; }
            }

            return "Unknown sound error code.";
        }
    }

    LRESULT DSC(const LRESULT code, const char* message)
    {
        if (code != DS_OK) { LogMessage("[ERROR] [SOUND] %s %s\n", message, Checks::Convert(code)); }

        return code;
    }
}