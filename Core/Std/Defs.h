#ifndef DEF_STD_DEFS_
#define DEF_STD_DEFS_

#ifdef __cplusplus
extern "C" {
#endif

#include "Types.h"
#include <limits.h>

#define STD_BITS_IN_TYPE(T) (sizeof(T) * CHAR_BIT)

#define STD_INVALID_SIZE ((STD_Size)(~0))
#define STD_INVALID_IDX  ((STD_Size)(~0))
#define STD_INVALID_POS  ((STD_Size)0)

#define STD_IDX_TO_POS(idx) ((idx) + 1)
#define STD_POS_TO_IDX(pos) ((pos) - 1)

#define STD_ALIGNUP(x, align) (((x) + ((align) - 1)) & ~((align) - 1))
#define STD_ALIGN(x)          alignas(x)

#ifdef __cplusplus
}
#endif

#endif
