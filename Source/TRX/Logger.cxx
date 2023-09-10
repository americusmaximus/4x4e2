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

#include "App.Windows.hxx"
#include "Logger.hxx"
#include "Native.hxx"
#include "Time.hxx"

#include <stdarg.h>
#include <stdio.h>

#define LOG_FILE_NAME "errorlog.txt"
#define MAX_LOG_MESSAGE_BUFFER_SIZE 1024

using namespace App::Windows;
using namespace Native;
using namespace Time;

namespace Logger
{
    LoggerContainer LoggerState;

    void LogMessage(const char* format, ...)
    {
        char buffer[MAX_LOG_MESSAGE_BUFFER_SIZE];

        va_list vargs;
        va_start(vargs, format);

        vsnprintf_s(buffer, MAX_LOG_MESSAGE_BUFFER_SIZE, format, vargs);

        va_end(vargs);

        OutputDebugStringA(buffer);
    }

    // 0x0048df40
    void __declspec(noreturn) LogError(const char* format, ...)
    {
        if (LoggerState.IsOpen) { Halt(); }

        LoggerState.IsOpen = TRUE;

        char buffer[MAX_LOG_MESSAGE_BUFFER_SIZE];

        va_list vargs;
        va_start(vargs, format);

        vsnprintf_s(buffer, MAX_LOG_MESSAGE_BUFFER_SIZE, format, vargs);

        va_end(vargs);

        {
            const auto file = fopen(LOG_FILE_NAME, "at");

            if (file != NULL)
            {
                LoggerState.IsOpen = TRUE;

                fprintf(file, "%s\n", buffer);
                fclose(file);

                LoggerState.IsOpen = FALSE;
            }
        }

        if (IsDebuggerActive()) { Halt(); }

        // TODO ReleaseGraphics();

        ReleaseTime();

        MessageBoxA(*AppWindowState._HWND, buffer, APP_WINDOW_NAME, MB_ICONEXCLAMATION);

        ExitProcess(EXIT_FAILURE);
    }
}