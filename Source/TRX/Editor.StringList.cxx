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

#include "Editor.Clipboard.hxx"
#include "Editor.StringList.hxx"
#include "Logger.hxx"
#include "Strings.hxx"

#include <search.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STRING_LIST_ALLOCATION_INCREMENT 21

using namespace Memory;
using namespace Objects;
using namespace Logger;
using namespace Strings;

namespace Editor
{
    StringListContainer StringListState;

    // 0x00445c40
    s32 StringComparator(void const* a, void const* b) { return strcmpi((char*)a, (char*)b); }

    // 0x00445790
    StringList* AssignStringList(StringList* self, StringList* value)
    {
        self->Self = &StringListState.StringListSelf;

        self->Count = 0;
        self->Items = NULL;

        self->Self->Clear(self);

        if (value->Count < 1) { return self; }

        AllocateStringList(self, value->Count);

        for (u32 x = 0; x < value->Count; x = x + 1)
        {
            AttachStringList(self, AcquireStringList(value, x));
        }

        return self;
    }

    // 0x00445720
    StringList* ConstructStringList(StringList* self)
    {
        self->Capacity = 0;
        self->Count = 0;

        self->Items = NULL;

        self->Self = &StringListState.StringListSelf;

        return self;
    }

    // 0x00445740
    void* ReleaseStringList(StringList* self, const ReleaseMode mode)
    {
        if ((mode & ReleaseMode::Unknown4) != ReleaseMode::None)
        {
            ReleaseMemory3(ReleaseObject(self, &StringListState.Initializer));

            return self;
        }

        self->Self = &StringListState.StringListSelf;

        ClearStringList(self);

        if ((mode & ReleaseMode::Unknown2) != ReleaseMode::None) { ReleaseMemory2(self); }

        return self;
    }

    // 0x00445b60
    // a.k.a. remove
    void RemoveStringList(StringList* self, const s32 start, const s32 end)
    {
        if (start < 0 || self->Count <= end || end < start)
        {
            LogError("Unable to remove elements from the collection, invalid index(es) %d and %d.", start, end);
        }

        for (u32 x = start; start <= end; x++)
        {
            if (self->Items[x] != NULL) { ReleaseMemory1(self->Items[x]); }
        }

        self->Count = self->Count - (end - start + 1);

        memmove(self->Items[start], self->Items[end + 1], (self->Count - start) * sizeof(char*));
    }

    // 0x00445c60
    // a.k.a. sort
    void SortStringList(StringList* self, const s32 start, const s32 end)
    {
        if (start < end)
        {
            if (start < 0 || self->Count <= end)
            {
                LogError("Unable to sort elements in the collection, invalid index(es) %d and %d.", start, end);
            }

            qsort(self->Items[start], (end - start) + 1, sizeof(char*), StringComparator);
        }
    }

    // 0x00445940
    // a.k.a. insert
    void InsertStringList(StringList* self, const s32 indx, const char* value)
    {
        if (indx < 0 || self->Count < indx)
        {
            LogError("Unable to insert an element into the collection, invalid index %d.", indx);
        }

        self->Count = self->Count + 1;

        if (self->Capacity < self->Count)
        {
            AllocateStringList(self, self->Capacity + STRING_LIST_ALLOCATION_INCREMENT);
        }

        memmove(&self->Items[indx + 1], &self->Items[indx], (self->Count - indx - 1) * sizeof(char*));

        self->Items[indx] = (char*)AllocateMemory(strlen(value));

        if (self->Items[indx] == NULL) { LogError("Unable to allocate memory to insert an element into the collection."); }

        strcpy(self->Items[indx], value);
    }

    // 0x00445f50
    // a.k.a. swap
    void SwapStringList(StringList* self, const s32 src, const s32 dst)
    {
        if (src < 0 || self->Count <= src || dst < 0 || self->Count <= dst)
        {
            LogError("Unable to swap elements in the collection, invalid index(es) %d and %d.", src, dst);
        }

        if (src == dst) { return; }

        auto item = self->Items[src];

        self->Items[src] = self->Items[dst];
        self->Items[dst] = item;
    }

    // 0x00445810
    void ClearStringList(StringList* self)
    {
        if (self->Items != NULL)
        {
            for (u32 x = 0; x < self->Count; x++)
            {
                if (self->Items[x] != NULL)
                {
                    ReleaseMemory1(self->Items[x]);

                    self->Items[x] = NULL;
                }
            }

            ReleaseMemory1(self->Items);
        }

        self->Items = NULL;

        self->Capacity = 0;
        self->Count = 0;
    }

    // 0x00445a30
    // a.k.a. allocate
    void AllocateStringList(StringList* self, const s32 count)
    {
        if (count < self->Count) { LogError("Unable to shrink the collection."); }

        if (count < 1) { LogError("Unable to allocate element collection, invalid count %d.", count); }

        if (self->Capacity < count)
        {
            self->Capacity = count;

            self->Items = (char**)ReallocateMemory(self->Items, count * sizeof(char*));

            if (self->Items == NULL) { LogError("Unable to allocate memory for the collection."); }
        }
    }

    // 0x00445cd0
    char* AcquireStringList(StringList* self, const s32 indx)
    {
        return self->Items[indx];
    }

    // 0x00445870
    void AttachStringList(StringList* self, const char* value)
    {
        self->Self->Insert(self, self->Count, value);
    }

    // 0x00445e80
    // a.k.a. set
    void SetStringList(StringList* self, const s32 indx, const char* value)
    {
        if (indx < 0 || self->Count <= indx) { LogError("Unable to set the collection item value, invalid index %d.", indx); }

        self->Items[indx] = (char*)ReallocateMemory(self->Items[indx], IsNull(value) ? 0 : strlen(value));

        if (self->Items[indx] == NULL) { LogError("Unable to allocate memory for the element collection item."); }

        if (IsNull(value)) { self->Items[indx][0] = NULL; return; }

        strcpy(self->Items[indx], value);
    }

    // 0x00445fd0
    // a.k.a. copyToClipboard
    void CopyStringList(StringList* self)
    {
        u32 length = 0;

        for (u32 x = 0; x < self->Count; x++)
        {
            length = length + strlen(AcquireStringList(self, x));
        }

        char* buffer = (char*)AllocateMemory(length + 1);

        if (buffer == NULL) { LogError("Unable to allocate clipboard memory."); }

        u32 indx = 0;

        for (u32 x = 0; x < self->Count; x++)
        {
            indx = indx + sprintf(&buffer[indx], "%s\n", AcquireStringList(self, x));
        }

        buffer[indx] = NULL;

        SelectClipboardValue(*EditorState._InstancePointer, buffer);

        ReleaseMemory1(buffer);
    }
}