#include "Debug.h"
#include "Log.h"

STD_API void STD_CALL DIAG_FailureLog(const DIAG_FailureInfo *pFail_info) {
    if (!pFail_info) {
        return;
    }

    const STD_Char8 *pInit_str;
    switch (pFail_info->type) {
    case DIAG_FAILURE_TYPE_VERIFY:
        pInit_str = "VERIFY FAILED";
        break;
    case DIAG_FAILURE_TYPE_PANIC:
        pInit_str = "ENGINE PANIC";
        break;
    case DIAG_FAILURE_TYPE_ASSERTION:
        pInit_str = "ASSERTION FAILED";
        break;
    case DIAG_FAILURE_TYPE_UNIMPLEMENTED:
        pInit_str = "UNREACHABLE REACHED";
        break;
    case DIAG_FAILURE_TYPE_UNREACHABLE:
        pInit_str = "UNIMPLEMENTED CODE REACHED";
        break;
    default:
        pInit_str = "UNKNOWN FAILURE CODE";
        break;
    }

    DIAG_Log(DIAG_FAILURE_DEFAULT_LOG_FILE, DIAG_LOG_LVL_ERROR,
             pFail_info->pFile, pFail_info->pFunc, pFail_info->line,
             "%s: (%s) : %s", pInit_str, pFail_info->pExpr, pFail_info->pMsg);
}
