#ifndef DEF_DIAG_DEBUG_
#define DEF_DIAG_DEBUG_

#ifdef __cplusplus
extern "C" {
#endif

#include "Core/Std/Platform.h"
#include "Core/Std/Types.h"

typedef enum DIAG_FailureType {
    DIAG_FAILURE_TYPE_VERIFY = 0,
    DIAG_FAILURE_TYPE_PANIC,
    DIAG_FAILURE_TYPE_ASSERTION,
    DIAG_FAILURE_TYPE_UNREACHABLE,
    DIAG_FAILURE_TYPE_UNIMPLEMENTED,
} DIAG_FailureType;

typedef struct DIAG_FailureInfo {
    DIAG_FailureType type;
    const STD_Char8 *pExpr;
    const STD_Char8 *pMsg;
    const STD_Char8 *pFile;
    const STD_Char8 *pFunc;
    STD_U32          line;
    // Future implementation of a stack trace possibility.
} DIAG_FailureInfo;

#define DIAG_FAILURE_INFO_TMPL(failure_type, pFailure_expr, pFailure_msg)      \
    (DIAG_FailureInfo) {                                                       \
        .type = (failure_type), .pExpr = (pFailure_expr),                      \
        .pMsg = (pFailure_msg), pFile = __FILE__, .pFunc = __func__,           \
        .line = __LINE__                                                       \
    }

#define DIAG_FAILURE_DEFAULT_LOG_FILE (stderr)

/**
 * Logs the failure that happened to the DIAG_FAILURE_DEFAULT_LOG_FILE.
 *
 * @param pFail_info The info specifying how the failure happened.
 *                   Must not equal nullptr.
 */
STD_API void STD_CALL DIAG_FailureLog(const DIAG_FailureInfo *pFail_info);

/* ----  VERIFY ---- */

#define DIAG_Verify(expr)                                                      \
    do {                                                                       \
        if (!(expr)) {                                                         \
            DIAG_FailureInfo info = DIAG_FAILURE_INFO_TMPL(                    \
                DIAG_FAILURE_TYPE_VERIFY, #expr, nullptr);                     \
            DIAG_FailureInfo(&info);                                           \
            STD_DEBUG_BREAK();                                                 \
            STD_ABORT();                                                       \
        }                                                                      \
    } while (0)

#define DIAG_VerifyMsg(expr, pMsg)                                             \
    do {                                                                       \
        if (!(expr)) {                                                         \
            DIAG_FailureInfo info =                                            \
                DIAG_FAILURE_INFO_TMPL(DIAG_FAILURE_TYPE_VERIFY, #expr, pMsg); \
            DIAG_FailureInfo(&info);                                           \
            STD_DEBUG_BREAK();                                                 \
            STD_ABORT();                                                       \
        }                                                                      \
    } while (0)

/* ----  PANIC ---- */

#define DIAG_Panic()                                                           \
    do {                                                                       \
        DIAG_FailureInfo info =                                                \
            DIAG_FAILURE_INFO_TMPL(DIAG_FAILURE_TYPE_PANIC, nullptr, nullptr); \
        DIAG_FailureInfo(&info);                                               \
        STD_DEBUG_BREAK();                                                     \
        STD_ABORT();                                                           \
    } while (0)

#define DIAG_PanicMsg(pMsg)                                                    \
    do {                                                                       \
        DIAG_FailureInfo info =                                                \
            DIAG_FAILURE_INFO_TMPL(DIAG_FAILURE_TYPE_PANIC, nullptr, pMsg);    \
        DIAG_FailureInfo(&info);                                               \
        STD_DEBUG_BREAK();                                                     \
        STD_ABORT();                                                           \
    } while (0)

/* ----  UNIMPLEMENTED/UNREACHABLE ---- */

#define DIAG_Unimplemented()                                                   \
    do {                                                                       \
        DIAG_FailureInfo info = DIAG_FAILURE_INFO_TMPL(                        \
            DIAG_FAILURE_TYPE_UNIMPLEMENTED, nullptr, nullptr);                \
        DIAG_FailureInfo(&info);                                               \
        STD_DEBUG_BREAK();                                                     \
    } while (0)

#ifdef STD_COMPILE_MODE_RELEASE
    #define DIAG_Unreachable() STD_UNREACHABLE()
#elifdef STD_COMPILE_MODE_DEBUG
    #define DIAG_Unreachable()                                                 \
        do {                                                                   \
            DIAG_FailureInfo info = DIAG_FAILURE_INFO_TMPL(                    \
                DIAG_FAILURE_TYPE_UNREACHABLE, nullptr, nullptr);              \
            DIAG_FailureInfo(&info);                                           \
            STD_DEBUG_BREAK();                                                 \
            STD_ABORT();                                                       \
        } while (0)
#endif

/* ----  STATIC ASSERT ---- */

#define DIAG_StaticAssert(expr, pMsg) static_assert(expr, pMsg)

/* ----  ASSERT ---- */

#ifdef STD_COMPILE_MODE_RELEASE
    #define DIAG_Assert(expr)          ((void)0)
    #define DIAG_AssertMsg(expr, pMsg) ((void)0)
#elifdef STD_COMPILE_MODE_DEBUG
    #define DIAG_Assert(expr)                                                  \
        do {                                                                   \
            if (!(expr)) {                                                     \
                DIAG_FailureInfo info = DIAG_FAILURE_INFO_TMPL(                \
                    DIAG_FAILURE_TYPE_ASSERTION, #expr, nullptr);              \
                DIAG_FailureInfo(&info);                                       \
                STD_DEBUG_BREAK();                                             \
                STD_ABORT();                                                   \
            }                                                                  \
        } while (0)

    #define DIAG_AssertMsg(expr, pMsg)                                         \
        do {                                                                   \
            if (!(expr)) {                                                     \
                DIAG_FailureInfo info = DIAG_FAILURE_INFO_TMPL(                \
                    DIAG_FAILURE_TYPE_ASSERTION, #expr, pMsg);                 \
                DIAG_FailureInfo(&info);                                       \
                STD_DEBUG_BREAK();                                             \
                STD_ABORT();                                                   \
            }                                                                  \
        } while (0)
#endif

#ifdef __cplusplus
}
#endif

#endif
