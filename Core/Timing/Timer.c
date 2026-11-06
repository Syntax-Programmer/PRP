#include "Timer.h"

STD_API TIME_Tick STD_CALL TIME_TimerElapsedTicks(TIME_Timer timer) {
    return TIME_TimeNow() - timer;
}

STD_API STD_F64 STD_CALL TIME_TimerElapsed(TIME_Timer timer, TIME_Unit unit) {
    return TIME_TicksToUnit(TIME_TimerElapsedTicks(timer), unit);
}
