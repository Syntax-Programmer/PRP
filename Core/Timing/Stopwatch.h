#ifndef DEF_TIMING_STOPWATCH_
#define DEF_TIMING_STOPWATCH_

#ifdef __cplusplus
extern "C" {
#endif

#include "Time.h"

typedef struct TIME_Stopwatch {
    TIME_Tick start;
    TIME_Tick elapsed;
    STD_Bool  is_running;
} TIME_Stopwatch;

/**
 * Creates a new stopwatch that is not started.
 */
#define TIME_StopwatchCreator ((TIME_Stopwatch){0})

/**
 * Starts the stopwatch, beginning accumulation of elapsed ticks.
 *
 * @param pSw Pointer to the stopwatch object to start.
 *            Must not equal nullptr.
 *
 * @return STD_SUCCESS       on success.
 * @return STD_ERR_INV_ARG   if arguments are invalid.
 * @return STD_ERR_INV_STATE if the stopwatch is already running.
 */
[[nodiscard("This function can fail, please check result for success.")]]
STD_API STD_Rslt STD_CALL TIME_StopwatchStart(TIME_Stopwatch *pSw);
/**
 * Stops the stopwatch,  finalizing accumulation of elapsed ticks.
 *
 * @param pSw Pointer to the stopwatch object to stop.
 *            Must not equal nullptr.
 *
 * @return STD_SUCCESS       on success.
 * @return STD_ERR_INV_ARG   if arguments are invalid.
 * @return STD_ERR_INV_STATE if the stopwatch is already stopped.
 */
[[nodiscard("This function can fail, please check result for success.")]]
STD_API STD_Rslt STD_CALL TIME_StopwatchStop(TIME_Stopwatch *pSw);
/**
 * Resets the stopwatch.
 *
 * @param pSw Pointer to the stopwatch object to reset.
 *            Must not equal nullptr.
 *
 * @return STD_SUCCESS       on success.
 * @return STD_ERR_INV_ARG   if arguments are invalid.
 */
[[nodiscard("This function can fail, please check result for success.")]]
STD_API STD_Rslt STD_CALL TIME_StopwatchReset(TIME_Stopwatch *pSw);
/**
 * Finds the total accumulated elapsed ticks of the stopwatch.
 *
 * @param pSw    Pointer to the stopwatch object to get accumulated elapsed
 *               ticks of.
 *               Must not equal nullptr.
 * @param pTicks Output pointer that receives the elapsed ticks.
 *               Must not equal nullptr.
 *
 * @return STD_SUCCESS       on success.
 * @return STD_ERR_INV_ARG   if arguments are invalid.
 */
[[nodiscard("This function can fail, please check result for success.")]]
STD_API STD_Rslt STD_CALL TIME_StopwatchElapsedTicks(const TIME_Stopwatch *pSw,
                                                     TIME_Tick *pTicks);
/**
 * Finds the total accumulated elapsed time of the stopwatch.
 *
 * @param pSw   Pointer to the stopwatch object to get accumulated elapsed
 *              time of.
 *              Must not equal nullptr.
 * @param pTime Output pointer that receives the elapsed time.
 *              Must not equal nullptr.
 * @param unit  The unit to receive the elapsed time in.
 *              Must not be a non-TIME_Unit value.
 *
 * @return STD_SUCCESS       on success.
 * @return STD_ERR_INV_ARG   if arguments are invalid.
 */
[[nodiscard("This function can fail, please check result for success.")]]
STD_API STD_Rslt STD_CALL TIME_StopwatchElapsed(const TIME_Stopwatch *pSw,
                                                STD_F64 *pTime, TIME_Unit unit);

#ifdef __cplusplus
}
#endif

#endif
