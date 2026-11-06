#include "FrameTimer.h"

STD_API STD_Rslt STD_CALL TIME_FrameTimerBegin(TIME_FrameTimer *pF_timer) {
    if (!pF_timer) {
        return STD_ERR_INV_ARG;
    }

    TIME_Tick now              = TIME_TimeNow();
    pF_timer->frame_delta      = now - pF_timer->frame_start;
    pF_timer->frame_start      = now;
    pF_timer->fps_accumulator += pF_timer->frame_delta;
    pF_timer->accumulated_frame_count++;

    if (pF_timer->fps_accumulator >= TIME_FRAME_TIMER_ACCUMULATION_TICKS) {
        pF_timer->fps = ((STD_F64)TIME_TimeFreq() *
                         (STD_F64)pF_timer->accumulated_frame_count /
                         (STD_F64)pF_timer->fps_accumulator);
        pF_timer->fps_accumulator         = 0;
        pF_timer->accumulated_frame_count = 0;
    }
    pF_timer->frame_count++;

    return STD_SUCCESS;
}

STD_API STD_Rslt STD_CALL TIME_FrameTimerReset(TIME_FrameTimer *pF_timer) {
    if (!pF_timer) {
        return STD_ERR_INV_ARG;
    }

    *pF_timer = TIME_FrameTimerCreator;

    return STD_SUCCESS;
}

STD_API STD_Rslt STD_CALL
TIME_FrameTimerDeltaTicks(const TIME_FrameTimer *pF_timer, TIME_Tick *pTicks) {
    if (!pF_timer || !pTicks) {
        return STD_ERR_INV_ARG;
    }

    *pTicks = pF_timer->frame_delta;

    return STD_SUCCESS;
}

STD_API STD_Rslt STD_CALL TIME_FrameTimerDeltaTime(
    const TIME_FrameTimer *pF_timer, TIME_Tick *pTime, TIME_Unit unit) {
    if (!pF_timer || !pTime) {
        return STD_ERR_INV_ARG;
    }

    *pTime = TIME_TicksToUnit(pF_timer->frame_delta, unit);

    return STD_SUCCESS;
}

STD_API STD_Rslt STD_CALL TIME_FrameTimerFPS(const TIME_FrameTimer *pF_timer,
                                             STD_F64               *pFPS) {
    if (!pF_timer || !pFPS) {
        return STD_ERR_INV_ARG;
    }

    *pFPS = pF_timer->fps;

    return STD_SUCCESS;
}

STD_API STD_Rslt STD_CALL TIME_FrameTimerFrameC(const TIME_FrameTimer *pF_timer,
                                                STD_U64 *pFrame_count) {
    if (!pF_timer || !pFrame_count) {
        return STD_ERR_INV_ARG;
    }

    *pFrame_count = pF_timer->frame_count;

    return STD_SUCCESS;
}
