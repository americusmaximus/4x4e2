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
#include "Objects.hxx"
#include "Sounds.hxx"
#include "Strings.hxx"

#include <stdio.h>
#include <stdlib.h>

#define SFX_COMMENT_TEMPLATE "//"
#define SFX_REFERENCE_DISTANCE_PROPERTY_TEMPLATE "refDist =%f"
#define SFX_MINIMUM_DISTANCE_PROPERTY_TEMPLATE "minDist =%f"
#define SFX_MAXIMUM_DISTANCE_PROPERTY_TEMPLATE "maxDist =%f"
#define SFX_MAXIMUM_VOLUME_PROPERTY_TEMPALTE "maxVol =%f"
#define SFX_LENGTH_PROPERTY_TEMPLATE "length =%d"
#define SFX_LOOP_PROPERTY_TEMPLATE "loop"

#define MAX_SFX_FILE_LINE_LENGTH 300
#define SFX_COMMENT_VALUE "//"

using namespace IO::Streams;
using namespace IO;
using namespace Logger;
using namespace Objects;
using namespace Strings;

namespace Sounds
{
    // 0x005bcd20
    SoundSampleDescriptor* ConstructSoundSampleDescriptor(SoundSampleDescriptor* self)
    {
        ZeroMemory(self, sizeof(SoundSampleDescriptor));

        self->ReferenceDistance = 20.0f * SoundDeviceControllerState.DistanceFactor.InverseValue; // TODO constant

        self->MinimumDistance = *SoundState._UnknownSoundEffectValue1 * SoundDeviceControllerState.DistanceFactor.InverseValue;
        self->MaximumDistance = 10000.0f * SoundDeviceControllerState.DistanceFactor.InverseValue; // TODO constant

        return self;
    }

    // 0x005bcde0
    s32 AcquireSoundSampleDescriptorOffset(SoundSampleDescriptor* self, const s32 offset)
    {
        const s32 value = offset * self->Definition.BitsPerSample * self->Definition.Channels + 7; // TODO constant

        const auto sign = value >> 0x1f; // TODO constants

        return (s32)((value + sign * -8) - (u32)(sign << 2 < 0)) >> 3; // TODO constants
    }

    // 0x005bd060
    u32 AcquireUnknownSoundSampleDescriptorValue1(SoundSampleDescriptor* self)
    {
        if (self->LoopMode != SoundLoopMode::Unknown2
            && self->Definition.Length == self->ChannelLength[0]) // TODO constant
        {
            if (self->Unk6 < 1) // TODO constant
            {
                if (self->Unk6 == 0) { return 1; } // TODO constant

                return 0; // TODO constant
            }
        }

        return 2; // TODO constant
    }

    // 0x005b8a10
    void LoadSoundSampleDescriptor(SoundSampleDescriptor* self)
    {
        self->LoopMode = SoundLoopMode::Looping;
        self->ReferenceDistance = 20.0f * SoundDeviceControllerState.DistanceFactor.InverseValue; // TODO constant
        self->MinimumDistance = *SoundState._UnknownSoundEffectValue1 * SoundDeviceControllerState.DistanceFactor.InverseValue;
        self->MaximumDistance = 10000.0f * SoundDeviceControllerState.DistanceFactor.InverseValue; // TODO constant
        self->ChannelLength[0] = self->Definition.Length;
        self->Unk6 = -1; // TODO constant

        char file[STANDARD_IO_FILE_NAME_LENGTH];
        char klp[STANDARD_IO_FILE_NAME_LENGTH];

        _splitpath(self->Definition.Name, NULL, NULL, file, NULL);
        _makepath(klp, NULL, NULL, file, SOUND_FILE_KLP_EXTENSION_NAME);

        if (0 < AcquireFileSize(SOUND_DIRECTORY_NAME, klp))
        {
            self->LoopMode = SoundLoopMode::Looping;
            self->Unk6 = 0;
            self->ChannelLength[0] = self->Definition.Length;
        }

        char path[STANDARD_IO_FILE_NAME_LENGTH];
        _makepath(path, NULL, NULL, file, SOUND_FILE_SFX_EXTENSION_NAME);

        u32 index = 0;
        InStreamFile stream;
        ConstructInStreamFile(&stream);

        if (OpenInStreamFileFilePath(&stream, SOUND_DIRECTORY_NAME, path, StreamMode::Data))
        {
            char line[MAX_SFX_FILE_LINE_LENGTH];

            u32 minimumDistanceLineIndex = 0;
            u32 referenceDistanceLineIndex = 0;

            while (STREAM_RESULT_END_OF_DATA < ReadAbstractInStreamLine((AbstractInStream*)&stream, line, MAX_SFX_FILE_LINE_LENGTH))
            {
                index = index + 1;

                {
                    // Check if the line contains (or starts with) comment.
                    auto comment = strstr(line, SFX_COMMENT_VALUE);

                    // End the line at the comment start.
                    if (comment != NULL) { comment[0] = NULL; }
                }

                {
                    // Trim trailing "space" characters.
                    auto len = strlen(line);

                    while (isspace(line[len - 1])) { len = len - 1; }

                    line[len] = NULL;
                }

                {
                    // Trim "space" characters at the start of the string.

                    u32 indx = 0;
                    auto len = strlen(line);

                    while (isspace(line[indx])) { indx = index + 1; }

                    if (indx != 0) { memmove(line, &line[indx], len - indx); }
                }

                // Check if there is anything to parse after all the cleaning done above.
                if (line[0] == NULL) { continue; }

                if (sscanf(line, SFX_REFERENCE_DISTANCE_PROPERTY_TEMPLATE, &self->ReferenceDistance) == 1)
                {
                    if (referenceDistanceLineIndex != 0)
                    {
                        LogError("Unable to parse reference distance. The property was specified in %s on line %d and then again on line %d.",
                            path, referenceDistanceLineIndex, index);
                    }

                    referenceDistanceLineIndex = index;

                    if (self->ReferenceDistance < 0.1f) // TODO constant
                    {
                        LogError("Specified reference volume distance value of %f is too small in %s on line %d.",
                            self->ReferenceDistance, path, index);
                    }

                    if (minimumDistanceLineIndex == 0) { self->MinimumDistance = self->ReferenceDistance / 4.0f; } // TODO constant

                    continue;
                }

                if (sscanf(line, SFX_MINIMUM_DISTANCE_PROPERTY_TEMPLATE, &self->MinimumDistance) == 1)
                {
                    if (minimumDistanceLineIndex != 0)
                    {
                        LogError("Unable to parse minimum distance. The property was specified in %s on line %d and then again on line %d.",
                            path, minimumDistanceLineIndex, index);
                    }

                    minimumDistanceLineIndex = index;

                    if (self->ReferenceDistance < 0.1f) // TODO constant
                    {
                        LogError("Specified minimum volume distance value of %f is too small in %s on line %d.",
                            self->ReferenceDistance, path, index);
                    }

                    continue;
                }

                // NOTE: Maximum volume distance has no checks or validation.
                if (sscanf(line, SFX_MAXIMUM_DISTANCE_PROPERTY_TEMPLATE, &self->MaximumDistance) == 1) { continue; }

                {
                    f32 maxVolume;
                    if (sscanf(line, SFX_MAXIMUM_VOLUME_PROPERTY_TEMPALTE, &maxVolume) == 1)
                    {
                        if (referenceDistanceLineIndex == 0)
                        {
                            LogError("%s specifies maxVol on line %d without first specifying reference distance.", path, index);
                        }

                        if (minimumDistanceLineIndex != 0)
                        {
                            LogError("Minimum distance specified in %s on line %d and then again on line %d.",
                                path, minimumDistanceLineIndex, index);
                        }

                        minimumDistanceLineIndex = index;

                        if (maxVolume < 0.001f) // TODO constant
                        {
                            LogError("Specified maxVol value %f is too small in %s line %d.", maxVolume, path, index);
                        }

                        self->MinimumDistance = self->ReferenceDistance / maxVolume;

                        continue;
                    }
                }

                {
                    s32 length;
                    if (sscanf(line, SFX_LENGTH_PROPERTY_TEMPLATE, &length) == 1)
                    {
                        if (-1 < self->Definition.Length)
                        {
                            LogError("Length for %s is already known, then specified again in %s on line %d.",
                                self->Definition.Name, path, index);
                        }

                        if (length < 1) { LogError("Invalid length %d in %s on line %d.", length, path, index); }

                        self->Definition.Length = length;

                        continue;
                    }
                }

                if (EqualStrings(SFX_LOOP_PROPERTY_TEMPLATE, line))
                {
                    self->LoopMode = SoundLoopMode::Looping;
                    self->Unk6 = 0; // TODO constant
                    self->ChannelLength[0] = self->Definition.Length;

                    continue;
                }

                LogError("Unable to parse %s line %d: %s", path, index, line);
            }

            if (self->LoopMode != SoundLoopMode::None)
            {
                for (u32 x = 0; x < (u32)self->LoopMode; x++) // TODO
                {
                    if (self->ChannelLength[x] < 0)
                    {
                        self->ChannelLength[x] = self->Definition.Length;
                    }
                }
            }
        }

        ReleaseInStreamFile(&stream, ReleaseMode::None);
    }

    // 0x005bce10
    // a.k.a. cvtPlaybackPos
    f64 CalculateSoundSampleDescriptorPosition(SoundSampleDescriptor* self, const f64 position, const SoundSeek src, const SoundSeek dst)
    {
        if (src == dst) { return position; }

        auto result = position;

        switch (src)
        {
        case SoundSeek::Set: { break; }
        case SoundSeek::Current: { result = self->Definition.HZ * position; break; }
        case SoundSeek::End:
        {
            if (self->Definition.Length < 1) { LogError("Unable to calculate sound sample position, unable to use relative sample position when length of %s is not known.", self->Definition.Name); }

            result = self->Definition.Length * position; break;
        }
        default: { LogError("Unable to calculate sound sample position, invalid input sample position type."); }
        }

        switch (dst)
        {
        case SoundSeek::Set: { break; }
        case SoundSeek::Current: { result = position / self->Definition.HZ; break; }
        case SoundSeek::End:
        {
            if (self->Definition.Length < 1) { LogError("Unable to calculate sound sample position, unable to use relative sample position when length of %s is not known.", self->Definition.Name); }

            result = position / self->Definition.Length; break;
        }
        default: { LogError("Unable to calculate sound sample position, invalid input sample position type."); }
        }

        return result;
    }
}