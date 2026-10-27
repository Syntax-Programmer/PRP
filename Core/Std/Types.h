#ifndef DEF_STD_TYPES_
#define DEF_STD_TYPES_

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>
#include <uchar.h>

/* ---- NUMBER TYPES ---- */

typedef uint64_t STD_U64;
#define STD_U64_MAX ((STD_U64)~0)
#define STD_U64_MIN ((STD_U64)0)
typedef int64_t STD_I64;
#define STD_I64_MAX ((STD_I64)0X7FFFFFFFFFFFFFFFll)
#define STD_I64_MIN ((STD_I64)(-0X8000000000000000ll))

typedef uint32_t STD_U32;
#define STD_U32_MAX ((STD_U32)~0)
#define STD_U32_MIN ((STD_U32)0)
typedef int32_t STD_I32;
#define STD_I32_MAX ((STD_I32)0X7FFFFFFF)
#define STD_I32_MIN ((STD_I32)(-0X80000000))

typedef uint16_t STD_U16;
#define STD_U16_MAX ((STD_U16)~0)
#define STD_U16_MIN ((STD_U16)0)
typedef int16_t STD_I16;
#define STD_I16_MAX ((STD_I16)0X7FFF)
#define STD_I16_MIN ((STD_I16)(-0X8000))

typedef uint8_t STD_U8;
#define STD_U8_MAX ((STD_U8)~0)
#define STD_U8_MIN ((STD_U8)0)
typedef int8_t STD_I8;
#define STD_I8_MAX ((STD_I8)0X7F)
#define STD_I8_MIN ((STD_I8)(-0X80))

typedef unsigned _BitInt(24) STD_U24;
typedef _BitInt(24) STD_I24;
typedef unsigned _BitInt(40) STD_U40;
typedef _BitInt(40) STD_I40;
typedef unsigned _BitInt(48) STD_U48;
typedef _BitInt(48) STD_I48;
typedef unsigned _BitInt(56) STD_U56;
typedef _BitInt(56) STD_I56;
#define PRP_BITINT_MAXWIDTH (BITINT_MAXWIDTH)

typedef float  STD_F32;
typedef double STD_F64;

/* ---- CHARACTER TYPES ---- */

typedef char8_t  STD_UChar8;
typedef char16_t STD_UChar16;
typedef char32_t STD_UChar32;

typedef char    STD_Char8;
typedef int16_t STD_Char16;
typedef int32_t STD_Char32;

/* ---- POINTER AND SIZES ---- */

typedef size_t STD_Size;
#define PRP_SIZE_MIN ((PRP_size)0)
#define PRP_SIZE_MAX (SIZE_MAX)

typedef nullptr_t STD_NullptrT;
typedef ptrdiff_t STD_PtrdiffT;
typedef uintptr_t STD_UintptrT;

/* ---- BOOLEAN ---- */

typedef bool STD_Bool;

/* ---- STD FUNCTION RETURN ---- */

typedef STD_U32 STD_Rslt;

#define STD_UNKNOWN_ERR        (0)
#define STD_SUCCESS            (1 << 0)
#define STD_ERR_INV_ARG        (1 << 1)
#define STD_ERR_INV_STATE      (1 << 2)
#define STD_ERR_OOM            (1 << 3)
#define STD_ERR_OOB            (1 << 4)
#define STD_ERR_RES_EXHAUSTED  (1 << 5)
#define STD_ERR_IO             (1 << 6)
#define STD_ERR_PARSE          (1 << 7)
#define STD_ERR_NOT_FOUND      (1 << 8)
#define STD_ERR_ALREADY_EXISTS (1 << 9)
#define STD_ERR_UNSUPPORTED    (1 << 10)
#define STD_ERR_BUSY           (1 << 11)
#define STD_ERR_TIMEOUT        (1 << 12)
#define STD_ERR_CORRUPTED      (1 << 13)
#define STD_ERR_INTERNAL       (1 << 14)

#ifdef __cplusplus
}
#endif

#endif
