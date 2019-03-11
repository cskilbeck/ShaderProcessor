#pragma once

struct Timer
{
    uint64 base;

    Timer()
    {
        reset();
    };

    void reset()
    {
        QueryPerformanceCounter((LARGE_INTEGER *)&base);
    }

    double time()
    {
        uint64 now;
        uint64 freq;
        QueryPerformanceCounter((LARGE_INTEGER *)&now);
        QueryPerformanceFrequency((LARGE_INTEGER *)&freq);
        return (now - base) / (double)freq;
    }

    // get elapsed since last time and reset it

    double elapsed()
    {
        uint64 now;
        uint64 freq;
        QueryPerformanceCounter((LARGE_INTEGER *)&now);
        QueryPerformanceFrequency((LARGE_INTEGER *)&freq);
        double e = (now - base) / (double)freq;
        base = now;
        return e;
    }
};
