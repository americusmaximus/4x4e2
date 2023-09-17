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
#include "Assets.Sounds.hxx"
#include "IO.Streams.hxx"

#define SOUND_DIRECTORY_NAME "sound"

// NOTE: klp is a legacy metadata file used in earlier games.
// The one I found in "Fly" had the following content: 1 0 0
#define SOUND_FILE_KLP_EXTENSION_NAME "klp"

// NOTE: sfx is a modern metadata file used to describe sound.
#define SOUND_FILE_SFX_EXTENSION_NAME "sfx"

namespace Sounds
{
    enum class SoundMixMode : u32
    {
        None = 0,
        Simple = 1, // Mixing
        Advanced = 2 // Mixing & Spatialization
    };

    enum class SoundLoopMode : u32
    {
        None = 0,
        Looping = 1,
        Unknown2 = 2 // TODO
    };

    struct SoundDecoder
    {
        // TODO
    };

    // a.k.a. SampleInfo
    struct SoundSampleDescriptor
    {
        Assets::Sounds::SoundSampleDefinition Definition;

        f32 ReferenceDistance;
        f32 MinimumDistance;
        f32 MaximumDistance;

        void* AllocatedMemory1;

        SoundLoopMode LoopMode;
        s32 ChannelLength[2]; // TODO

        s32 Unk106; // TODO
        s32 Unk107; // TODO
        s32 Unk108; // TODO
        s32 Unk6; // TODO, may be an enum
        s32 Unk109; // TODO
        s32 Unk110; // TODO
        s32 Unk111; // TODO
        s32 Unk112; // TODO

        Assets::Sounds::SoundCacheMode CacheControl;

        s32 Priority; // TODO name ReferenceCount
        u32 Offset; // TODO: name Index
    };

    // a.k.a. SfxSample
    struct SoundSample
    {
        SoundSampleDescriptor Descriptor;

        s32 Unk6; // TODO
        s32 Unk7; // TODO, some sort of index
        s32 Length; // TODO type

        s32 Unk9;
        s32 Unk10;
        s32 Unl11;
        SoundDecoder* Decoder;
        s32 Unk13;

        IO::Streams::InStreamFile Stream;

        struct
        {
            s32 Offset; // TODO type
            s32 Length; // TODO type
        } Lock;
    };

    // a.k.a. SfxOptions
    struct SoundEffectDescriptor
    {
        s32 NextChannelIndex; // TODO better name
        s32 Unknown101; // TODO
        f64x3 Location;
        s32 Unknown102; // TODO
        s32 Unknown103; // TODO
        f32x3 Velocity;
        s32 Unknown104; // TODO
        s32 Unknown105; // TODO
        f32 Volume;
        f32 HZ; // TODO
        f32 Unknown1001; // TODO
        s32 Unknown1002[2]; // TODO
        u32 Unk30;// TODO enum
        s32 Unknown1004; // TODO
        f64 Unknown1005; // TODO
        s32 Unknown1007; // TODO
        s32 Unknown1008; // TODO

        f32 MinimumDistance;

        f32 AAA01; // TODO

        f32 MaximumDistance;

        s32 AAA02; // TODO
    };

    // a.k.a. SfxSlot
    struct SoundEffect
    {
        SoundEffectDescriptor Descriptor;

        s32 UnknownIndex; // TODO

        BOOL AAA03; // TODO todo enum

        SoundSample* Sample;

        f32 HZ;

        s32 AAA04; // TODO
        s32 AAA05; // TODO
        s32 AAA06; // TODO
        s32 AAA07; // TODO
        s32 AAA08; // TODO
        s32 AAA09; // TODO
        s32 AAA10; // TODO
        s32 AAA11; // TODO

        f32 Pans[8]; // TODO constant

        s32 AAA12; // TODO
        s32 AAA13; // TODO
        s32 AAA14; // TODO
        s32 AAA15; // TODO
        s32 AAA16; // TODO
        s32 AAA17; // TODO
        s32 AAA18; // TODO
        s32 AAA19; // TODO
        s32 AAA20; // TODO
        s32 AAA21; // TODO
        s32 AAA22; // TODO
        s32 AAA23; // TODO
        s32 AAA24; // TODO
        s32 AAA25; // TODO
        s32 AAA26; // TODO
        s32 AAA27; // TODO
        s32 AAA28; // TODO
        s32 AAA29; // TODO
        s32 AAA30; // TODO
        s32 AAA31; // TODO

        f64 Position;

        s32 AAA32; // TODO

        u32 Options; // TODO: flags
    };
}