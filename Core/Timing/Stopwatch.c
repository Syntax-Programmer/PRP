#include "Stopwatch.h"

STD_API STD_Rslt STD_CALL TIME_StopwatchStart(TIME_Stopwatch *pSw) {
    if (!pSw) {
        return STD_ERR_INV_ARG;
    }
    if (pSw->is_running) {
        return STD_ERR_INV_STATE;
    }

    pSw->start      = TIME_TimeNow();
    pSw->is_running = true;

    return STD_SUCCESS;
}

STD_API STD_Rslt STD_CALL TIME_StopwatchStop(TIME_Stopwatch *pSw) {
    if (!pSw) {
        return STD_ERR_INV_ARG;
    }
    if (!pSw->is_running) {
        return STD_ERR_INV_STATE;
    }

    pSw->elapsed    += TIME_TimeNow() - pSw->start;
    pSw->start       = 0;
    pSw->is_running  = false;

    return STD_SUCCESS;
}

STD_API STD_Rslt STD_CALL TIME_StopwatchReset(TIME_Stopwatch *pSw) {
    if (!pSw) {
        return STD_ERR_INV_ARG;
    }

    *pSw = TIME_StopwatchCreator;

    return STD_SUCCESS;
}

STD_API STD_Rslt STD_CALL TIME_StopwatchElapsedTicks(const TIME_Stopwatch *pSw,
                                                     TIME_Tick *pTicks) {
    if (!pSw || !pTicks) {
        return STD_ERR_INV_ARG;
    }

    *pTicks = pSw->elapsed;
    if (pSw->is_running) {
        *pTicks += TIME_TimeNow() - pSw->start;
    }

    return STD_SUCCESS;
}

STD_API STD_Rslt STD_CALL TIME_StopwatchElapsed(const TIME_Stopwatch *pSw,
                                                STD_F64              *pTime,
                                                TIME_Unit             unit) {
    if (!pSw || !pTime) {
        return STD_ERR_INV_ARG;
    }

    TIME_Tick ticks = pSw->elapsed;
    if (pSw->is_running) {
        ticks += TIME_TimeNow() - pSw->start;
    }

    *pTime = TIME_TicksToUnit(ticks, unit);
    if (*pTime < 0.0f) {
        return STD_ERR_INV_ARG;
    }

    return STD_SUCCESS;
}
