#ifndef DEF_TIMING_TIMER_
#define DEF_TIMING_TIMER_

#ifdef __cplusplus
extern "C" {
#endif

#include "Time.h"

typedef TIME_Tick TIME_Timer;

/**
 * Creates/Starts a new one-shot timer.
 */
#define TIME_TimerInitializer ((TIME_Timer)TIME_TimeNow())

/**
 * Finds the number of ticks elapsed since the timer was created.
 *
 * @param timer The timer to find elapsed ticks for.
 *
 * @return The ticks that elapsed b/w timer creation and this function call.
 */
[[nodiscard("Must not call functions when you don't use their result.")]]
STD_API TIME_Tick STD_CALL TIME_TimerElapsedTicks(TIME_Timer timer);
/**
 * Finds the time elapsed since the timer was created.
 *
 * @param timer The timer to find elapsed ticks for.
 * @param unit  The target time unit to get the result in.
 *
 * @return The time elapsed in the given physical time unit.
 */
[[nodiscard("Must not call functions when you don't use their result.")]]
STD_API STD_F64 STD_CALL TIME_TimerElapsed(TIME_Timer timer, TIME_Unit unit);

#ifdef __cplusplus
}
#endif

#endif
