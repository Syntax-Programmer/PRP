#ifndef DEF_TIMING_FRAME_TIMER_
#define DEF_TIMING_FRAME_TIMER_

#ifdef __cplusplus
extern "C" {
#endif

#include "Time.h"

#define TIME_FRAME_TIMER_ACCUMULATION_TICKS (500'000'000)

typedef struct TIME_FrameTimer {
    TIME_Tick frame_start;
    TIME_Tick frame_delta;
    STD_U64   frame_count;
    TIME_Tick fps_accumulator;
    STD_U64   accumulated_frame_count;
    STD_F64   fps;
} TIME_FrameTimer;

/**
 * Creates a new frame timer that is not started.
 */
#define TIME_FrameTimerCreator                                                 \
    ((TIME_FrameTimer){.frame_start = TIME_TimeNow()})

/**
 * Marks the beginning of  anew frame, updating all frame timing metadata.
 *
 * @param pF_timer Pointer to the frame timer object to start.
 *                 Must not equal nullptr.
 *
 * @return STD_SUCCESS       on success.
 * @return STD_ERR_INV_ARG   if arguments are invalid.
 */
[[nodiscard("This function can fail, please check result for success.")]]
STD_API STD_Rslt STD_CALL TIME_FrameTimerBegin(TIME_FrameTimer *pF_timer);
/**
 * Resets the frame timer.
 *
 * @param pF_timer Pointer to the frame timer object to reset.
 *                 Must not equal nullptr.
 *
 * @return STD_SUCCESS       on success.
 * @return STD_ERR_INV_ARG   if arguments are invalid.
 */
[[nodiscard("This function can fail, please check result for success.")]]
STD_API STD_Rslt STD_CALL TIME_FrameTimerReset(TIME_FrameTimer *pF_timer);
/**
 * Finds the tick duration for the most recent frame completion.
 *
 * @param pF_timer Pointer to the frame timer object to get delta ticks of.
 *                 Must not equal nullptr.
 * @param pTicks   Output pointer that receives the delta ticks.
 *                 Must not equal nullptr.
 *
 * @return STD_SUCCESS       on success.
 * @return STD_ERR_INV_ARG   if arguments are invalid.
 */
[[nodiscard("This function can fail, please check result for success.")]]
STD_API STD_Rslt STD_CALL
TIME_FrameTimerDeltaTicks(const TIME_FrameTimer *pF_timer, TIME_Tick *pTicks);
/**
 * Finds the time duration for the most recent frame completion.
 *
 * @param pF_timer Pointer to the frame timer object to get delta time of.
 *                 Must not equal nullptr.
 * @param pTime    Output pointer that receives the delta time.
 *                 Must not equal nullptr.
 * @param unit    The unit to receive the delta time in.
 *                Must not be a non-TIME_Unit value.
 *
 * @return STD_SUCCESS       on success.
 * @return STD_ERR_INV_ARG   if arguments are invalid.
 */
[[nodiscard("This function can fail, please check result for success.")]]
STD_API STD_Rslt STD_CALL TIME_FrameTimerDeltaTime(
    const TIME_FrameTimer *pF_timer, TIME_Tick *pTime, TIME_Unit unit);
/**
 * Finds the current fps the frame timer is measuring.
 *
 * @param pF_timer Pointer to the frame timer object to get fps of.
 *                 Must not equal nullptr.
 * @param pFPS     Output pointer that receives the fps.
 *                 Must not equal nullptr.
 *
 * @return STD_SUCCESS       on success.
 * @return STD_ERR_INV_ARG   if arguments are invalid.
 */
[[nodiscard("This function can fail, please check result for success.")]]
STD_API STD_Rslt STD_CALL TIME_FrameTimerFPS(const TIME_FrameTimer *pF_timer,
                                             STD_F64               *pFPS);
/**
 * Finds the total frames measured by frame timer.
 *
 * @param pF_timer     Pointer to the frame timer object to get frame count of.
 *                     Must not equal nullptr.
 * @param pFrame_count Output pointer that receives the frame count.
 *                     Must not equal nullptr.
 *
 * @return STD_SUCCESS       on success.
 * @return STD_ERR_INV_ARG   if arguments are invalid.
 */
[[nodiscard("This function can fail, please check result for success.")]]
STD_API
    STD_Rslt STD_CALL TIME_FrameTimerFrameCount(const TIME_FrameTimer *pF_timer,
                                                STD_U64 *pFrame_count);

#ifdef __cplusplus
}
#endif

#endif
