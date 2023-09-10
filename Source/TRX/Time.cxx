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

#include "App.hxx"
#include "App.Windows.hxx"
#include "Time.hxx"

#include <math.h>
#include <timeapi.h>

#define TIMER_PERIOD_LENGTH 1

#define TIMER_PERFORMANCE_COUNTER_MULTIPLIER (4294967296.0)

#define TIMER_PERFORMANCE_COUNTER_DIVIDEND (1179648.0)

#define TIMER_ITERATION_COUNT 5
#define TIMER_ITERATION_STEP_COUNT 100000000
#define TIMER_ITERATION_STEP_VALUE 99
#define TIMER_ITERATION_MULTIPLIER 117964800

#define TIMER_VALUE_MULTIPLIER 1180

using namespace App;
using namespace App::Windows;

namespace Time
{
    TimeContainer TimeState;

    // 0x0060add0
    void InitializeTime(void)
    {
        if (QueryPerformanceFrequency(&TimeState.Counter.Performance.Frequency))
        {
            TimeState.Counter.Performance.Divisor =
                TimeState.Counter.Performance.Frequency.LowPart +
                TimeState.Counter.Performance.Frequency.HighPart * TIMER_PERFORMANCE_COUNTER_MULTIPLIER;

            TimeState.Counter.Performance.Quotient = TIMER_PERFORMANCE_COUNTER_DIVIDEND / TimeState.Counter.Performance.Divisor;

            QueryPerformanceCounter(&TimeState.Counter.Current);
        }
        else
        {
            *TimeState.Timer._IsActive = TRUE;

            if (timeBeginPeriod(TIMER_PERIOD_LENGTH) != TIMERR_NOERROR)
            {
                if (MessageBoxA(*AppWindowState._HWND, "Unable to initialize timer.", AppWindowState.WindowName, MB_ICONEXCLAMATION | MB_OKCANCEL) == IDCANCEL)
                {
                    *AppState._IsQuit = TRUE;
                }
            }
        }
    }

    // 0x0060b020
    u32 AcquireTime(void)
    {
        if (*TimeState.Timer._IsActive) { return (u32)(timeGetTime() * TIMER_VALUE_MULTIPLIER); }

        TimeState.Counter.Previous.LowPart = TimeState.Counter.Current.LowPart;
        TimeState.Counter.Previous.HighPart = TimeState.Counter.Current.HighPart;

        QueryPerformanceCounter(&TimeState.Counter.Current);

        const auto lp = TimeState.Counter.Current.LowPart - TimeState.Counter.Previous.LowPart;

        const auto hp = (TimeState.Counter.Current.HighPart - TimeState.Counter.Previous.HighPart)
            - (s32)(TimeState.Counter.Current.LowPart < TimeState.Counter.Previous.LowPart);

        const auto dh = TimeState.Counter.Current.HighPart - TimeState.Counter.Previous.HighPart;
        const auto dhc = dh - (s32)(TimeState.Counter.Current.LowPart < TimeState.Counter.Previous.LowPart);

        if ((dh != dhc) == hp < 0)
        {
            TimeState.Counter.Delta.Value = (s32)round((f64)((((s64)hp) << 32) + lp) * TimeState.Counter.Performance.Quotient);

            TimeState.Counter.Delta.LowPart = lp;
            TimeState.Counter.Delta.HighPart = hp;

            if (0 < TimeState.Counter.Delta.Value)
            {
                TimeState.Counter.Time = TimeState.Counter.Time + TimeState.Counter.Delta.Value;
            }
        }

        return TimeState.Counter.Time;
    }

    // 0x0060b010
    void ReleaseTime(void)
    {
        timeEndPeriod(TIMER_PERIOD_LENGTH);
    }
}