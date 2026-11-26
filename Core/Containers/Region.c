#include "Region.h"
#include <string.h>

struct CONT_Region {
    STD_Size  size;
    STD_Byte *pMem;
};

STD_API STD_Bool STD_CALL CONT_RegionIsValid(const CONT_Region *pRegion) {
    return (pRegion != nullptr && pRegion->pMem != nullptr &&
            pRegion->size > 0);
}

/* ---- CONSTRUCTORS/DESTRUCTORS ---- */

STD_API STD_Rslt STD_CALL CONT_RegionCreate(STD_Size      size,
                                            CONT_Region **ppRegion) {
    if (!ppRegion) {
        return STD_ERR_INV_ARG;
    }
    *ppRegion = nullptr;
    if (size == 0) {
        return STD_ERR_INV_ARG;
    }

    (*ppRegion) = malloc(sizeof(CONT_Region));
    if (*ppRegion) {
        return STD_ERR_OOM;
    }
    (*ppRegion)->pMem = calloc(1, size);
    if (!(*ppRegion)->pMem) {
        free(*ppRegion);
        return STD_ERR_OOM;
    }
    (*ppRegion)->size = size;

    return STD_SUCCESS;
}

STD_API STD_Rslt STD_CALL CONT_RegionCreateClone(const CONT_Region *pTo_clone,
                                                 CONT_Region      **ppRegion) {
    if (!ppRegion) {
        return STD_ERR_INV_ARG;
    }
    *ppRegion = nullptr;
    if (!CONT_RegionIsValid(pTo_clone)) {
        return STD_ERR_INV_ARG;
    }

    STD_Rslt code = CONT_RegionCreate(pTo_clone->size, ppRegion);
    if (code != STD_SUCCESS) {
        return code;
    }
    memcpy((*ppRegion)->pMem, pTo_clone->pMem, pTo_clone->size);

    return STD_SUCCESS;
}

STD_API STD_Rslt STD_CALL CONT_RegionDestroy(CONT_Region **ppRegion) {
    if (!ppRegion || !(*ppRegion)) {
        return STD_ERR_INV_ARG;
    }

    if ((*ppRegion)->pMem) {
        free((*ppRegion)->pMem);
    }
    free(*ppRegion);
    *ppRegion = nullptr;

    return STD_SUCCESS;
}

STD_API STD_Rslt STD_CALL CONT_RegionReset(CONT_Region *pRegion) {
    if (!CONT_RegionIsValid(pRegion)) {
        return STD_ERR_INV_ARG;
    }

    memset(pRegion->pMem, 0, pRegion->size);

    return STD_SUCCESS;
}

/* ---- REGION SIZING ---- */

STD_API STD_Rslt STD_CALL CONT_RegionChangeSize(CONT_Region *pRegion,
                                                STD_Size     new_size) {
    if (!CONT_RegionIsValid(pRegion) || new_size == 0) {
        return STD_ERR_INV_ARG;
    }

    if (pRegion->size == new_size) {
        return STD_SUCCESS;
    }

    STD_Byte *pMem = realloc(pRegion->pMem, new_size);
    if (!pMem) {
        return STD_ERR_OOM;
    }
    if (new_size > pRegion->size) {
        memset(pMem + pRegion->size, 0, new_size - pRegion->size);
    }
    pRegion->pMem = pMem;
    pRegion->size = new_size;

    return STD_SUCCESS;
}

STD_API STD_Rslt STD_CALL CONT_RegionReserveMemory(CONT_Region *pRegion,
                                                   STD_Size     ofs,
                                                   STD_Size     size) {
    if (!CONT_RegionIsValid(pRegion) || size == 0) {
        return STD_ERR_INV_ARG;
    }
    if (ofs > pRegion->size) {
        return STD_ERR_OOB;
    }
    if (CONT_REGION_MAX_SIZE - ofs < size) {
        return STD_ERR_RES_EXHAUSTED;
    }

    if (pRegion->size - ofs < size) {
        return CONT_RegionChangeSize(pRegion, size + ofs);
    }

    return STD_SUCCESS;
}

/* ---- REGION ATTRIBUTES ---- */

STD_API STD_Size STD_CALL CONT_RegionSize(const CONT_Region *pRegion) {
    DIAG_Verify(CONT_RegionIsValid(pRegion));

    return pRegion->size;
}

STD_API const void *STD_CALL CONT_RegionRaw(const CONT_Region *pRegion) {
    DIAG_Verify(CONT_RegionIsValid(pRegion));

    return pRegion->pMem;
}

STD_API STD_Bool STD_CALL CONT_RegionIsOwnedByRegion(const CONT_Region *pRegion,
                                                     const void        *pMem) {
    DIAG_Verify(CONT_RegionIsValid(pRegion));
    DIAG_Verify(pMem != nullptr);

    return pRegion->pMem <= (STD_Byte *)pMem &&
           pRegion->pMem + pRegion->size > (STD_Byte *)pMem;
}

/* ---- TRIVIAL ---- */

STD_API STD_Bool STD_CALL CONT_RegionCmp(const CONT_Region *pRegion1,
                                         const CONT_Region *pRegion2) {
    if (pRegion1->size != pRegion2->size) {
        return false;
    }

    return memcmp(pRegion1->pMem, pRegion2->pMem, pRegion1->size) == 0;
}

/* ---- MODIFICATION FUNCTIONS ---- */

STD_API STD_Rslt STD_CALL CONT_RegionGet(const CONT_Region *pRegion,
                                         STD_Size ofs, void **ppDest) {
    if (!ppDest) {
        return STD_ERR_INV_ARG;
    }
    *ppDest = nullptr;
    if (CONT_RegionIsValid(pRegion)) {
        return STD_ERR_INV_ARG;
    }
    if (ofs >= pRegion->size) {
        return STD_ERR_OOB;
    }

    *ppDest = pRegion->pMem + ofs;

    return STD_SUCCESS;
}

STD_API STD_Rslt STD_CALL CONT_RegionSet(const CONT_Region *pRegion,
                                         STD_Size ofs, STD_Size size,
                                         void *pData) {
    if (!CONT_RegionIsValid(pRegion) || !pData || size == 0) {
        return STD_ERR_INV_ARG;
    }
    if (ofs > pRegion->size) {
        return STD_ERR_OOB;
    }
    if (pRegion->size - ofs < size) {
        return STD_ERR_RES_EXHAUSTED;
    }

    memcpy(pRegion->pMem + ofs, pData, size);

    return STD_SUCCESS;
}

STD_API STD_Rslt STD_CALL CONT_RegionCopyUnchecked(const CONT_Region *pRegion1,
                                                   CONT_Region       *pRegion2,
                                                   STD_Size ofs1, STD_Size ofs2,
                                                   STD_Size size) {
    CONT_RegionIsValidAssert(pRegion1);
    CONT_RegionIsValidAssert(pRegion2);
    DIAG_AssertMsg(ofs1 <= pRegion1->size, "The given ofs1 is out of bound");
    DIAG_AssertMsg(ofs2 <= pRegion2->size, "The given ofs2 is out of bound");
    DIAG_AssertMsg(size > 0,
                   "The size of the region to copy must be greater than 0.");

    if (pRegion2->size - ofs2 < size || pRegion1->size - ofs1 < size) {
        return STD_ERR_RES_EXHAUSTED;
    }

    memcpy(pRegion2->pMem + ofs2, pRegion1->pMem + ofs1, size);

    return STD_SUCCESS;
}

STD_API STD_Rslt STD_CALL CONT_RegionCopy(const CONT_Region *pRegion1,
                                          CONT_Region *pRegion2, STD_Size ofs1,
                                          STD_Size ofs2, STD_Size size) {
    if (!CONT_RegionIsValid(pRegion1) || !CONT_RegionIsValid(pRegion2) ||
        size == 0) {
        return STD_ERR_INV_ARG;
    }
    if (ofs1 > pRegion1->size || ofs2 > pRegion2->size) {
        return STD_ERR_OOB;
    }
    if (pRegion2->size - ofs2 < size || pRegion1->size - ofs1 < size) {
        return STD_ERR_RES_EXHAUSTED;
    }

    memcpy(pRegion2->pMem + ofs2, pRegion1->pMem + ofs1, size);

    return STD_SUCCESS;
}

STD_API STD_Rslt STD_CALL CONT_RegionFillUnchecked(const CONT_Region *pRegion,
                                                   STD_Size ofs, STD_Size size,
                                                   STD_Byte byte) {
    CONT_RegionIsValidAssert(pRegion);
    DIAG_AssertMsg(ofs <= pRegion->size, "The given offset is out of bound");
    DIAG_AssertMsg(size > 0,
                   "The size of the region to fill must be greater than 0.");

    if (pRegion->size - ofs < size) {
        return STD_ERR_RES_EXHAUSTED;
    }

    memset(pRegion->pMem + ofs, byte, size);

    return STD_SUCCESS;
}
STD_API STD_Rslt STD_CALL CONT_RegionFill(const CONT_Region *pRegion,
                                          STD_Size ofs, STD_Size size,
                                          STD_Byte byte) {
    if (!CONT_RegionIsValid(pRegion) || size == 0) {
        return STD_ERR_INV_ARG;
    }
    if (ofs > pRegion->size) {
        return STD_ERR_OOB;
    }
    if (pRegion->size - ofs < size) {
        return STD_ERR_RES_EXHAUSTED;
    }

    memset(pRegion->pMem + ofs, byte, size);

    return STD_SUCCESS;
}

STD_API STD_Rslt STD_CALL CONT_RegionSwapUnchecked(const CONT_Region *pRegion,
                                                   STD_Size ofs1, STD_Size ofs2,
                                                   STD_Size size,
                                                   void    *pSwap_bffr) {
    CONT_RegionIsValidAssert(pRegion);
    DIAG_AssertMsg(ofs1 <= pRegion->size, "The given ofs1 is out of bound");
    DIAG_AssertMsg(ofs2 <= pRegion->size, "The given ofs2 is out of bound");
    DIAG_AssertMsg(size > 0,
                   "The size of the region to swap must be greater than 0.");
    DIAG_Assert(pSwap_bffr != nullptr);

    if (pRegion->size - ofs2 < size || pRegion->size - ofs1 < size) {
        return STD_ERR_RES_EXHAUSTED;
    }

    STD_Byte *pR1 = pRegion->pMem + ofs1;
    STD_Byte *pR2 = pRegion->pMem + ofs2;

    memcpy(pSwap_bffr, pR1, size);
    memcpy(pR1, pR2, size);
    memcpy(pR2, pSwap_bffr, size);

    return STD_SUCCESS;
}

STD_API STD_Rslt STD_CALL CONT_RegionSwap(const CONT_Region *pRegion,
                                          STD_Size ofs1, STD_Size ofs2,
                                          STD_Size size, void *pSwap_bffr) {
    if (!CONT_RegionIsValid(pRegion) || !pSwap_bffr || size == 0) {
        return STD_ERR_INV_ARG;
    }
    if (ofs1 > pRegion->size || ofs2 > pRegion->size) {
        return STD_ERR_OOB;
    }

    // The STD_ERR_RES_EXHAUSTED condition is handled in the unchecked func.
    return CONT_RegionSwapUnchecked(pRegion, ofs1, ofs2, size, pSwap_bffr);
}

STD_API STD_Rslt STD_CALL
CONT_RegionExtendUnchecked(CONT_Region *pRegion1, const CONT_Region *pRegion2) {
    CONT_RegionIsValidAssert(pRegion1);
    CONT_RegionIsValidAssert(pRegion2);

    if (CONT_REGION_MAX_SIZE - pRegion1->size < pRegion2->size) {
        return STD_ERR_RES_EXHAUSTED;
    }
    STD_Size old_size = pRegion1->size;
    STD_Rslt code =
        CONT_RegionChangeSize(pRegion1, pRegion1->size + pRegion2->size);
    if (code != STD_SUCCESS) {
        return code;
    }
    memcpy(pRegion1->pMem + old_size, pRegion2->pMem, pRegion2->size);

    return STD_SUCCESS;
}

STD_API STD_Rslt STD_CALL CONT_RegionExtend(CONT_Region       *pRegion1,
                                            const CONT_Region *pRegion2) {
    if (!CONT_RegionIsValid(pRegion1) || !CONT_RegionIsValid(pRegion2)) {
        return STD_ERR_INV_ARG;
    }

    return CONT_RegionExtendUnchecked(pRegion1, pRegion2);
}
