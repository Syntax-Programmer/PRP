#include "Time.h"

#ifdef STD_PLATFORM_WEB
    #include <emscripten/emscripten.h>

static TIME_Tick time_s_freq = 1'000'000'000ULL;

#elif defined(STD_PLATFORM_LINUX) || defined(STD_PLATFORM_ANDROID)
    #include <errno.h>
    #include <sched.h>
    #include <time.h>

static TIME_Tick time_s_freq = 1'000'000'000ULL;

#elifdef STD_PLATFORM_WINDOWS
    #include <windows.h>

static TIME_Tick time_s_freq = 0;

#elif defined(STD_PLATFORM_MACOS) || defined(STD_PLATFORM_IOS)
    #include <errno.h>
    #include <mach/mach_time.h>
    #include <sched.h>

static TIME_Tick time_s_freq = 0;

#else
    #error Unsupported platform detected.
#endif

STD_API TIME_Tick STD_CALL TIME_TimeNow(void) {
#ifdef STD_PLATFORM_WEB
    return (TIME_Tick)(emscripten_get_now() * ((time_s_freq) / 1000.0));

#elif defined(STD_PLATFORM_LINUX) || defined(STD_PLATFORM_ANDROID)
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);

    return ((TIME_Tick)ts.tv_sec * time_s_freq) + (TIME_Tick)ts.tv_nsec;

#elifdef STD_PLATFORM_WINDOWS
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);

    return (TIME_Tick)counter.QuadPart;

#elif defined(STD_PLATFORM_MACOS) || defined(STD_PLATFORM_IOS)
    return (TIME_Tick)mach_absolute_time();
#else
    #error Unsupported platform detected.
#endif
}

STD_API TIME_Tick STD_CALL TIME_TimeFreq(void) {
#ifdef STD_PLATFORM_WINDOWS
    if (time_s_freq == 0) {
        LARGE_INTEGER freq;
        QueryPerformanceFrequency(&freq);
        time_s_freq = (PRP_TimeTicks)freq.QuadPart;
    }
#elif defined(STD_PLATFORM_MACOS) || defined(STD_PLATFORM_IOS)
    if (time_s_freq == 0) {
        mach_timebase_info_data_t timebase;
        mach_timebase_info(&timebase);
        time_s_freq = (1000000000ULL * timebase.denom) / timebase.numer;
    }
#endif

    return time_s_freq;
}

STD_API STD_F64 STD_CALL TIME_TicksToUnit(TIME_Tick tick, TIME_Unit unit) {
    switch (unit) {
    case TIME_UNIT_NS:
        return ((STD_F64)tick * 1'000'000'000.0) / (STD_F64)TIME_TimeFreq();
    case TIME_UNIT_US:
        return ((STD_F64)tick * 1'000'000.0) / (STD_F64)TIME_TimeFreq();
    case TIME_UNIT_MS:
        return ((STD_F64)tick * 1'000.0) / (STD_F64)TIME_TimeFreq();
    case TIME_UNIT_S:
        return (STD_F64)tick / (STD_F64)TIME_TimeFreq();
    default:
        return -1;
    }
}

STD_API void STD_CALL TIME_Sleep(STD_F64 ms) {
    if (ms <= 0.0) {
        return;
    }

#ifdef STD_PLATFORM_WINDOWS
    STD_U64 whole_ms = (STD_U64)ms;
    if ((STD_F64)whole_ms < ms) {
        whole_ms++;
    }
    while (whole_ms > STD_U32_MAX) {
        Sleep(STD_U32_MAX);
        whole_ms -= STD_U32_MAX;
    }
    Sleep((DWORD)whole_ms);

#elifdef STD_PLATFORM_WEB
    STD_U64 whole_ms = (STD_U64)ms;
    if ((STD_F64)whole_ms < ms) {
        whole_ms++;
    }
    while (whole_ms > STD_U32_MAX) {
        emscripten_sleep(STD_U32_MAX);
        whole_ms -= STD_U32_MAX;
    }
    emscripten_sleep((DWORD)whole_ms);

#elif defined(STD_PLATFORM_LINUX) || defined(STD_PLATFORM_ANDROID) ||          \
    defined(STD_PLATFORM_MACOS) || defined(STD_PLATFORM_IOS)
    STD_F64         sec = ms / 1000.0f;
    struct timespec req;

    req.tv_sec  = (time_t)sec;
    req.tv_nsec = (long)(sec - (STD_F64)(req.tv_sec) * 1'000'000'000.0f);
    while (nanosleep(&req, &req) == -1 && errno == EINTR)
        ;

#else
    #error Unsupported platform detected.
#endif
}

STD_API void STD_CALL TIME_SleepTicks(TIME_Tick tick) {
    if (tick == 0) {
        return;
    }

    STD_F64 ms = TIME_TicksToUnit(tick, TIME_UNIT_MS);
    TIME_Sleep(ms);
}

STD_API void STD_CALL TIME_Yield(void) {
#ifdef STD_PLATFORM_WINDOWS
    (void)SwitchToThread();

#elifdef STD_PLATFORM_WEB
    #ifdef __EMSCRIPTEN_PTHREADS__
    (void)sched_yield();
    #endif

#elif defined(STD_PLATFORM_LINUX) || defined(STD_PLATFORM_ANDROID) ||          \
    defined(STD_PLATFORM_MACOS) || defined(STD_PLATFORM_IOS)
    (void)sched_yield();

#else
    #error Unsupported platform detected.
#endif
}
