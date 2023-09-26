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

#include "ASCII.hxx"
#include "Editor.ScrollBar.hxx"
#include "Editor.StringList.hxx"
#include "Editor.StringPickList.hxx"
#include "Input.hxx"
#include "Logger.hxx"
#include "Native.Basic.hxx"

#include <string.h>

using namespace Input;
using namespace Objects;
using namespace Logger;
using namespace Memory;

namespace Editor
{
    StringPickListContainer StringPickListState;

    // 0x00446a30
    StringPickList* ConstructStringPickList(StringPickList* self)
    {
        ConstructStringList((StringList*)self);

        ConstructScrollBar(&self->Scroll);

        ConstructTextButton(&self->Buttons.OK);
        ConstructTextButton(&self->Buttons.Cancel);

        self->Self = &StringPickListState.StringPickListSelf;

        self->Scroll.Unk14 = 0;

        self->Unknown1000 = -1; // TODO constant

        self->States.Count = 0;
        self->States.Items = NULL;

        self->HotKeys.Count = 0;
        self->HotKeys.Items = NULL;

        strcpy(self->Text.OK, "OK"); // TODO constant
        strcpy(self->Text.Cancel, "Cancel"); // TODO constant

        self->Unk900 = -1; // TODO constant
        self->Unk901 = 0;

        return self;
    }

    // 0x00446b20
    void* ReleaseStringPickList(StringPickList* self, const ReleaseMode mode)
    {
        if ((mode & ReleaseMode::Unknown4) != ReleaseMode::None)
        {
            ReleaseMemory3(ReleaseObject(self, &StringPickListState.Initializer));

            return self;
        }

        self->Self = &StringPickListState.StringPickListSelf;

        ClearStringList((StringList*)self);

        ReleaseTextButton(&self->Buttons.Cancel, ReleaseMode::None);
        ReleaseTextButton(&self->Buttons.OK, ReleaseMode::None);

        ReleaseScrollBar(&self->Scroll, ReleaseMode::None);

        ReleaseStringList((StringList*)self, ReleaseMode::Unknown1);

        if ((mode & ReleaseMode::Unknown2) != ReleaseMode::None) { ReleaseMemory2(self); }

        return self;
    }

    // 0x00448820
    void RemoveStringPickList(StringPickList* self, const s32 start, const s32 end)
    {
        for (auto x = start, indx = end + 1; indx < self->Count; x = x + 1, indx = indx + 1)
        {
            SelectStringPickListState(self, x, AcquireStringPickListState(self, indx));
            SelectStringPickListHotKey(self, x, AcquireStringPickListHotKey(self, indx));
        }

        RemoveStringList((StringList*)self, start, end);
    }

    // 0x004488f0
    void SortStringPickList(StringPickList* self, const s32 start, const s32 end)
    {
        if (self->States.Count <= start && self->HotKeys.Count <= start)
        {
            SortStringList((StringList*)self, start, end);

            return;
        }

        for (auto x = end; start < x; x--)
        {
            for (auto xx = start; xx < x; xx++)
            {
                if (0 < strcmpi(self->Items[xx], self->Items[xx + 1])) { self->Self->Swap(self, xx, xx + 1); }
            }
        }
    }

    // 0x004487a0
    void InsertStringPickList(StringPickList* self, const s32 indx, const char* value)
    {
        InsertStringList((StringList*)self, indx, value);

        for (s32 x = self->Count - 2; indx < x; x = x - 1)
        {
            SelectStringPickListState(self, x, AcquireStringPickListState(self, x - 1));
            SelectStringPickListHotKey(self, x, AcquireStringPickListHotKey(self, x - 1));
        }

        SelectStringPickListState(self, indx, TRUE);
        SelectStringPickListHotKey(self, indx, -1); // TODO constant
    }

    // 0x00448980
    void SwapStringPickList(StringPickList* self, const s32 src, const s32 dst)
    {
        SwapStringList((StringList*)self, src, dst);

        {
            const auto a = AcquireStringPickListState(self, src);
            const auto b = AcquireStringPickListState(self, dst);

            if (a != b)
            {
                SelectStringPickListState(self, src, b);
                SelectStringPickListState(self, dst, a);
            }
        }

        {
            const auto a = AcquireStringPickListHotKey(self, src);
            const auto b = AcquireStringPickListHotKey(self, dst);

            if (a != b)
            {
                SelectStringPickListHotKey(self, src, b);
                SelectStringPickListHotKey(self, dst, a);
            }
        }
    }

    // 0x00448880
    void ClearStringPickList(StringPickList* self)
    {
        if (self->States.Items != NULL)
        {
            ReleaseMemory1(self->States.Items);
            self->States.Items = NULL;
        }

        self->States.Count = 0;

        if (self->HotKeys.Items != NULL)
        {
            ReleaseMemory1(self->HotKeys.Items);
            self->HotKeys.Items = NULL;
        }

        self->HotKeys.Count = 0;

        ClearStringList((StringList*)self);
    }

    // 0x00446df0
    s32 AcquireStringPickListAction(StringPickList* self)
    {
        if ((*InputState._KeyController)->Self->AcquireOnce(*InputState._KeyController, INPUT_KEY_ESC) != 0) { return -1; } // TODO constants

        if (AcquireKeyControllerControlState(*InputState._KeyController))
        {
            const auto insnpk = (*InputState._KeyController)->Self->AcquireOnce(*InputState._KeyController, INPUT_KEY_NUM_PAD_0);
            const auto insk = (*InputState._KeyController)->Self->AcquireOnce(*InputState._KeyController, INPUT_KEY_INSERT);

            // Check whether:
            // the INSERT key is pressed,
            // or the INSERT key on NUMPAD is pressed while NUMLOCK is NOT toggled (inactive).
            if (insk || (insnpk && (GetKeyState(VK_NUMLOCK) & 1) == 0))
            {
                CopyStringList((StringList*)self);
            }

            if ((*InputState._KeyController)->Self->AcquireOnce(*InputState._KeyController, INPUT_KEY_C))
            {
                CopyStringList((StringList*)self);
            }
        }

        s32 todo = 0; // TODO constant

        if (!AcquireKeyControllerShiftState(*InputState._KeyController)
            && (*InputState._KeyController)->Self->AcquireOnce(*InputState._KeyController, INPUT_KEY_TAB))
        {
            todo = 1; // TODO constant
        }

        if (AcquireKeyControllerShiftState(*InputState._KeyController)
            && (*InputState._KeyController)->Self->AcquireOnce(*InputState._KeyController, INPUT_KEY_TAB))
        {
            todo = -1; // TODO constant
        }

        for (u32 x = 0; x < 10; x++) // TODO constant
        {
            auto value = self->Unknown1000 + todo;
            self->Unknown1000 = value;

            if (1 < value) // TODO constant
            {
                self->Unknown1000 = 1; // TODO constant

                break;
            }

            if (value < -1)
            {
                self->Unknown1000 = 1; // TODO constant

                if (self->Text.Cancel[0] != NULL) { break; }
            }
            else if (value != 0 || self->Text.OK[0] != NULL)
            {
                if (self->Unknown1000 == 1) // TODO constant
                {
                    if (self->Text.Cancel[0] == NULL) { continue; }
                }

                break;
            }
        }

        u32 key = ASCII_NULL;

        // Go into the loop to handle key events, until an element is selected.
        do
        {
            do
            {
                do
                {
                    do
                    {
                        if (!AreUnknownsNotEqual())
                        {
                            // Check for "UP" combinations.
                            {
                                const auto upnpk = (*InputState._KeyController)->Self->AcquireOnce(*InputState._KeyController, INPUT_KEY_NUM_PAD_8);
                                const auto upk = (*InputState._KeyController)->Self->AcquireOnce(*InputState._KeyController, INPUT_KEY_UP_ARROW);

                                if (upk || (upnpk && (GetKeyState(VK_NUMLOCK) & 1) == 0))
                                {
                                    self->Text.Input[0] = NULL;

                                    self->Index = self->Index - 1;
                                }
                            }

                            // Check for "DOWN" combinations.
                            {
                                const auto dwnnpk = (*InputState._KeyController)->Self->AcquireOnce(*InputState._KeyController, INPUT_KEY_NUM_PAD_2);
                                const auto dwnk = (*InputState._KeyController)->Self->AcquireOnce(*InputState._KeyController, INPUT_KEY_DOWN_ARROW);

                                if (dwnk || (dwnnpk && (GetKeyState(VK_NUMLOCK) & 1) == 0))
                                {
                                    self->Text.Input[0] = NULL;

                                    self->Index = self->Index + 1;
                                }
                            }

                            // Check for "HOME" combinations.
                            {
                                const auto hmnpk = (*InputState._KeyController)->Self->AcquireOnce(*InputState._KeyController, INPUT_KEY_NUM_PAD_7);
                                const auto hmk = (*InputState._KeyController)->Self->AcquireOnce(*InputState._KeyController, INPUT_KEY_HOME);

                                if (hmk || (hmnpk && (GetKeyState(VK_NUMLOCK) & 1) == 0))
                                {
                                    self->Text.Input[0] = NULL;

                                    self->Index = 0;
                                }
                            }

                            // Check for "END" combinations.
                            {
                                const auto endnpk = (*InputState._KeyController)->Self->AcquireOnce(*InputState._KeyController, INPUT_KEY_NUM_PAD_1);
                                const auto endk = (*InputState._KeyController)->Self->AcquireOnce(*InputState._KeyController, INPUT_KEY_END);

                                if (endk || (endnpk && (GetKeyState(VK_NUMLOCK) & 1) == 0))
                                {
                                    self->Text.Input[0] = NULL;

                                    self->Index = self->Count;
                                }
                            }

                            // Check for "PAGE UP" combinations.
                            {
                                const auto pgupnpk = (*InputState._KeyController)->Self->AcquireOnce(*InputState._KeyController, INPUT_KEY_NUM_PAD_9);
                                const auto pgupk = (*InputState._KeyController)->Self->AcquireOnce(*InputState._KeyController, INPUT_KEY_PAGE_UP);

                                if (pgupk || (pgupnpk && (GetKeyState(VK_NUMLOCK) & 1) == 0))
                                {
                                    self->Text.Input[0] = NULL;

                                    self->Index = self->Index - self->PageCount;
                                    self->Unknown5 = self->Unknown5 - self->PageCount;
                                }
                            }

                            // Check for "PAGE DOWN" combinations.
                            {
                                const auto pgdwnnpk = (*InputState._KeyController)->Self->AcquireOnce(*InputState._KeyController, INPUT_KEY_NUM_PAD_3);
                                const auto pgdwnk = (*InputState._KeyController)->Self->AcquireOnce(*InputState._KeyController, INPUT_KEY_PAGE_DOWN);

                                if (pgdwnk || (pgdwnnpk && (GetKeyState(VK_NUMLOCK) & 1) == 0))
                                {
                                    self->Text.Input[0] = NULL;

                                    self->Index = self->Index + self->PageCount;
                                    self->Unknown5 = self->Unknown5 + self->PageCount;
                                }
                            }

                            // Check for "LEFT" combinations.
                            {
                                const auto lnpk = (*InputState._KeyController)->Self->AcquireOnce(*InputState._KeyController, INPUT_KEY_NUM_PAD_4);
                                const auto lk = (*InputState._KeyController)->Self->AcquireOnce(*InputState._KeyController, INPUT_KEY_LEFT_ARROW);

                                if (lk || (lnpk && (GetKeyState(VK_NUMLOCK) & 1) == 0))
                                {
                                    self->Text.Input[0] = NULL;

                                    self->Index = self->Index - self->Unknown7;
                                }
                            }

                            // Check for "RIGHT" combinations.
                            {
                                const auto rnpk = (*InputState._KeyController)->Self->AcquireOnce(*InputState._KeyController, INPUT_KEY_NUM_PAD_6);
                                const auto rk = (*InputState._KeyController)->Self->AcquireOnce(*InputState._KeyController, INPUT_KEY_RIGHT_ARROW);

                                if (rk || (rnpk && (GetKeyState(VK_NUMLOCK) & 1) == 0))
                                {
                                    self->Text.Input[0] = NULL;

                                    self->Index = self->Index + self->Unknown7;
                                }
                            }

                            return 0; // TODO constant
                        }

                        key = AcquireKeyControllerUpperCaseValue(*InputState._KeyController);

                        if (key == ASCII_CARRIAGE_RETURN)
                        {
                            if (self->Unknown1000 == 1) { return -1; } // TODO constant

                            if (AcquireStringPickListState(self, self->Index))
                            {
                                self->Unknown1 = 1; // TODO constant

                                return 0; // TODO constant
                            }
                        }

                    } while (self->Unknown1000 != -1); // TODO constant

                    if (key == ASCII_BACKSPACE) { self->Text.Input[0] = NULL; }

                } while (self->HotKeys.Count != 0);

                // Once all hot keys and navigation options are exhausted -- add valid character to the input.

                // Pattern: a-zA-Z0-9
                if (isalnum(key))
                {
                    const auto len = strlen(self->Text.Input);

                    if (len - 1 < (MAX_STRING_PICK_LIST_TEXT_LENGTH - 1))
                    {
                        self->Text.Input[len] = (char)key;
                        self->Text.Input[len + 1] = NULL;
                    }
                }

            } while (self->Text.Input[0] == NULL);

            // NOTE: The function returns 0 if the given two strings are same,
            // Negative value indicates whether the length of STR1 is less than the length of STR2.
            if (strcmpi(self->Text.Input, AcquireStringList((StringList*)self, self->Index)) < 1) { self->Index = 0; }

            while (self->Index < self->Count)
            {
                // NOTE: The function returns 0 if the given two strings are same,
                // Negative value indicates whether the length of STR1 is less than the length of STR2.
                if (strcmpi(self->Text.Input, AcquireStringList((StringList*)self, self->Index)) < 1) { break; }

                self->Index = self->Index + 1;
            }

        } while (TRUE);
    }

    // 0x00448740
    // a.k.a. getItemHotKey
    s32 AcquireStringPickListHotKey(StringPickList* self, const s32 indx)
    {
        if (indx < 0 || self->Count <= indx) { LogError("Unable to acquire hot key, invalid index %d.", indx); }

        if (self->HotKeys.Count <= indx) { return -1; } // TODO constant

        return self->HotKeys.Items[indx];
    }

    // 0x00448670
    // a.k.a. setItemHotKey
    void SelectStringPickListHotKey(StringPickList* self, const s32 indx, const s32 value)
    {
        if (indx < 0 || self->Count <= indx) { LogError("Unable to select hot key, invalid index %d.", indx); }

        if (self->HotKeys.Count <= indx)
        {
            if (value == -1) { return; } // TODO constant

            self->HotKeys.Items = (s32*)ReallocateMemory(self->HotKeys.Items, self->Count * sizeof(s32));

            if (self->HotKeys.Items == NULL) { LogError("Unable to allocate memory for item hot keys."); }

            while (self->HotKeys.Count < self->Count)
            {
                self->HotKeys.Items[self->HotKeys.Count] = -1; // TODO constant
                self->HotKeys.Count = self->HotKeys.Count + 1;
            }
        }

        self->HotKeys.Items[indx] = value;
    }

    // 0x00445c20
    void SortStringPickListAll(StringPickList* self)
    {
        self->Self->Sort(self, 0, self->Count - 1);
    }

    // 0x00448610
    // a.k.a. isItemEnabled
    BOOL AcquireStringPickListState(StringPickList* self, const s32 indx)
    {
        if (indx < 0 || self->Count <= indx) { LogError("Unable to acquire state, invalid index %d.", indx); }

        if (self->States.Count <= indx) { return TRUE; }

        return self->States.Items[indx];
    }

    // 0x00448540
    // a.k.a. enableItem
    void SelectStringPickListState(StringPickList* self, const s32 indx, const BOOL value)
    {
        if (indx < 0 || self->Count <= indx) { LogError("Unable to select state, invalid index %d.", indx); }

        if (self->States.Count <= indx)
        {
            if (value) { return; }

            self->States.Items = (BOOL*)ReallocateMemory(self->States.Items, self->Count * sizeof(BOOL));

            if (self->States.Items == NULL) { LogError("Unable to allocate memory for item states."); }

            while (self->States.Count < self->Count)
            {
                self->States.Items[self->States.Count] = TRUE;
                self->States.Count = self->States.Count + 1;
            }
        }

        self->States.Items[indx] = value;
    }
}