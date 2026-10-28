#include "Log.h"
#include <pthread.h>
#include <stdarg.h>

#ifdef STD_COMPILE_MODE_DEBUG
    #warning                                                                   \
        "The current implementation of Log.c, uses pthread.h as the thread interface. In future to be replaced with dedicated Threading library."
#endif

static pthread_mutex_t log_mtx = PTHREAD_MUTEX_INITIALIZER;

STD_API STD_I32 STD_CALL STD_PRINTF_FORMAT(3, 4)
    DIAG_LogFormat(STD_Char8 *pDest, STD_Size size, const STD_Char8 *pMsg,
                   ...) {
    if (!pDest || size == 0 || !pMsg) {
        return -1;
    }

    va_list args;
    va_start(args, pMsg);
    STD_I32 rslt = vsnprintf(pDest, size, pMsg, args);
    va_end(args);

    return rslt;
}

STD_API STD_Rslt STD_CALL DIAG_LogWriteC(FILE *pFile, STD_Char8 c) {
    if (!pFile) {
        return STD_ERR_INV_ARG;
    }

    STD_Rslt code = STD_SUCCESS;
    pthread_mutex_lock(&log_mtx);
    if (fwrite(&c, 1, 1, pFile) != 1) {
        code = STD_ERR_IO;
    }
    pthread_mutex_unlock(&log_mtx);

    return code;
}

STD_API STD_Rslt STD_CALL DIAG_LogWrite(FILE *pFile, const STD_Char8 *pMsg,
                                        STD_Size len) {
    if (!pFile || !pMsg) {
        return STD_ERR_INV_ARG;
    }

    STD_Rslt code = STD_SUCCESS;
    pthread_mutex_lock(&log_mtx);
    if (fwrite(pMsg, 1, len, pFile) != len) {
        code = STD_ERR_IO;
    }
    pthread_mutex_unlock(&log_mtx);

    return code;
}

STD_API STD_Rslt STD_CALL DIAG_LogWriteLn(FILE *pFile, const STD_Char8 *pMsg,
                                          STD_Size len) {
    if (!pFile || !pMsg) {
        return STD_ERR_INV_ARG;
    }

    STD_Rslt code = STD_SUCCESS;
    pthread_mutex_lock(&log_mtx);
    if (fwrite(pMsg, 1, len, pFile) != len || fwrite("\n", 1, 1, pFile) != 1) {
        code = STD_ERR_IO;
    }
    pthread_mutex_unlock(&log_mtx);

    return code;
}

STD_API STD_Rslt STD_CALL STD_PRINTF_FORMAT(2, 3)
    DIAG_LogWriteFmt(FILE *pFile, const STD_Char8 *pMsg, ...) {
    if (!pFile || !pMsg) {
        return STD_ERR_INV_ARG;
    }

    va_list args;
    va_start(args, pMsg);
    STD_Rslt code = STD_SUCCESS;

    pthread_mutex_lock(&log_mtx);
    if (vfprintf(pFile, pMsg, args) < 0) {
        code = STD_ERR_IO;
    }
    pthread_mutex_unlock(&log_mtx);

    va_end(args);

    return code;
}

STD_API STD_Rslt STD_CALL STD_PRINTF_FORMAT(2, 3)
    DIAG_LogWriteLnFmt(FILE *pFile, const STD_Char8 *pMsg, ...) {
    if (!pFile || !pMsg) {
        return STD_ERR_INV_ARG;
    }

    va_list args;
    va_start(args, pMsg);
    STD_Rslt code = STD_SUCCESS;

    pthread_mutex_lock(&log_mtx);
    if (vfprintf(pFile, pMsg, args) < 0 || fwrite("\n", 1, 1, pFile) != 1) {
        code = STD_ERR_IO;
    }
    pthread_mutex_unlock(&log_mtx);

    va_end(args);

    return code;
}

/* ----  LOGGING ABSTRACTIONS ---- */

STD_API STD_Rslt STD_CALL STD_PRINTF_FORMAT(6, 7)
    DIAG_Log(FILE *pF_out, DIAG_LogLvl lvl, const STD_Char8 *pFile,
             const STD_Char8 *pFunc, STD_Size line, const STD_Char8 *pMsg,
             ...) {
    if (!pF_out) {
        return STD_ERR_INV_ARG;
    }

    pFile = (!pFile) ? "---" : pFile;
    pFunc = (!pFunc) ? "---" : pFunc;
    pMsg  = (!pMsg) ? "---" : pMsg;

    const STD_Char8 *pLvl_str;
    switch (lvl) {
    case DIAG_LOG_LVL_TRACE:
        pLvl_str = "TRACE";
        break;
    case DIAG_LOG_LVL_DEBUG:
        pLvl_str = "DEBUG";
        break;
    case DIAG_LOG_LVL_INFO:
        pLvl_str = "INFO";
        break;
    case DIAG_LOG_LVL_WARN:
        pLvl_str = "WARN";
        break;
    case DIAG_LOG_LVL_ERROR:
        pLvl_str = "ERROR";
        break;
    case DIAG_LOG_LVL_FATAL:
        pLvl_str = "FATAL";
        break;
    default:
        pLvl_str = "UNKNOWN";
        break;
    }

    va_list args;
    va_start(args, pMsg);

    // +1 to account for the nul-terminator implicitly put by the function.
    STD_Char8 bffr[DIAG_LOG_MAX_MSG_LEN + 1];
    STD_I32   fmt_rslt = vsnprintf(bffr, DIAG_LOG_MAX_MSG_LEN + 1, pMsg, args);

    va_end(args);

    if (fmt_rslt < 0) {
        return STD_ERR_IO;
    }

    return DIAG_LogWriteFmt(pF_out, "%s: [File: %s]{Func %s :-: Line %zu} %s\n",
                            pLvl_str, pFile, pFunc, line, bffr);
}
