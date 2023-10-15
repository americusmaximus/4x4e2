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

#include "Objects.hxx"

#define MAX_SOUND_VEHICLE_SOUND_EFFECT_COUNT 10

namespace Vehicles
{
    struct VehicleSoundEffectDetail
    {
        s32 Unk00; // TODO
        s32 Unk01; // TODO
        s32 Unk02; // TODO
        s32 Unk03; // TODO
        s32 Unk04; // TODO
        s32 Unk05; // TODO
        s32 Unk06; // TODO
        s32 Unk07; // TODO
        s32 Unk08; // TODO
        s32 Unk09; // TODO

        s32 Unk10; // TODO
        s32 Unk11; // TODO
        s32 Unk12; // TODO
        s32 Unk13; // TODO
        s32 Unk14; // TODO
        s32 Unk15; // TODO
        s32 Unk16; // TODO
        s32 Unk17; // TODO
        s32 Unk18; // TODO
        s32 Unk19; // TODO

        s32 Unk20; // TODO
        s32 Unk21; // TODO
        s32 Unk22; // TODO
        s32 Unk23; // TODO
        s32 Unk24; // TODO
        s32 Unk25; // TODO
        s32 Unk26; // TODO
        s32 Unk27; // TODO
        s32 Unk28; // TODO
        s32 Unk29; // TODO

        s32 Unk30; // TODO
        s32 Unk31; // TODO
        s32 Unk32; // TODO
        s32 Unk33; // TODO
        s32 Unk34; // TODO
        s32 Unk35; // TODO
        s32 Unk36; // TODO
        s32 Unk37; // TODO
        s32 Unk38; // TODO
        s32 Unk39; // TODO

        s32 Unk40; // TODO
        s32 Unk41; // TODO
        s32 Unk42; // TODO
        s32 Unk43; // TODO
        s32 Unk44; // TODO
        s32 Unk45; // TODO
        s32 Unk46; // TODO
        s32 Unk47; // TODO
        s32 Unk48; // TODO
        s32 Unk49; // TODO

        s32 Unk50; // TODO
        s32 Unk51; // TODO
        s32 Unk52; // TODO
        s32 Unk53; // TODO
        s32 Unk54; // TODO
        s32 Unk55; // TODO
        s32 Unk56; // TODO
        s32 Unk57; // TODO
        s32 Unk58; // TODO
        s32 Unk59; // TODO

        s32 Unk60; // TODO
        s32 Unk61; // TODO
        s32 Unk62; // TODO
        s32 Unk63; // TODO
        s32 Unk64; // TODO
        s32 Unk65; // TODO
        s32 Unk66; // TODO
        s32 Unk67; // TODO
        s32 Unk68; // TODO
        s32 Unk69; // TODO

        s32 Unk70; // TODO
        s32 Unk71; // TODO
        s32 Unk72; // TODO
        s32 Unk73; // TODO
        s32 Unk74; // TODO
        s32 Unk75; // TODO
        s32 Unk76; // TODO
        s32 Unk77; // TODO
        f32 Unk78; // TODO
        f32 Unk79; // TODO

        f32 Unk80; // TODO
        f32 Unk81; // TODO
        s32 Unk82; // TODO
        s32 Unk83; // TODO
        s32 Unk84; // TODO
        s32 Unk85; // TODO
        s32 Unk86; // TODO
        s32 Unk87; // TODO
        s32 Unk88; // TODO
        s32 Unk89; // TODO

        s32 Unk90; // TODO
        s32 Unk91; // TODO
        s32 Unk92; // TODO
        s32 Unk93; // TODO
        f32 Unk94; // TODO
        f32 Unk95; // TODO
        f32 Unk96; // TODO
        f32 Unk97; // TODO
    };

    // a.k.a. CVehicleSfx
    struct VehicleSoundEffect
    {
        void* Vehicle;

        s32 Unk001[14]; // TODO

        VehicleSoundEffectDetail Details;

        s32 Unk1001; // TODO
        s32 Unk1002; // TODO
        s32 Unk1003; // TODO
        s32 Unk1004; // TODO
        s32 Unk1005; // TODO
        s32 Unk1006; // TODO
        s32 Unk1007; // TODO
        s32 Unk1008; // TODO
        s32 Unk1009; // TODO
        s32 Unk1010; // TODO
        s32 Unk1011; // TODO
        s32 Unk1012; // TODO
        s32 Unk1013; // TODO
        s32 Unk1014; // TODO
        s32 Unk1015; // TODO
        s32 Unk1016; // TODO
        s32 Unk1017; // TODO
        s32 Unk1018; // TODO
        s32 Unk1019; // TODO

        f32 Unk1020; // TODO

        s32 Unk1021; // TODO
        s32 Unk1022; // TODO
        s32 Unk1023; // TODO
        s32 Unk1024; // TODO
        s32 Unk1025; // TODO
        s32 Unk1026; // TODO
        s32 Unk1027; // TODO
        s32 Unk1028; // TODO
        s32 Unk1029; // TODO
        s32 Unk1030; // TODO
        s32 Unk1031; // TODO
        s32 Unk1032; // TODO
        s32 Unk1033; // TODO
        s32 Unk1034; // TODO
    };

    VehicleSoundEffect* ConstructVehicleSoundEffect(VehicleSoundEffect* self);
    void* ReleaseVehicleSoundEffect(VehicleSoundEffect* self);

    struct VehicleSoundEffectContainer
    {
        VehicleSoundEffect* _Items = (VehicleSoundEffect*)0x00d61624; // TODO Array of MAX_SOUND_VEHICLE_SOUND_EFFECT_COUNT

        // 0x00667f40
        Objects::AbstractObjectInitializer Initializer =
        {
            .Options = 0x20100, // TODO
            .Initialize = (Objects::ABSTRACTOBJECTINITIALIZERINITIALIZE)&ConstructVehicleSoundEffect,
            .Assign = NULL,
            .Release = (Objects::ABSTRACTOBJECTINITIALIZERRELEASE)&ReleaseVehicleSoundEffect,
            .Size = sizeof(VehicleSoundEffect),
            .Name = "$CVehicleSfx$$"
        };
    };

    extern VehicleSoundEffectContainer VehicleSoundEffectState;
}