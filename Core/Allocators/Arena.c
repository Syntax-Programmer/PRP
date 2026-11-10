#include "Arena.h"
#include <string.h>

struct ALOC_Arena {
    STD_Size size;
    STD_Size ofs;
    STD_Byte pBlock[];
};

STD_API STD_Bool STD_CALL ALOC_ArenaIsValid(const ALOC_Arena *pArena) {
    return (pArena != nullptr && pArena->size > 0 &&
            pArena->size <= ALOC_ArenaMaxBlockSize() &&
            pArena->ofs <= pArena->size);
}

STD_API STD_Rslt STD_CALL ALOC_ArenaCreate(STD_Size     size,
                                           ALOC_Arena **ppArena) {
    if (!ppArena) {
        return STD_ERR_INV_ARG;
    }
    *ppArena = nullptr;
    if (size == 0 || size > ALOC_ArenaMaxBlockSize()) {
        return STD_ERR_INV_ARG;
    }

    *ppArena = malloc(sizeof(ALOC_Arena) * size);
    if (!(*ppArena)) {
        return STD_ERR_OOM;
    }
    (*ppArena)->size = size;
    (*ppArena)->ofs  = 0;

    return STD_SUCCESS;
};

STD_API STD_Rslt STD_CALL ALOC_ArenaDestroy(ALOC_Arena **ppArena) {
    if (!ppArena || !(*ppArena)) {
        return STD_ERR_INV_ARG;
    }

    free(*ppArena);
    *ppArena = nullptr;

    return STD_SUCCESS;
}

STD_API STD_Rslt STD_CALL ALOC_ArenaReset(ALOC_Arena *pArena) {
    if (!ALOC_ArenaIsValid(pArena)) {
        return STD_ERR_INV_ARG;
    }

    pArena->ofs = 0;
}

STD_API STD_Rslt STD_CALL ALOC_ArenaAlloc(ALOC_Arena *pArena, STD_Size size,
                                          STD_Bool zeroed, void **ppDest) {
    if (!ppDest) {
        return STD_ERR_INV_ARG;
    }
    *ppDest = nullptr;
    if (!ALOC_ArenaIsValid(pArena) || size == 0) {
        return STD_ERR_INV_ARG;
    }

    if (size > pArena->size - pArena->ofs) {
        return STD_ERR_RES_EXHAUSTED;
    }
    *ppDest = pArena->pBlock + pArena->ofs;
    if (zeroed) {
        memset(*ppDest, 0, size);
    }
    pArena->ofs += size;

    return STD_SUCCESS;
}

STD_API STD_Rslt STD_CALL ALOC_ArenaAllocMany(ALOC_Arena *pArena, STD_Size size,
                                              STD_Bool zeroed, STD_Size count,
                                              void **ppDest) {
    if (!ppDest) {
        return STD_ERR_INV_ARG;
    }
    *ppDest = nullptr;
    if (!ALOC_ArenaIsValid(pArena) || size == 0 || count == 0) {
        return STD_ERR_INV_ARG;
    }

    STD_Size total_size = size * count;
    if (total_size > pArena->size - pArena->ofs) {
        return STD_ERR_RES_EXHAUSTED;
    }
    *ppDest = pArena->pBlock + pArena->ofs;
    if (zeroed) {
        memset(*ppDest, 0, total_size);
    }
    pArena->ofs += total_size;

    return STD_SUCCESS;
}

STD_API STD_Rslt STD_CALL ALOC_ArenaAllocAligned(ALOC_Arena *pArena,
                                                 STD_Size size, STD_Size align,
                                                 STD_Bool zeroed,
                                                 void   **ppDest) {
    if (!ppDest) {
        return STD_ERR_INV_ARG;
    }
    *ppDest = nullptr;
    if (!ALOC_ArenaIsValid(pArena) || size == 0 || align == 0 ||
        (align & (align - 1)) != 0) {
        return STD_ERR_INV_ARG;
    }

    STD_UintptrT base      = (STD_UintptrT)pArena->pBlock;
    STD_UintptrT current   = base + pArena->ofs;
    STD_Size     padding   = (STD_Size)((-(STD_UintptrT)current) & (align - 1));
    STD_Size     available = pArena->size - pArena->ofs;
    if (padding > available || size > available - padding) {
        return STD_ERR_RES_EXHAUSTED;
    }
    *ppDest = pArena->pBlock + pArena->ofs + padding;
    if (zeroed) {
        memset(*ppDest, 0, size);
    }
    pArena->ofs += padding + size;

    return STD_SUCCESS;
}

STD_API STD_Rslt STD_CALL
ALOC_ArenaAllocManyAligned(ALOC_Arena *pArena, STD_Size size, STD_Size align,
                           STD_Bool zeroed, STD_Size count, void **ppDest) {
    if (!ppDest) {
        return STD_ERR_INV_ARG;
    }
    *ppDest = nullptr;
    if (!ALOC_ArenaIsValid(pArena) || size == 0 || count == 0 || align == 0 ||
        (align & (align - 1)) != 0) {
        return STD_ERR_INV_ARG;
    }

    STD_UintptrT base      = (STD_UintptrT)pArena->pBlock;
    STD_UintptrT current   = base + pArena->ofs;
    STD_Size     padding   = (STD_Size)((-(STD_UintptrT)current) & (align - 1));
    STD_Size     available = pArena->size - pArena->ofs;
    STD_Size     total_size = size * count;
    if (padding > available || total_size > available - padding) {
        return STD_ERR_RES_EXHAUSTED;
    }
    *ppDest = pArena->pBlock + pArena->ofs + padding;
    if (zeroed) {
        memset(*ppDest, 0, total_size);
    }
    pArena->ofs += padding + total_size;

    return STD_SUCCESS;
}

STD_API STD_Size STD_CALL ALOC_ArenaMaxBlockSize(void) {
    return STD_SIZE_MAX - sizeof(ALOC_Arena);
}

STD_API STD_Size STD_CALL ALOC_ArenaGetFreeSpace(const ALOC_Arena *pArena) {
    DIAG_Verify(pArena != nullptr);

    return pArena->size - pArena->ofs;
}

STD_API STD_Bool STD_CALL ALOC_ArenaIsEmpty(const ALOC_Arena *pArena) {
    DIAG_Verify(pArena != nullptr);

    return pArena->ofs == 0;
}

STD_API STD_Bool STD_CALL ALOC_ArenaIsFull(const ALOC_Arena *pArena) {
    DIAG_Verify(pArena != nullptr);

    return pArena->size == pArena->ofs;
}

STD_API STD_Bool STD_CALL ALOC_ArenaIsOwnedByArena(const ALOC_Arena *pArena,
                                                   const void       *pBlock) {
    DIAG_Verify(pArena != nullptr);
    DIAG_Verify(pBlock != nullptr);

    return (STD_Byte *)pBlock >= pArena->pBlock &&
           (STD_Byte *)pBlock < pArena->pBlock + pArena->size;
}
