#include "Pool.h"
#include <string.h>

#ifdef STD_COMPILE_MODE_DEBUG
    #warning The pool implementation isn't protected against double free of the same slot. Future developer and user be weary of that.
#endif

struct ALOC_Pool {
    STD_Size  cap;
    STD_Size  free_count;
    STD_Size  slot_size;
    STD_Byte *pFree_list;
    STD_Byte  pSlots[];
};

#define PaddedSlotSize(slot_size)                                              \
    ((slot_size) > sizeof(STD_Byte *) ? slot_size : sizeof(STD_Byte *))

STD_API STD_Bool STD_CALL ALOC_PoolIsValid(const ALOC_Pool *pPool) {
    return (pPool != nullptr && pPool->slot_size > 0 && pPool->cap > 0 &&
            pPool->cap <= ALOC_PoolMaxCap(pPool->slot_size) &&
            pPool->free_count <= pPool->cap && pPool->pFree_list != nullptr);
}

#define MakeFreeSlotFreeList(pPool)                                            \
    do {                                                                       \
        STD_Byte *pCurr     = (pPool)->pSlots;                                 \
        (pPool)->pFree_list = pCurr;                                           \
        for (STD_Size i = 0; i < (pPool)->cap - 1; i++) {                      \
            *((STD_Byte **)pCurr)  = pCurr + (pPool)->slot_size;               \
            pCurr                 += (pPool)->slot_size;                       \
        }                                                                      \
        *((STD_Byte **)pCurr) = nullptr;                                       \
    } while (0)

STD_API STD_Rslt STD_CALL ALOC_PoolCreate(STD_Size slot_size, STD_Size cap,
                                          ALOC_Pool **ppPool) {
    if (!ppPool) {
        return STD_ERR_INV_ARG;
    }
    *ppPool = nullptr;
    if (slot_size == 0 || cap == 0 || cap > ALOC_PoolMaxCap(slot_size)) {
        return STD_ERR_INV_ARG;
    }

    slot_size = PaddedSlotSize(slot_size);
    *ppPool   = malloc(sizeof(ALOC_Pool) + (slot_size * cap));
    if (!(*ppPool)) {
        return STD_ERR_OOM;
    }
    (*ppPool)->cap        = cap;
    (*ppPool)->slot_size  = slot_size;
    (*ppPool)->free_count = cap;
    MakeFreeSlotFreeList(*(ppPool));

    return STD_SUCCESS;
}

STD_API STD_Rslt STD_CALL ALOC_PoolCreateAligned(STD_Size slot_size,
                                                 STD_Size cap, STD_Size align,
                                                 ALOC_Pool **ppPool) {
    if (!ppPool) {
        return STD_ERR_INV_ARG;
    }
    *ppPool = nullptr;
    if (slot_size == 0 || cap == 0 || cap > ALOC_PoolMaxCap(slot_size) ||
        align == 0 || (align & (align - 1)) != 0) {
        return STD_ERR_INV_ARG;
    }

    STD_Size effective_align =
        (alignof(ALOC_Pool) > align) ? alignof(ALOC_Pool) : align;
    slot_size           = PaddedSlotSize(slot_size);
    STD_Size alloc_size = sizeof(ALOC_Pool) + (slot_size * cap);
    STD_Size padding =
        (effective_align - (alloc_size % effective_align)) % effective_align;
    if (STD_SIZE_MAX - alloc_size < padding) {
        return STD_ERR_OOM;
    }

    *ppPool = aligned_alloc(effective_align, alloc_size + padding);
    if (!(*ppPool)) {
        return STD_ERR_OOM;
    }
    (*ppPool)->cap        = cap;
    (*ppPool)->slot_size  = slot_size;
    (*ppPool)->free_count = cap;
    MakeFreeSlotFreeList(*(ppPool));

    return STD_SUCCESS;
}

STD_API STD_Rslt STD_CALL ALOC_PoolDestroy(ALOC_Pool **ppPool) {
    if (!ppPool || !(*ppPool)) {
        return STD_ERR_INV_ARG;
    }

    free(*ppPool);
    *ppPool = nullptr;

    return STD_SUCCESS;
}

STD_API STD_Rslt STD_CALL ALOC_PoolReset(ALOC_Pool *pPool) {
    if (!ALOC_PoolIsValid(pPool)) {
        return STD_ERR_INV_ARG;
    }

    MakeFreeSlotFreeList(pPool);
    pPool->free_count = pPool->cap;

    return STD_SUCCESS;
}

STD_API STD_Rslt STD_CALL ALOC_PoolAlloc(ALOC_Pool *pPool, STD_Bool zeroed,
                                         void **ppDest) {
    if (!ppDest) {
        return STD_ERR_INV_ARG;
    }
    *ppDest = nullptr;
    if (!ALOC_PoolIsValid(pPool)) {
        return STD_ERR_INV_ARG;
    }

    if (!pPool->pFree_list) {
        return STD_ERR_RES_EXHAUSTED;
    }
    void *pAllocation = pPool->pFree_list;
    pPool->pFree_list = *((STD_Byte **)pPool->pFree_list);
    *ppDest           = pAllocation;
    if (zeroed) {
        memset(*ppDest, 0, pPool->slot_size);
    }
    pPool->free_count++;

    return STD_SUCCESS;
}

STD_API STD_Rslt STD_CALL ALOC_PoolAllocMany(ALOC_Pool *pPool, STD_Size count,
                                             STD_Bool zeroed,
                                             void   **ppDest_arr) {
    if (!ppDest_arr) {
        return STD_ERR_INV_ARG;
    }
    *ppDest_arr = nullptr;
    if (!ALOC_PoolIsValid(pPool) || count == 0) {
        return STD_ERR_INV_ARG;
    }

    if (count > pPool->free_count) {
        return STD_ERR_RES_EXHAUSTED;
    }
    for (STD_Size idx = 0; idx < count; idx++) {
        void *pAllocation = pPool->pFree_list;
        pPool->pFree_list = *((STD_Byte **)pPool->pFree_list);
        ppDest_arr[idx]   = pAllocation;
        if (zeroed) {
            memset(ppDest_arr[idx], 0, pPool->slot_size);
        }
        pPool->free_count++;
    }

    return STD_SUCCESS;
}

STD_API STD_Rslt STD_CALL ALOC_PoolFree(ALOC_Pool *pPool, void *pSlot) {
    if (!ALOC_PoolIsValid(pPool) || !pSlot) {
        return STD_ERR_INV_ARG;
    }
    STD_Byte *ptr = pSlot;
    if (ptr < pPool->pSlots ||
        ptr >= pPool->pSlots + (pPool->cap * pPool->slot_size) ||
        ((ptr - pPool->pSlots) % pPool->slot_size) != 0) {
        return STD_ERR_INV_ARG;
    }

    *((STD_Byte **)pSlot) = pPool->pFree_list;
    pPool->pFree_list     = pSlot;
    pPool->free_count--;

    return STD_SUCCESS;
}

STD_API STD_Size STD_CALL ALOC_PoolMaxCap(STD_Size slot_size) {
    DIAG_VerifyMsg(slot_size > 0,
                   "The given slot size must be greater than 0.");

    return (STD_SIZE_MAX - sizeof(ALOC_Pool)) / PaddedSlotSize(slot_size);
}

STD_API STD_Size STD_CALL ALOC_PoolGetSlotSize(const ALOC_Pool *pPool) {
    DIAG_Verify(ALOC_PoolIsValid(pPool));

    return pPool->slot_size;
}

STD_API STD_Size STD_CALL ALOC_PoolGetCap(const ALOC_Pool *pPool) {
    DIAG_Verify(ALOC_PoolIsValid(pPool));

    return pPool->cap;
}

STD_API STD_Size STD_CALL ALOC_PoolGetFreeSlotCount(const ALOC_Pool *pPool) {
    DIAG_Verify(ALOC_PoolIsValid(pPool));

    return pPool->free_count;
}

STD_API STD_Size STD_CALL
ALOC_PoolGetOccupiedSlotCount(const ALOC_Pool *pPool) {
    DIAG_Verify(ALOC_PoolIsValid(pPool));

    return pPool->cap - pPool->free_count;
}

STD_API STD_Bool STD_CALL ALOC_PoolIsEmpty(const ALOC_Pool *pPool) {
    DIAG_Verify(ALOC_PoolIsValid(pPool));

    return pPool->free_count == pPool->cap;
}

STD_API STD_Bool STD_CALL ALOC_PoolIsFull(const ALOC_Pool *pPool) {
    DIAG_Verify(ALOC_PoolIsValid(pPool));

    return pPool->free_count == 0;
}

STD_API STD_Bool STD_CALL ALOC_PoolIsOwnedByPool(const ALOC_Pool *pPool,
                                                 const void      *pSlot) {
    DIAG_Verify(ALOC_PoolIsValid(pPool));
    DIAG_Verify(pSlot != nullptr);

    const STD_Byte *ptr = pSlot;

    return ptr >= pPool->pSlots &&
           ptr < pPool->pSlots + (pPool->cap * pPool->slot_size) &&
           ((ptr - pPool->pSlots) % pPool->slot_size);
}
