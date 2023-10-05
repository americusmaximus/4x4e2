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

#define MAX_SOUND_SAMPLE_NAME_LENGTH 256

namespace Assets::Sounds
{
    enum class SoundSampleType : u32
    {
        Unknown = 0, // WAVE_FORMAT_UNKNOWN
        WAVMSPCM = 1, // WAVE_FORMAT_PCM
        WAVYADPCM = 2, // WAVE_FORMAT_YAMAHA_ADPCM
        Unknown3 = 3,
        Unknown4 = 4,
        Unknown5 = 5,
        Unknown6 = 6
    };

    enum class SoundCacheMode : u32
    {
        Normal = 0,
        Static = 1,
        Stream = 2
    };

    struct SoundSampleDefinition
    {
        char Name[MAX_SOUND_SAMPLE_NAME_LENGTH];

        Assets::Sounds::SoundSampleType Type;

        u32 BitsPerSample;
        u32 Channels;
        u32 HZ;
        s32 Length; // TODO type
    };
}