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

#include "Basic.hxx"

namespace Sounds
{
    // a.k.a. CSound
    struct SoundManager
    {
        BOOL IsActive;

        s32 SampleCount; // TODO type

        char Unk001[64]; // TODO

        char SampleNames[1][96]; // TODO

        char Unk002[6768]; // TODO

        struct
        {
            s32 Index; // TODO type
            u32 Hash;
            char Name[32]; // TODO
        } Sample;

        s32 Unk003; // TODO
        u32 UnknownIndex1; // TODO
        char UnknownName1[32]; // TODO

        s32 UnknownValue10000; // TODO
    };

    struct SoundManagerContainer
    {
        BOOL* _UnknownValue101 = (BOOL*)0x00892484; // TODO, proper place for this one

        BOOL* _IsActive = (BOOL*)0x0067d6a8; // TODO, default value TRUE

        SoundManager* _Instance = (SoundManager*)0x00d5fa74; //TODO
    };

    extern SoundManagerContainer SoundManagerState;

    void LoadSoundManagerOptions(SoundManager* self, const char* file, const char* section);
    void SaveSoundManagerOptions(SoundManager* self, const char* file, const char* section);

    void InitializeSoundManager(SoundManager* self);
    void StartSoundManager(SoundManager* self);
    void CloseSoundManager(SoundManager* self);
    void PauseSoundManager(SoundManager* self);
    void PlaySoundManager(SoundManager* self);
    void LoadSoundManagerSoundSample(SoundManager* self, const s32 indx); // TODO type
}