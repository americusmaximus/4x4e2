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

#include "Native.hxx"

#include <math.h>

#define CREATE_THREAD_OPTION_NONE 0

#define WAIT_TIME_MULTIPLIER (1000.0)

namespace Native
{
    void __declspec(noreturn) Halt() { DebugBreak(); }

    BOOL IsDebuggerActive(void) { return IsDebuggerPresent(); }

    // 0x0060c640
    HMODULE InitializeModule(const char* name) { return LoadLibraryA(name); }

    // 0x0060c630
    BOOL ReleaseModule(const HMODULE module) { return FreeLibrary(module); }

    // 0x0060c650
    FARPROC AcquireLambda(const HMODULE module, const char* name) { return GetProcAddress(module, name); }

    // 0x0060c230
    HANDLE InitializeMutex(void) { return CreateMutexA(NULL, FALSE, NULL); }

    // 0x0060c240
    BOOL WaitMutex(const HANDLE mutex)
    {
        if (mutex == NULL) { return FALSE; }

        return WaitForSingleObject(mutex, MAXDWORD) == WAIT_OBJECT_0;
    }

    // 0x0060c260
    BOOL WaitMutex2(const HANDLE mutex, const f64 time)
    {
        if (mutex == NULL) { return FALSE; }

        return WaitForSingleObject(mutex, (DWORD)round(time * WAIT_TIME_MULTIPLIER)) == WAIT_OBJECT_0;
    }

    // 0x0060c2c0
    HANDLE InitializeThread(const void* lambda, void* params)
    {
        DWORD id;

        return CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)lambda, params, CREATE_THREAD_OPTION_NONE, &id);
    }

    // 0x0060c2a0
    BOOL DisposeMutex(const HANDLE mutex)
    {
        if (mutex == NULL) { return FALSE; }

        return ReleaseMutex(mutex);
    }

    // 0x0060c350
    void Wait(const f64 time)
    {
        Sleep(time <= 0.0 ? 0 : (DWORD)round(time * WAIT_TIME_MULTIPLIER));
    }

    // 0x0060c2f0
    BOOL SelectThreadWeight(const HANDLE thread, const ThreadWeight weight)
    {
        auto priority = THREAD_PRIORITY_NORMAL;

        switch (weight)
        {
        case ThreadWeight::Low: { priority = THREAD_PRIORITY_BELOW_NORMAL; break; }
        case ThreadWeight::Normal: { priority = THREAD_PRIORITY_NORMAL; break; }
        case ThreadWeight::High: { priority = THREAD_PRIORITY_ABOVE_NORMAL; break; }
        case ThreadWeight::Max: { priority = THREAD_PRIORITY_HIGHEST; break; }
        }

        return SetThreadPriority(thread, priority);
    }
}