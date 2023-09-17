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

namespace Components
{
    struct StringList;

    typedef const void* (CDECLAPI* STRINGLISTRELEASE) (StringList* self, const Objects::ReleaseMode mode);
    typedef const void(CDECLAPI* STRINGLISTREMOVE) (StringList* self, const s32 start, const s32 end);
    typedef const void(CDECLAPI* STRINGLISTSORT) (StringList* self, const s32 start, const s32 end);
    typedef const void(CDECLAPI* STRINGLISTINSERT) (StringList* self, const s32 indx, const char* value);
    typedef const void(CDECLAPI* STRINGLISTSWAP) (StringList* self, const s32 src, const s32 dst);
    typedef const void(CDECLAPI* STRINGLISTCLEAR) (StringList* self);

    void* ReleaseStringList(StringList* self, const Objects::ReleaseMode mode);
    void RemoveStringList(StringList* self, const s32 start, const s32 end);
    void SortStringList(StringList* self, const s32 start, const s32 end);
    void InsertStringList(StringList* self, const s32 indx, const char* value);
    void SwapStringList(StringList* self, const s32 src, const s32 dst);
    void ClearStringList(StringList* self);

    struct StringListSelf
    {
        STRINGLISTRELEASE Release = (STRINGLISTRELEASE)&ReleaseStringList; // 0x00445740
        STRINGLISTREMOVE Remove = (STRINGLISTREMOVE)&RemoveStringList; // 0x00445b60
        STRINGLISTSORT Sort = (STRINGLISTSORT)&SortStringList; // 0x00445c60
        STRINGLISTINSERT Insert = (STRINGLISTINSERT)&InsertStringList; // 0x00445940
        STRINGLISTSWAP Swap = (STRINGLISTSWAP)&SwapStringList; // 0x00445f50
        STRINGLISTCLEAR Clear = (STRINGLISTCLEAR)&ClearStringList; // 00445810
    };

    // a.k.a. CStrList
    struct StringList
    {
        s32 Count;
        s32 Capacity;
        char** Items;
        StringListSelf* Self;
    };

    StringList* ConstructStringList(StringList* self);
    StringList* AssignStringList(StringList* self, StringList* value);

    struct StringListContainer
    {
        StringListSelf StringListSelf; // 0x006643e4

        // 0x00664460
        Objects::AbstractObjectInitializer Initializer =
        {
            .Options = 0x20100, // TODO
            .Initialize = (Objects::ABSTRACTOBJECTINITIALIZERINITIALIZE)&ConstructStringList,
            .Assign = (Objects::ABSTRACTOBJECTINITIALIZERASSIGN)&AssignStringList,
            .Release = (Objects::ABSTRACTOBJECTINITIALIZERRELEASE)&ReleaseStringList,
            .Size = sizeof(StringList),
            .Name = "$CStrList$$"
        };
    };

    extern StringListContainer StringListState;

    void AllocateStringList(StringList* self, const s32 count);
    char* AcquireStringListItem(StringList* self, const s32 indx);
    void AttachStringListItem(StringList*, const char* value);
}