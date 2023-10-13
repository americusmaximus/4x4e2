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
#include "Logger.hxx"
#include "Native.Basic.hxx"
#include "Sounds.hxx"

#include <math.h>
#include <mmsystem.h>

#define MCI_NOERROR MMSYSERR_BASE
#define MCI_COMMAND_OPTIONS_NONE 0

#define DEFAULT_MCI_DEVICE_ID 0

#define MAX_MCI_ERROR_BUFFER_SIZE 256

#define MCI_DEVICE_TYPE_CD_AUDIO_NAME "cdaudio"

#define INVALID_MCI_TRACK 0

#define MCI_COMMAND_CLOSE_NAME "close"
#define MCI_COMMAND_OPEN_NAME "open"
#define MCI_COMMAND_PAUSE_NAME "pause"
#define MCI_COMMAND_PLAY_NAME "play"
#define MCI_COMMAND_SET_TIME_FORMAT_NAME "setTimeFormat"
#define MCI_COMMAND_STOP_NAME "stop"

#define DEFAULT_MIXER_ID 0

#define MIXER_CHANNEL_COUNT_MONO 1

#define MIXER_VOLUME_MULTIPLIER (65535.0f)

using namespace App::Windows;
using namespace Logger;

namespace Sounds
{
    SoundDiskContainer SoundDiskState;

    // 0x005c1ba0
    void SDE(SoundDisk* self, const char* command, const MCIERROR code)
    {
        char buffer[MAX_MCI_ERROR_BUFFER_SIZE];

        mciGetErrorStringA(code, buffer, (MAX_MCI_ERROR_BUFFER_SIZE - 1));

        LogMessage("Redbook Audio error while trying to %s.\n Error: %s\n", command, buffer);

        ReleaseSoundDisk(self);
    }

    // 0x005c17f0
    BOOL OpenSoundDiskDevice(void)
    {
        if (SoundDiskState.IsOpen) { return TRUE; }

        MCI_OPEN_PARMSA open =
        {
            .wDeviceID = DEFAULT_MCI_DEVICE_ID,
            .lpstrDeviceType = MCI_DEVICE_TYPE_CD_AUDIO_NAME
        };

        if (mciSendCommandA(DEFAULT_MCI_DEVICE_ID, MCI_OPEN, MCI_OPEN_TYPE | MCI_OPEN_SHAREABLE | MCI_WAIT, (DWORD_PTR)&open) != MCI_NOERROR)
        {
            LogMessage("[ERROR] [SOUND] Unable to open CD device.\n");

            return FALSE;
        }

        // Set the time format to track/minute/second/frame (TMSF).
        MCI_SET_PARMS set = { .dwTimeFormat = MCI_FORMAT_TMSF };

        if (mciSendCommandA(open.wDeviceID, MCI_SET, MCI_SET_TIME_FORMAT, (DWORD_PTR)&set) != MCI_NOERROR)
        {
            LogMessage("[ERROR] [SOUND] Unable to set CD time format.\n");

            return FALSE;
        }

        SoundDiskState.DeviceID = open.wDeviceID;
        SoundDiskState.IsOpen = TRUE;

        return TRUE;
    }

    // 0x005c1ad0
    BOOL StopSoundDiskDevice(void)
    {
        if (SoundDiskState.IsOpen)
        {
            if (mciSendCommandA(SoundDiskState.DeviceID, MCI_STOP, MCI_WAIT, NULL) != MCI_NOERROR)
            {
                LogMessage("[ERROR] [SOUND] Unable to stop CD device.\n");

                return FALSE;
            }
        }

        return TRUE;
    }

    // 0x005c1b80
    SoundDisk* InitializeSoundDisk(SoundDisk* self)
    {
        ReleaseSoundDisk(self);

        return self;
    }

    // 0x005c1d10
    void ReleaseSoundDisk(SoundDisk* self)
    {
        mciSendCommandA(MCI_ALL_DEVICE_ID, MCI_CLOSE, MCI_WAIT, NULL);

        self->IsOpen = FALSE;
        self->IsPaused = FALSE;
        self->IsPlaying = FALSE;

        self->Volume = 0.0f;

        self->Track = INVALID_MCI_TRACK;

        self->PlayFrom = 0;
        self->PlayTo = 0;
    }

    // 0x005c22f0
    BOOL AcquireSoundDiskState(SoundDisk* self, const u32 track)
    {
        MCI_STATUS_PARMS status = { .dwItem = MCI_STATUS_MODE | MCI_STATUS_LENGTH };

        mciSendCommandA(self->ID, MCI_STATUS, MCI_STATUS_ITEM, (DWORD_PTR)&status);

        if (status.dwReturn == MCI_NOERROR) { return FALSE; }

        MCI_STATUS_PARMS trs =
        {
            .dwItem = MCI_VD_FORMAT_TRACK,
            .dwTrack = track
        };

        mciSendCommandA(self->ID, MCI_STATUS, MCI_STATUS_ITEM | MCI_TRACK, (DWORD_PTR)&trs);

        return trs.dwReturn == MCI_CDA_TRACK_AUDIO;
    }

    // 0x005c1ff0
    BOOL StopSoundDisk(SoundDisk* self)
    {
        if (!self->IsPlaying) { return TRUE; }

        const auto result = mciSendCommandA(self->ID, MCI_STOP, MCI_COMMAND_OPTIONS_NONE, NULL);

        if (result != MCI_NOERROR) { SDE(self, MCI_COMMAND_STOP_NAME, result); return FALSE; }

        self->IsPaused = FALSE;
        self->IsPlaying = FALSE;

        return TRUE;
    }

    // 0x005c2040
    BOOL PauseSoundDisk(SoundDisk* self)
    {
        if (!self->IsPlaying) { return FALSE; }

        const auto result = mciSendCommandA(self->ID, MCI_PAUSE, MCI_COMMAND_OPTIONS_NONE, NULL);

        if (result != MCI_NOERROR) { SDE(self, MCI_COMMAND_PAUSE_NAME, result); return FALSE; }

        self->IsPaused = TRUE;
        self->IsPlaying = FALSE;

        return TRUE;
    }

    // 0x005c1cc0
    u32 AcquireSoundDiskTrackPosition(SoundDisk* self, const u32 track)
    {
        MCI_STATUS_PARMS status =
        {
            .dwItem = MCI_STATUS_POSITION,
            .dwTrack = track
        };

        mciSendCommandA(self->ID, MCI_STATUS, MCI_STATUS_ITEM | MCI_TRACK, (DWORD_PTR)&status);

        return status.dwReturn;
    }

    // 0x005c1c70
    u32 AcquireSoundDiskTrackLength(SoundDisk* self, const u32 track)
    {
        MCI_STATUS_PARMS status =
        {
            .dwItem = MCI_STATUS_LENGTH,
            .dwTrack = track
        };

        mciSendCommandA(self->ID, MCI_STATUS, MCI_STATUS_ITEM | MCI_TRACK, (DWORD_PTR)&status);

        return status.dwReturn;
    }

    // 0x005c1c10
    BOOL SelectSoundDiskTimeFormat(SoundDisk* self)
    {
        MCI_SET_PARMS set = { .dwTimeFormat = MCI_FORMAT_MILLISECONDS };

        const auto result = mciSendCommandA(self->ID, MCI_SET, MCI_SET_TIME_FORMAT, (DWORD_PTR)&set);

        if (result != MCI_NOERROR) { SDE(self, MCI_COMMAND_SET_TIME_FORMAT_NAME, result); return FALSE; }

        return TRUE;
    }

    // 0x005c1d60
    BOOL OpenSoundDisk(SoundDisk* self)
    {
        if (self->IsOpen) { return TRUE; }

        MCI_OPEN_PARMSA open = { .lpstrDeviceType = (LPCSTR)MCI_DEVTYPE_CD_AUDIO };

        const auto result = mciSendCommandA(DEFAULT_MCI_DEVICE_ID, MCI_OPEN, MCI_OPEN_TYPE | MCI_OPEN_TYPE_ID | MCI_OPEN_SHAREABLE, (DWORD_PTR)&open);

        if (result != MCI_NOERROR) { SDE(self, MCI_COMMAND_OPEN_NAME, result); return FALSE; }

        self->ID = open.wDeviceID;

        SelectSoundDiskTimeFormat(self);

        self->IsOpen = TRUE;

        return TRUE;
    }

    // 0x005c1df0
    BOOL CloseSoundDisk(SoundDisk* self)
    {
        if (!self->IsOpen) { return TRUE; }

        if (self->IsPlaying) { StopSoundDisk(self); }

        MCI_GENERIC_PARMS close = { 0 };

        const auto result = mciSendCommandA(self->ID, MCI_CLOSE, MCI_COMMAND_OPTIONS_NONE, (DWORD_PTR)&close);

        if (result != MCI_NOERROR) { SDE(self, MCI_COMMAND_CLOSE_NAME, result); return FALSE; }

        self->IsOpen = FALSE;

        return TRUE;
    }

    // 0x005c1e60
    BOOL PlaySoundDisk(SoundDisk* self, const u32 track, const f32 position)
    {
        if (!OpenSoundDisk(self)) { return FALSE; }

        if (self->IsPlaying) { return TRUE; }

        auto trk = track;

        if (trk == INVALID_MCI_TRACK)
        {
            trk = self->Track;

            if (trk == INVALID_MCI_TRACK) { return FALSE; }
        }

        if (!AcquireSoundDiskState(self, track))
        {
            LogMessage("[ERROR] [SOUND] Unable to play background music. Either there is no CD present or no audio track was found.\n");

            return FALSE;
        }

        if (!self->IsPaused)
        {
            self->PlayFrom = (u32)round(position * 1000.0f) + AcquireSoundDiskTrackPosition(self, track); // TODO constant
            self->PlayTo = AcquireSoundDiskTrackLength(self, track) + AcquireSoundDiskTrackPosition(self, track) - 2000; // TODO constant

            MCI_PLAY_PARMS play =
            {
                .dwCallback = (DWORD)*AppWindowState._HWND,
                .dwFrom = self->PlayFrom,
                .dwTo = self->PlayTo
            };

            const auto result = mciSendCommandA(self->ID, MCI_PLAY, MCI_TRACK | MCI_WAIT | MCI_NOTIFY, (DWORD_PTR)&play);

            if (result != MCI_NOERROR) { SDE(self, MCI_COMMAND_PLAY_NAME, result); return FALSE; }
        }
        else
        {
            MCI_PLAY_PARMS play = { .dwCallback = (DWORD)*AppWindowState._HWND };

            const auto result = mciSendCommandA(self->ID, MCI_PLAY, MCI_NOTIFY, (DWORD_PTR)&play);

            if (result != MCI_NOERROR) { SDE(self, MCI_COMMAND_PLAY_NAME, result); return FALSE; }
        }

        self->IsPlaying = TRUE;
        self->IsPaused = FALSE;

        self->Track = track;

        return TRUE;
    }

    // 0x005c2090
    BOOL SelectSoundDiskVolume(SoundDisk* self, const f32 volume)
    {
        if (volume == self->Volume) { return FALSE; }

        LogMessage("Setting CD audio volume to: %f\n", volume);

        HMIXER handle;

        if (mixerOpen(&handle, DEFAULT_MIXER_ID, NULL, NULL, MIXER_OBJECTF_MIXER) != MMSYSERR_NOERROR)
        {
            LogMessage("Unable to open the mixer. Another application may be using it (e.g. Window's volume control).\n");

            mixerClose(handle);

            return FALSE;
        }

        MIXERLINEA line =
        {
            .cbStruct = sizeof(MIXERLINEA),
            .dwComponentType = MIXERLINE_COMPONENTTYPE_SRC_COMPACTDISC
        };

        HMIXEROBJ mixer = (HMIXEROBJ)handle;

        if (mixerGetLineInfoA(mixer, &line, MIXER_GETLINEINFOF_COMPONENTTYPE) != MMSYSERR_NOERROR)
        {
            LogMessage("Unable to get the mixer line.\n");

            mixerClose(handle);

            return FALSE;
        }

        MIXERCONTROLA control = { .cbStruct = sizeof(MIXERCONTROLA) };

        MIXERLINECONTROLSA cl =
        {
            .cbStruct = sizeof(MIXERLINECONTROLSA),
            .dwLineID = line.dwLineID,
            .dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME,
            .cControls = MIXER_CHANNEL_COUNT_MONO,
            .cbmxctrl = sizeof(MIXERCONTROLA),
            .pamxctrl = &control
        };

        if (mixerGetLineControlsA(mixer, &cl, MIXER_GETLINECONTROLSF_ONEBYTYPE) != MMSYSERR_NOERROR)
        {
            LogMessage("Unable to get the control.\n");

            mixerClose(handle);

            return FALSE;
        }

        const auto vol = (u32)round(volume * MIXER_VOLUME_MULTIPLIER);

        MIXERCONTROLDETAILS details =
        {
            .cbStruct = sizeof(MIXERCONTROLDETAILS),
            .dwControlID = control.dwControlID,
            .cChannels = MIXER_CHANNEL_COUNT_MONO,
            .cbDetails = sizeof(u32),
            .paDetails = (void*)&vol
        };

        if (mixerGetControlDetailsA(mixer, &details, MIXER_GETCONTROLDETAILSF_VALUE) != MMSYSERR_NOERROR)
        {
            LogMessage("Unable to set the current volume.\n");

            mixerClose(handle);

            return FALSE;
        }

        mixerClose(handle);

        self->Volume = volume;

        return TRUE;
    }

    // 0x005c18d0
    BOOL PlaySoundDiskInBack(const u32 value, const f32 offset)
    {
        if (!SoundDiskState.IsOpen)
        {
            MCI_OPEN_PARMSA open =
            {
                .wDeviceID = DEFAULT_MCI_DEVICE_ID,
                .lpstrDeviceType = MCI_DEVICE_TYPE_CD_AUDIO_NAME
            };

            if (mciSendCommandA(DEFAULT_MCI_DEVICE_ID, MCI_OPEN, MCI_OPEN_TYPE | MCI_OPEN_SHAREABLE | MCI_WAIT, (DWORD_PTR)&open) != MCI_NOERROR)
            {
                LogMessage("[ERROR] [SOUND] Unable to open CD device.\n");

                return FALSE;
            }

            // Set the time format to track/minute/second/frame (TMSF).
            MCI_SET_PARMS set = { .dwTimeFormat = MCI_FORMAT_TMSF };

            if (mciSendCommandA(open.wDeviceID, MCI_SET, MCI_SET_TIME_FORMAT, (DWORD_PTR)&set) != MCI_NOERROR)
            {
                LogMessage("[ERROR] [SOUND] Unable to set CD time format.\n");

                return FALSE;
            }

            SoundDiskState.DeviceID = open.wDeviceID;
            SoundDiskState.IsOpen = TRUE;
        }

        const auto v1 = roundf(offset / 60.0f); // TODO constant
        const auto v2 = roundf(offset - v1 * offset); // TODO
        const auto v3 = round((offset - AcquireUnknownSoundValue101(offset)) * 60.0); // TODO constant

        MCI_PLAY_PARMS play =
        {
            .dwFrom = (((u32)((u16)v3)) << 24) | (((u32)((u8)v2)) << 16) | (((u32)roundf(v1) & 0xffffU) << 8) | value & 0xff, // TODO
            .dwTo = (value + 1) & 0xff // TODO
        };

        if (mciSendCommandA(SoundDiskState.DeviceID, MCI_PLAY, MCI_TRACK | MCI_WAIT, (DWORD_PTR)&play) != MCI_NOERROR)
        {
            if (mciSendCommandA(SoundDiskState.DeviceID, MCI_PLAY, MCI_FROM | MCI_NOTIFY, (DWORD_PTR)&play) != MCI_NOERROR)
            {
                LogMessage("[ERROR] [SOUND] Unable to start CD playback.\n");

                return FALSE;
            }
        }

        return TRUE;
    }
}