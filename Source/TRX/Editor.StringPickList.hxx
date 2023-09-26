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

#include "Editor.TextButton.hxx"
#include "Editor.ScrollBar.hxx"
#include "Objects.hxx"

#define MAX_STRING_PICK_LIST_TEXT_LENGTH 100

namespace Editor
{
    struct StringPickList;

    typedef const void* (CDECLAPI* STRINGPICKLISTRELEASE) (StringPickList* self, const Objects::ReleaseMode mode);
    typedef const void(CDECLAPI* STRINGPICKLISTREMOVE) (StringPickList* self, const s32 start, const s32 end);
    typedef const void(CDECLAPI* STRINGPICKLISTSORT) (StringPickList* self, const s32 start, const s32 end);
    typedef const void(CDECLAPI* STRINGPICKLISTINSERT) (StringPickList* self, const s32 indx, const char* value);
    typedef const void(CDECLAPI* STRINGPICKLISTSWAP) (StringPickList* self, const s32 src, const s32 dst);
    typedef const void(CDECLAPI* STRINGPICKLISTCLEAR) (StringPickList* self);
    typedef const s32(CDECLAPI* STRINGPICKLISTACQUIREACTION) (StringPickList* self);

    void* ReleaseStringPickList(StringPickList* self, const Objects::ReleaseMode mode);
    void RemoveStringPickList(StringPickList* self, const s32 start, const s32 end);
    void SortStringPickList(StringPickList* self, const s32 start, const s32 end);
    void InsertStringPickList(StringPickList* self, const s32 indx, const char* value);
    void SwapStringPickList(StringPickList* self, const s32 src, const s32 dst);
    void ClearStringPickList(StringPickList* self);
    s32 AcquireStringPickListAction(StringPickList* self);

    struct StringPickListSelf
    {
        STRINGPICKLISTRELEASE Release = (STRINGPICKLISTRELEASE)&ReleaseStringPickList; // 0x00446b20
        STRINGPICKLISTREMOVE Remove = (STRINGPICKLISTREMOVE)&RemoveStringPickList; // 0x00448820
        STRINGPICKLISTSORT Sort = (STRINGPICKLISTSORT)&SortStringPickList; // 0x004488f0
        STRINGPICKLISTINSERT Insert = (STRINGPICKLISTINSERT)&InsertStringPickList; // 0x004487a0
        STRINGPICKLISTSWAP Swap = (STRINGPICKLISTSWAP)&SwapStringPickList; // 0x00448980
        STRINGPICKLISTCLEAR Clear = (STRINGPICKLISTCLEAR)&ClearStringPickList; // 0x00448880
        STRINGPICKLISTACQUIREACTION AcquireAction = (STRINGPICKLISTACQUIREACTION)&AcquireStringPickListAction; // 0x00446df0
    };

    // a.k.a. CPickList
    // INHERITANCE: StringList
    struct StringPickList
    {
        s32 Count;
        s32 Capacity;
        char** Items;
        StringPickListSelf* Self;

        char Unk899[68]; // TODO

        s32 Unk900; // TODO
        s32 Unk901; // TODO

        struct
        {
            char OK[MAX_STRING_PICK_LIST_TEXT_LENGTH];
            char Cancel[MAX_STRING_PICK_LIST_TEXT_LENGTH];
            char Input[MAX_STRING_PICK_LIST_TEXT_LENGTH];
        } Text;

        s32 Unknown1; // TODO
        s32 Unknown2; // TODO
        s32 Unknown3; // TODO
        s32 Index;
        s32 Unknown5; // TODO
        s32 PageCount;
        s32 Unknown7; // TODO
        s32 Unknown8; // TODO
        s32 Unknown9; // TODO

        struct
        {
            s32 Count;
            BOOL* Items;
        } States;

        struct
        {
            s32 Count;
            s32* Items;
        } HotKeys;

        ScrollBar Scroll;

        struct
        {
            TextButton OK;
            TextButton Cancel;
        } Buttons;

        s32 Unknown1000; // TODO, looks like an enum or some sort of index
    };

    StringPickList* ConstructStringPickList(StringPickList* self);

    struct StringPickListContainer
    {
        StringPickListSelf StringPickListSelf; // 0x00664404

        // 0x00664460
        Objects::AbstractObjectInitializer Initializer =
        {
            .Options = 0x20100, // TODO
            .Initialize = (Objects::ABSTRACTOBJECTINITIALIZERINITIALIZE)&ConstructStringPickList,
            .Assign = NULL,
            .Release = (Objects::ABSTRACTOBJECTINITIALIZERRELEASE)&ReleaseStringPickList,
            .Size = sizeof(StringPickList),
            .Name = "$CPickList$$"
        };
    };

    extern StringPickListContainer StringPickListState;

    s32 AcquireStringPickListHotKey(StringPickList* self, const s32 indx);
    void SelectStringPickListHotKey(StringPickList* self, const s32 indx, const s32 value);
    void SortStringPickListAll(StringPickList* self);
    BOOL AcquireStringPickListState(StringPickList* self, const s32 indx);
    void SelectStringPickListState(StringPickList* self, const s32 indx, const BOOL value);
}