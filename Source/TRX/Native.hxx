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
#include "Native.Basic.hxx"

namespace Native
{
    enum class ThreadWeight : u32
    {
        Low = 0,
        Normal = 1,
        High = 2,
        Max = 3
    };

    void __declspec(noreturn) Halt();
    BOOL IsDebuggerActive(void);
    BOOL ReleaseModule(const HMODULE module);
    BOOL DisposeMutex(const HANDLE mutex);
    BOOL SelectThreadWeight(const HANDLE thread, const ThreadWeight weight);
    BOOL WaitMutex(const HANDLE mutex);
    BOOL WaitMutex2(const HANDLE mutex, const f64 time);
    FARPROC AcquireLambda(const HMODULE module, const char* name);
    HANDLE InitializeMutex(void);
    HANDLE InitializeThread(const void* lambda, void* params);
    HMODULE InitializeModule(const char* name);
    void Wait(const f64 time);
}