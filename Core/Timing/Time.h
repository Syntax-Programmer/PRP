#ifndef DEF_TIMING_TIME_
#define DEF_TIMING_TIME_

#ifdef __cplusplus
extern "C" {
#endif

#include "Core/Std/Platform.h"
#include "Core/Std/Types.h"

typedef STD_U64 TIME_Tick;

typedef enum TIME_Unit {
    TIME_UNIT_NS = 0,
    TIME_UNIT_US,
    TIME_UNIT_MS,
    TIME_UNIT_S,
} TIME_Unit;

/**
 * Samples the current value of the platform's monotonic clock.
 *
 * @return The current tick count of the monotonic clock.
 */
[[nodiscard("Must not call functions when you don't use their result.")]]
STD_API TIME_Tick STD_CALL TIME_TimeNow(void);
/**
 * Queries the frequency of the platform's monotonic clock in ticks per second.
 *
 * @return The number of ticks that elapse in one physical second.
 */
[[nodiscard("Must not call functions when you don't use their result.")]]
STD_API TIME_Tick STD_CALL TIME_TimeFreq(void);
/**
 * Converts a raw tick count to the specified physical time unit.
 *
 * @param tick The tick count to convert.
 * @param unit The target time unit to covert the tick into.
 *
 * @return The duration represented in the requested time units.
 * @return -1.0f If the given unit is not recognized.
 */
[[nodiscard("Must not call functions when you don't use their result.")]]
STD_API STD_F64 STD_CALL TIME_TicksToUnit(TIME_Tick tick, TIME_Unit unit);

/**
 * Suspends the calling thread for at least the specified milliseconds.
 *
 * @param ms The milliseconds to suspend the thread at least for.
 */
STD_API void STD_CALL TIME_Sleep(STD_F64 ms);
/**
 * Suspends the calling thread for at least the specified ticks.
 *
 * @param tick The ticks to suspend the thread at least for.
 */
STD_API void STD_CALL TIME_SleepTicks(TIME_Tick tick);
/**
 * Hints to the OS scheduler that the calling thread is willing to yield its
 * remaining time slice.
 */
STD_API void STD_CALL TIME_Yield(void);

#ifdef __cplusplus
}
#endif

#endif
