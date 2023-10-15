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

#include "Logger.hxx"
#include "Sounds.Effects.hxx"
#include "Sounds.hxx"
#include "Sounds.Options.hxx"
#include "Sounds.Samples.hxx"
#include "Vehicles.Sounds.hxx"

#define SOUND_MANAGER_TRACK_SAMPLE_NAME_TEMPLATE "track%d.wav"

using namespace IO::Ini;
using namespace Logger;
using namespace Objects;
using namespace Sounds::Options;
using namespace Vehicles;

namespace Sounds
{
    SoundManagerContainer SoundManagerState;

    // 0x005e3e20
    void LoadSoundManagerOptions(SoundManager* self, const char* file, const char* section)
    {
        IniFile ini;

        InitializeIniFile(&ini, file, section);

        LoadSoundOptions((AbstractIni*)&ini);

        ReleaseIniFile(&ini, ReleaseMode::None);
    }

    // 0x005e3e70
    void SaveSoundManagerOptions(SoundManager* self, const char* file, const char* section)
    {
        IniFile ini;

        InitializeIniFile(&ini, file, section);

        SaveSoundOptions((AbstractIni*)&ini);

        ReleaseIniFile(&ini, ReleaseMode::None);
    }

    // 0x005e4060
    void InitializeSoundManager(SoundManager* self)
    {
        if (!*SoundManagerState._IsActive) { return; }

        ReleaseSoundEffectSamples();

        ZeroMemory(VehicleSoundEffectState._Items, MAX_SOUND_VEHICLE_SOUND_EFFECT_COUNT * sizeof(VehicleSoundEffect));

        self->IsActive = FALSE;
    }

    // 0x005e3f40
    // a.k.a. start
    void StartSoundManager(SoundManager* self)
    {
        if (!*SoundManagerState._IsActive) { return; }

        LogMessage("StartSoundManager {\n");

        InitializeSoundManager(self);

        if (0.2f < AcquireSoundEffectChannelVolume(0)) { SelectSoundEffectChannelVolume(0, 0.2f); } // TODO constant

        if (!*SoundManagerState._UnknownValue101) { return; }

        if (AcquireSoundEffectChannelState(0))
        {
            StartSoundDeviceController();

            if (AcquireSoundDeviceControllerActiveState())
            {
                // TODO NOT IMPLEMENTED

                LockSounds();

                // TODO NOT IMPLEMENTED

                UnlockSound1();

                self->IsActive = TRUE;

                LogMessage("} // StartSoundManager\n");
            }
        }
    }

    // 0x005e8cd0
    void CloseSoundManager(SoundManager* self)
    {
        if (!*SoundManagerState._IsActive) { return; }

        if (self->UnknownValue10000 == 0) // TODO constant
        {
            DisposeNamedSoundSample(self->Sample.Name, TRUE);

            self->Sample.Index = 0; // TODO constant

            return;
        }

        CloseSoundDisk(*SoundState._SoundDisk);
    }

    // 0x005e8d20
    void PauseSoundManager(SoundManager* self)
    {
        if (!*SoundManagerState._IsActive) { return; }

        if (self->UnknownValue10000 == 0) // TODO constant
        {
            AttemptSoundEffectDispose(self->Sample.Hash);

            return;
        }

        PauseSoundDisk(*SoundState._SoundDisk);
    }

    // 0x005e8d60
    void PlaySoundManager(SoundManager* self)
    {
        if (!*SoundManagerState._IsActive) { return; }

        if (AcquireSoundDeviceControllerActiveState())
        {
            if (MIN_SOUND_VOLUME < AcquireSoundEffectChannelVolume(1) && self->Sample.Index != 0) // TODO constant
            {
                if (self->UnknownValue10000 == 0)
                {
                    if (!AttemptSoundEffectAcquire(self->Sample.Hash))
                    {
                        LoadSoundManagerSoundSample(self, self->Sample.Index);
                    }
                }
                else if ((*SoundState._SoundDisk)->IsPaused)
                {
                    PlaySoundDisk(*SoundState._SoundDisk, 0, 0.0f);
                }
            }
        }
    }

    // 0x005e8f30
    void LoadSoundManagerSoundSample(SoundManager* self, const s32 indx)
    {
        if (AttemptSoundEffectAcquire(self->Sample.Hash)) { return; }

        self->Sample.Index = indx;

        sprintf(self->Sample.Name, SOUND_MANAGER_TRACK_SAMPLE_NAME_TEMPLATE, indx);
        
        PushCurrentSoundEffectDescriptor();

        SelectNextSoundEffectDescriptorChannel(1);

        UpdateCurrentSoundEffectDescriptorUnk30(1);
        UpdateCurrentSoundEffectDescriptorUnk30(2);

        SelectCurrentSoundEffectDescriptorVolume(2.0f); // TODO constant

        self->Sample.Hash = FUN_005bd740(self->Sample.Name);
        
        PopCurrentSoundEffectDescriptor();
    }
}