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

#define SOUND_BITS_8 8
#define SOUND_BITS_16 16

#define SOUND_BYTES_1 1
#define SOUND_BYTES_2 2

#define SOUND_CHANNEL_COUNT_NONE 0
#define SOUND_CHANNEL_COUNT_MONO 1
#define SOUND_CHANNEL_COUNT_STEREO 2
#define SOUND_CHANNEL_COUNT_SURROUND 3

#define SOUND_FREQUENCY_11025 11025
#define SOUND_FREQUENCY_22050 22050
#define SOUND_FREQUENCY_44100 44100

#define MAX_SOUND_CHANNEL_COUNT 8

#define MIN_SOUND_USER_DATA_COUNT 0
#define MAX_SOUND_USER_DATA_COUNT 2

#define MIN_SOUND_LATENCY (0.05f)
#define DEFAULT_SOUND_LATENCY (0.5f)
#define MAX_SOUND_LATENCY (2.0f)

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

        void* AllocatedMemory1; // TODO name

        SoundLoopMode LoopMode;
        s32 ChannelLength[2]; // TODO

        s32 Unk106; // TODO
        s32 Unk107; // TODO
        s32 Unk108; // TODO

        s32 Unk6; // TODO size, may be an enum
        s32 Unk7; // TODO

        s32 Unk110; // TODO
        s32 Unk111; // TODO
        s32 Unk112; // TODO

        Assets::Sounds::SoundCacheMode CacheControl;
    };

    // a.k.a. SfxSample
    struct SoundSample
    {
        SoundSampleDescriptor Descriptor;

        s32 ReferenceCount;
        u32 Index;

        s32 Unk6; // TODO
        s32 Unk7; // TODO, some sort of index
        s32 Length; // TODO type

        s32 Unk9;
        s32 Unk10; // TODO name Position
        s32 Position;
        SoundDecoder* Decoder;
        s32 Unk13;

        IO::Streams::InStreamFile Stream;

        struct
        {
            s32 Offset; // TODO type
            s32 Length; // TODO type
        } Lock;
    };

    // TODO name
    enum class SoundEffectDescriptorUnknownType : u32
    {
        None = 0,
        Single = 1,
        Double = 2
    };

    // TODO name
    struct SoundEffectDescriptorUnknown
    {
        union
        {
            f32x3* Single;
            f64x3* Double;
        };

        SoundEffectDescriptorUnknownType Type;
    };

    enum class SoundSeek : u32
    {
        Set = 0,
        Current = 1,
        End = 2
    };

    // a.k.a. SfxOptions
    struct SoundEffectDescriptor
    {
        s32 NextChannelIndex; // TODO better name
        s32 Unknown101; // TODO

        f64x3 Location;

        SoundEffectDescriptorUnknown Unknown102;

        f32x3 Velocity;

        SoundEffectDescriptorUnknown Unknown104;

        f32 Volume;
        f32 HZ; // TODO
        f32 RemainingDelay;
        void* UserData[MAX_SOUND_USER_DATA_COUNT]; // TODO
        u32 Unk30;// TODO enum
        s32 Unknown1004; // TODO


        f64 Position; // Linear position within the timeline of the sound effect.

        SoundSeek Seek;

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

        f32 AAA04[MAX_SOUND_CHANNEL_COUNT];  // TODO constant, name, Distance/Position?

        f32 Pans[MAX_SOUND_CHANNEL_COUNT];

        s32 AAA12[MAX_SOUND_CHANNEL_COUNT]; // TODO

        s32 AAA20[MAX_SOUND_CHANNEL_COUNT]; // TODO

        struct
        {
            f32 Minimum;
            f32 Current;
            f32 Maximum;
        } Volume;

        BOOL DebugMode;

        f64 Position;

        s32 AAA32; // TODO

        u32 Options; // TODO: flags
    };

    struct SoundEffectMixContainer
    {
        f32* Data[MAX_SOUND_CHANNEL_COUNT];

        u32 Length;
        u32 Channels;
        u32 HZ;
    };
}