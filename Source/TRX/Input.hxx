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
#include "Input.KeyController.hxx"

#define INPUT_KEY_NULL 0
#define INPUT_KEY_ESC 1
#define INPUT_KEY_1 2
#define INPUT_KEY_2 3
#define INPUT_KEY_3 4
#define INPUT_KEY_4 5
#define INPUT_KEY_5 6
#define INPUT_KEY_6 7
#define INPUT_KEY_7 8
#define INPUT_KEY_8 9
#define INPUT_KEY_9 10
#define INPUT_KEY_0 11
#define INPUT_KEY_MINUS 12
#define INPUT_KEY_EQUAL 13
#define INPUT_KEY_BACKSPACE 14
#define INPUT_KEY_TAB 15
#define INPUT_KEY_Q 16
#define INPUT_KEY_W 17
#define INPUT_KEY_E 18
#define INPUT_KEY_R 19
#define INPUT_KEY_T 20
#define INPUT_KEY_Y 21
#define INPUT_KEY_U 22
#define INPUT_KEY_I 23
#define INPUT_KEY_O 24
#define INPUT_KEY_P 25
#define INPUT_KEY_OPEN_BRACKET 26
#define INPUT_KEY_CLOSE_BRACKET 27
#define INPUT_KEY_ENTER 28
#define INPUT_KEY_LEFT_CTRL 29
#define INPUT_KEY_A 30
#define INPUT_KEY_S 31
#define INPUT_KEY_D 32
#define INPUT_KEY_F 33
#define INPUT_KEY_G 34
#define INPUT_KEY_H 35
#define INPUT_KEY_J 36
#define INPUT_KEY_K 37
#define INPUT_KEY_L 38
#define INPUT_KEY_SEMI_COLON 39
#define INPUT_KEY_SINGLE_QUOTE 40
#define INPUT_KEY_BACK_TICK 41
#define INPUT_KEY_BACK_TILDE 41
#define INPUT_KEY_LEFT_SHIFT 42
#define INPUT_KEY_BACK_SLASH 43
#define INPUT_KEY_Z 44
#define INPUT_KEY_X 45
#define INPUT_KEY_C 46
#define INPUT_KEY_V 47
#define INPUT_KEY_B 48
#define INPUT_KEY_N 49
#define INPUT_KEY_M 50
#define INPUT_KEY_COMA 51
#define INPUT_KEY_DOT 52
#define INPUT_KEY_FORWARD_SLASH 53
#define INPUT_KEY_RIGHT_SHIFT 54
#define INPUT_KEY_PRINT_SCREEN 55
#define INPUT_KEY_LEFT_ALT 56
#define INPUT_KEY_SPACE 57
#define INPUT_KEY_CAPS_LOCK 58
#define INPUT_KEY_F1 59
#define INPUT_KEY_F2 60
#define INPUT_KEY_F3 61
#define INPUT_KEY_F4 62
#define INPUT_KEY_F5 63
#define INPUT_KEY_F6 64
#define INPUT_KEY_F7 65
#define INPUT_KEY_F8 66
#define INPUT_KEY_F9 67
#define INPUT_KEY_F10 68
#define INPUT_KEY_NUM_LOCK 69
#define INPUT_KEY_SCROLL_LOCK 70
#define INPUT_KEY_NUM_PAD_7 71
#define INPUT_KEY_NUM_PAD_8 72
#define INPUT_KEY_NUM_PAD_9 73
#define INPUT_KEY_NUM_PAD_MINUS 74
#define INPUT_KEY_NUM_PAD_4 75
#define INPUT_KEY_NUM_PAD_5 76
#define INPUT_KEY_NUM_PAD_6 77
#define INPUT_KEY_NUM_PAD_PLUS 78
#define INPUT_KEY_NUM_PAD_1 79
#define INPUT_KEY_NUM_PAD_2 80
#define INPUT_KEY_NUM_PAD_3 81
#define INPUT_KEY_NUM_PAD_0 82
#define INPUT_KEY_NUM_PAD_DOT 83

#define INPUT_KEY_NUM_PAD_ASTERISK 86
#define INPUT_KEY_F11 87
#define INPUT_KEY_F12 88

#define INPUT_KEY_NUM_PAD_ENTER 284
#define INPUT_KEY_RIGHT_CTRL 285

#define INPUT_KEY_NUM_SLASH 310

#define INPUT_KEY_RIGHT_ALT 312

#define INPUT_KEY_PAUSE 325
#define INPUT_KEY_HOME 327
#define INPUT_KEY_UP_ARROW 328
#define INPUT_KEY_PAGE_UP 329

#define INPUT_KEY_LEFT_ARROW 331

#define INPUT_KEY_RIGHT_ARROW 333

#define INPUT_KEY_END 335
#define INPUT_KEY_DOWN_ARROW 336
#define INPUT_KEY_PAGE_DOWN 337
#define INPUT_KEY_INSERT 338
#define INPUT_KEY_DEL 339

#define INPUT_KEY_LEFT_MOUSE_BUTTON 512
#define INPUT_KEY_RIGHT_MOUSE_BUTTON 513

#define INPUT_KEY_MIDDLE_MOUSE_BUTTON 524
#define INPUT_KEY_X1_MOUSE_BUTTON 525
#define INPUT_KEY_X2_MOUSE_BUTTON 526
#define INPUT_KEY_FORWARD_MOUSE_WHEEL 527
#define INPUT_KEY_BACKWARD_MOUSE_WHEEL 528

#define INPUT_KEY_X_AXIS_MOUSE 597
#define INPUT_KEY_Y_AXIS_MOUSE 598
#define INPUT_KEY_NONE 599

#define MAX_INPUT_ELEMENT_COUNT 600

#define MAX_UNKNOWN_ARRAY1_COUNT 19 /* TODO NAME */

namespace Input
{
    struct InputContainer
    {
        KeyController** _KeyController = (KeyController**)0x00673e58; // TODO, points to KeyControllerState.Instance

        s32* _Unknown1 = (s32*)0x00d69274; // TODO
        s32* _Unknown2 = (s32*)0x00d69270; // TODO

        u32* _UnknownArray1 = (u32*)0x00d69220; // TODO array of MAX_UNKNOWN_ARRAY1_COUNT
    };

    extern InputContainer InputState;

    u32 ResetInput(void); // TODO type
    BOOL AreUnknownsNotEqual(void);// TODO name
    void SelectUnknownArray1Value(const u32 value); // TODO name
    char AcquireUnknownArray1Value(void); // TODO name
}