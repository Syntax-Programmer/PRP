#ifndef DEF_DIAG_LOG_
#define DEF_DIAG_LOG_

#ifdef __cplusplus
extern "C" {
#endif

#include "Core/Std/Platform.h"
#include "Core/Std/Types.h"
#include <stdio.h>

#define DIAG_LOG_DEFAULT_FILE (stdout)

/**
 * Formats a printf-style message into pDest.
 *
 * @param pDest Output destination to the  formatted string.
 *              Must not equal nullptr.
 * @param size  The size in bytes of the destination string buffer.
 *              Must be greater than zero.
 * @param pMsg  The message to format into the destination.
 *              Must not equal nullptr.
 *
 * @return Positive non-zero len of the formatted string on success.
 * @return -1 if the arguments are invalid.
 * @return -1 if the formatting fails.
 */
STD_API STD_I32 STD_CALL DIAG_LogFormat(STD_Char8 *pDest, STD_Size size,
                                        const STD_Char8 *pMsg, ...)
    STD_PRINTF_FORMAT(3, 4);

/**
 * Writes a single character to the given file.
 *
 * @param pFile Output file destination of the write.
 *              Must not equal nullptr.
 * @param c     The character to write.
 *
 * @return STD_SUCCESS     on success.
 * @return STD_ERR_INV_ARG if arguments are invalid.
 * @return STD_ERR_IO      if write operation failed.
 */
STD_API STD_Rslt STD_CALL DIAG_LogWriteC(FILE *pFile, STD_Char8 c);
/**
 * Writes the given string to the given file.
 *
 * @param pFile Output file destination of the write.
 *              Must not equal nullptr.
 * @param pMsg  The string to write.
 *              Must not equal nullptr.
 * @param len   The length of the string to write.
 *
 * @return STD_SUCCESS     on success.
 * @return STD_ERR_INV_ARG if arguments are invalid.
 * @return STD_ERR_IO      if write operation failed.
 */
STD_API STD_Rslt STD_CALL DIAG_LogWrite(FILE *pFile, const STD_Char8 *pMsg,
                                        STD_Size len);
/**
 * Writes the given string to the given file and inserts a newline character at
 * the end.
 *
 * @param pFile Output file destination of the write.
 *              Must not equal nullptr.
 * @param pMsg  The string to write.
 *              Must not equal nullptr.
 * @param len   The length of the string to write.
 *
 * @return STD_SUCCESS     on success.
 * @return STD_ERR_INV_ARG if arguments are invalid.
 * @return STD_ERR_IO      if write operation failed.
 */
STD_API STD_Rslt STD_CALL DIAG_LogWriteLn(FILE *pFile, const STD_Char8 *pMsg,
                                          STD_Size len);

/**
 * Formats a printf-style string into the given file.
 *
 * @param pFile Output file destination of the write.
 *              Must not equal nullptr.
 * @param pMsg  The string to format into the given file.
 *              Must not equal nullptr.
 *
 * @return STD_SUCCESS     on success.
 * @return STD_ERR_INV_ARG if arguments are invalid.
 * @return STD_ERR_IO      if write operation failed.
 */
STD_API STD_Rslt STD_CALL DIAG_LogWriteFmt(FILE *pFile, const STD_Char8 *pMsg,
                                           ...) STD_PRINTF_FORMAT(2, 3);

/**
 * Formats a printf-style string into the given file and inserts a newline
 * character at the end.
 *
 * @param pFile Output file destination of the write.
 *              Must not equal nullptr.
 * @param pMsg  The string to format into the given file.
 *              Must not equal nullptr.
 *
 * @return STD_SUCCESS     on success.
 * @return STD_ERR_INV_ARG if arguments are invalid.
 * @return STD_ERR_IO      if write operation failed.
 */
STD_API STD_Rslt STD_CALL DIAG_LogWriteLnFmt(FILE *pFile, const STD_Char8 *pMsg,
                                             ...) STD_PRINTF_FORMAT(2, 3);

/* ----  LOGGING ABSTRACTIONS ---- */

#define DIAG_LOG_MAX_MSG_LEN ((1 << 11) - 1)

typedef enum DIAG_LogLvl {
    DIAG_LOG_LVL_TRACE = 0,
    DIAG_LOG_LVL_DEBUG = 1,
    DIAG_LOG_LVL_INFO  = 2,
    DIAG_LOG_LVL_WARN  = 3,
    DIAG_LOG_LVL_ERROR = 4,
    DIAG_LOG_LVL_FATAL = 5,
} DIAG_LogLvl;

/**
 * Writes a structured printf-style formatted log line to pF_out with levels,
 * location and message.
 *
 * @param pF_out Output file destination of the write
 *               Must not equal nullptr.
 * @param lvl    The severity level of the message.
 * @param pFile  The source file name where the log is originating from.
 * @param line   The source line number where the log is being made.
 * @param pMsg   The string to format as the message.
 *
 * @return STD_SUCCESS     on success.
 * @return STD_ERR_INV_ARG if arguments are invalid.
 * @return STD_ERR_IO      if write operation failed.
 */
STD_API STD_Rslt STD_CALL DIAG_Log(FILE *pF_out, DIAG_LogLvl lvl,
                                   const STD_Char8 *pFile,
                                   const STD_Char8 *pFunc, STD_Size line,
                                   const STD_Char8 *pMsg, ...)
    STD_PRINTF_FORMAT(6, 7);

#define DIAG_LogTrace(pF_out, pMsg, ...)                                       \
    DIAG_Log((pF_out), DIAG_LOG_LVL_TRACE, __FILE__, __func__, __LINE__,       \
             (pMsg)__VA_OPT(, ) __VA_ARGS__)
#define DIAG_LogDebug(pF_out, pMsg, ...)                                       \
    DIAG_Log((pF_out), DIAG_LOG_LVL_DEBUG, __FILE__, __func__, __LINE__,       \
             (pMsg)__VA_OPT(, ) __VA_ARGS__)
#define DIAG_LogInfo(pF_out, pMsg, ...)                                        \
    DIAG_Log((pF_out), DIAG_LOG_LVL_INFO, __FILE__, __func__, __LINE__,        \
             (pMsg)__VA_OPT(, ) __VA_ARGS__)
#define DIAG_LogWarn(pF_out, pMsg, ...)                                        \
    DIAG_Log((pF_out), DIAG_LOG_LVL_WARN, __FILE__, __func__, __LINE__,        \
             (pMsg)__VA_OPT(, ) __VA_ARGS__)
#define DIAG_LogError(pF_out, pMsg, ...)                                       \
    DIAG_Log((pF_out), DIAG_LOG_LVL_ERROR, __FILE__, __func__, __LINE__,       \
             (pMsg)__VA_OPT(, ) __VA_ARGS__)
#define DIAG_LogFatal(pF_out, pMsg, ...)                                       \
    DIAG_Log((pF_out), DIAG_LOG_LVL_FATAL, __FILE__, __func__, __LINE__,       \
             (pMsg)__VA_OPT(, ) __VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif
