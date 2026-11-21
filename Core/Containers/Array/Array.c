#include "Array.h"
#include <string.h>

struct CONT_Array {
    STD_Size  cap;
    STD_Size  len;
    STD_Size  slot_size;
    STD_Byte *pSlots;
};

STD_API STD_Bool STD_CALL CONT_ArrayIsValid(const CONT_Array *pArr) {
    return (pArr != nullptr && pArr->pSlots != nullptr && pArr->slot_size > 0 &&
            pArr->cap > 0 && pArr->cap <= CONT_ArrayMaxCap(pArr->slot_size) &&
            pArr->len <= pArr->cap);
}

/* ---- CONSTRUCTORS/DESTRUCTORS ---- */

STD_API STD_Rslt STD_CALL CONT_ArrayCreate(STD_Size slot_size, STD_Size cap,
                                           CONT_Array **ppArr) {
    if (!ppArr) {
        return STD_ERR_INV_ARG;
    }
    *ppArr = nullptr;
    if (slot_size == 0 || cap == 0 || cap > CONT_ArrayMaxCap(slot_size)) {
        return STD_ERR_INV_ARG;
    }

    CONT_Array *pArr = malloc(sizeof(CONT_Array));
    if (!pArr) {
        return STD_ERR_OOM;
    }
    pArr->pSlots = malloc(slot_size * cap);
    if (!pArr->pSlots) {
        free(pArr);
        return STD_ERR_OOM;
    }
    pArr->slot_size = slot_size;
    pArr->cap       = cap;
    pArr->len       = 0;

    return STD_SUCCESS;
}

STD_API STD_Rslt STD_CALL CONT_ArrayCreateClone(const CONT_Array *pTo_clone,
                                                CONT_Array      **ppArr) {
    if (!ppArr) {
        return STD_ERR_INV_ARG;
    }
    *ppArr = nullptr;
    if (!CONT_ArrayIsValid(pTo_clone)) {
        return STD_ERR_INV_ARG;
    }

    STD_Rslt code =
        CONT_ArrayCreate(pTo_clone->slot_size, pTo_clone->len, ppArr);
    if (code != STD_SUCCESS) {
        return code;
    }
    (*ppArr)->len = pTo_clone->len;
    memcpy((*ppArr)->pSlots, pTo_clone->pSlots,
           pTo_clone->slot_size * pTo_clone->len);

    return STD_SUCCESS;
}

STD_API STD_Rslt STD_CALL CONT_ArrayCreateFrom(STD_Size slot_size, STD_Size len,
                                               const void  *pSlot_membs,
                                               CONT_Array **ppArr) {
    if (!ppArr) {
        return STD_ERR_INV_ARG;
    }
    *ppArr = nullptr;
    if (slot_size == 0 || len == 0 || len > CONT_ArrayMaxCap(slot_size) ||
        !pSlot_membs) {
        return STD_ERR_INV_ARG;
    }

    STD_Rslt code = CONT_ArrayCreate(slot_size, len, ppArr);
    if (code != STD_SUCCESS) {
        return code;
    }
    (*ppArr)->len = len;
    memcpy((*ppArr)->pSlots, pSlot_membs, slot_size * len);

    return STD_SUCCESS;
}

STD_API STD_Rslt STD_CALL CONT_ArrayCreateFilled(STD_Size     slot_size,
                                                 STD_Size     cap,
                                                 const void  *pFill,
                                                 CONT_Array **ppArr) {
    if (!ppArr) {
        return STD_ERR_INV_ARG;
    }
    *ppArr = nullptr;
    if (slot_size == 0 || cap == 0 || cap > CONT_ArrayMaxCap(slot_size) ||
        !pFill) {
        return STD_ERR_INV_ARG;
    }

    STD_Rslt code = CONT_ArrayCreate(slot_size, cap, ppArr);
    if (code != STD_SUCCESS) {
        return code;
    }
    (*ppArr)->len = cap;
    for (STD_Size idx = 0; idx < cap; idx++) {
        memcpy((*ppArr)->pSlots + (idx * slot_size), pFill, slot_size);
    }

    return STD_SUCCESS;
}

STD_API STD_Rslt STD_CALL CONT_ArrayDestroy(CONT_Array **ppArr) {
    if (!ppArr || !(*ppArr)) {
        return STD_ERR_INV_ARG;
    }

    if ((*ppArr)->pSlots) {
        free((*ppArr)->pSlots);
    }
    free(*ppArr);
    *ppArr = nullptr;

    return STD_SUCCESS;
}

STD_API STD_Rslt STD_CALL CONT_ArrayReset(CONT_Array *pArr) {
    if (!CONT_ArrayIsValid(pArr)) {
        return STD_ERR_INV_ARG;
    }

    pArr->len = 0;

    return STD_SUCCESS;
}

/* ---- ARRAY SIZING ---- */

STD_API STD_Rslt STD_CALL CONT_ArrayChangeSize(CONT_Array *pArr,
                                               STD_Size    new_cap) {
    if (!CONT_ArrayIsValid(pArr) || new_cap == 0 ||
        new_cap > CONT_ArrayMaxCap(pArr->slot_size)) {
        return STD_ERR_INV_ARG;
    }

    if (new_cap == pArr->cap) {
        return STD_SUCCESS;
    }

    STD_Byte *pSlots = realloc(pArr->pSlots, new_cap * pArr->slot_size);
    if (!pSlots) {
        return STD_ERR_OOM;
    }
    pArr->pSlots = pSlots;
    pArr->cap    = new_cap;
    if (pArr->len > new_cap) {
        pArr->len = new_cap;
    }

    return STD_SUCCESS;
}

STD_API STD_Rslt STD_CALL CONT_ArrayGrow(CONT_Array *pArr) {
    if (!CONT_ArrayIsValid(pArr)) {
        return STD_ERR_INV_ARG;
    }

    STD_Size max_cap = CONT_ArrayMaxCap(pArr->slot_size);
    if (pArr->cap == max_cap) {
        return STD_ERR_RES_EXHAUSTED;
    }
    STD_Size new_cap = ((max_cap / 2) >= pArr->cap) ? (pArr->cap * 2) : max_cap;

    return CONT_ArrayChangeSize(pArr, new_cap);
}

STD_API STD_Rslt STD_CALL CONT_ArrayShrinkFit(CONT_Array *pArr) {
    if (!CONT_ArrayIsValid(pArr)) {
        return STD_ERR_INV_ARG;
    }

    STD_Size new_cap = (pArr->len == 0) ? CONT_ARRAY_DEFAULT_CAP : pArr->len;

    return CONT_ArrayChangeSize(pArr, new_cap);
}

/* ---- ARRAY ATTRIBUTES ---- */

STD_API STD_Size STD_CALL CONT_ArrayMaxCap(STD_Size slot_size) {
    DIAG_VerifyMsg(slot_size > 0,
                   "The given slot size must be greater than 0.");

    return STD_SIZE_MAX / slot_size;
}

STD_API STD_Size STD_CALL CONT_ArrayLen(const CONT_Array *pArr) {
    DIAG_Verify(CONT_ArrayIsValid(pArr));

    return pArr->len;
}

STD_API STD_Size STD_CALL CONT_ArrayCap(const CONT_Array *pArr) {
    DIAG_Verify(CONT_ArrayIsValid(pArr));

    return pArr->cap;
}

STD_API STD_Size STD_CALL CONT_ArraySlotSize(const CONT_Array *pArr) {
    DIAG_Verify(CONT_ArrayIsValid(pArr));

    return pArr->slot_size;
}

STD_API STD_Size STD_CALL CONT_ArrayEmptySlotCount(const CONT_Array *pArr) {
    DIAG_Verify(CONT_ArrayIsValid(pArr));

    return pArr->cap - pArr->len;
}

STD_API STD_Bool STD_CALL CONT_ArrayIsEmpty(const CONT_Array *pArr) {
    DIAG_Verify(CONT_ArrayIsValid(pArr));

    return pArr->len == 0;
}

STD_API STD_Bool STD_CALL CONT_ArrayIsFull(const CONT_Array *pArr) {
    DIAG_Verify(CONT_ArrayIsValid(pArr));

    return pArr->len == pArr->cap;
}

STD_API STD_Bool STD_CALL CONT_ArrayIsOwnedByArray(const CONT_Array *pArr,
                                                   const void       *pSlot) {
    DIAG_Verify(CONT_ArrayIsValid(pArr));
    DIAG_Verify(pSlot != nullptr);

    const STD_Byte *ptr = pSlot;

    return ptr < pArr->pSlots ||
           ptr >= pArr->pSlots + (pArr->cap * pArr->slot_size) ||
           ((ptr - pArr->pSlots) % pArr->slot_size) != 0;
}

STD_API const void *STD_CALL CONT_ArrayRaw(const CONT_Array *pArr) {
    DIAG_Verify(CONT_ArrayIsValid(pArr));

    return pArr->pSlots;
}

/* ---- TRIVIAL ---- */

STD_API STD_Bool STD_CALL CONT_ArrayCmp(const CONT_Array *pArr1,
                                        const CONT_Array *pArr2) {
    DIAG_Verify(CONT_ArrayIsValid(pArr1));
    DIAG_Verify(CONT_ArrayIsValid(pArr2));

    if (pArr1->len != pArr2->len || pArr1->slot_size != pArr2->slot_size) {
        return false;
    }

    return memcmp(pArr1->pSlots, pArr2->pSlots,
                  pArr1->len * pArr1->slot_size) == 0;
}

STD_API STD_Rslt STD_CALL CONT_ArrayForeach(const CONT_Array *pArr,
                                            STD_Rslt (*pCb)(void *,
                                                            void *pUser_data),
                                            void *pUser_data) {
    DIAG_Verify(CONT_ArrayIsValid(pArr));
    DIAG_Verify(pCb != nullptr);

    STD_Byte *pSlots = pArr->pSlots;
    for (STD_Size i = 0; i < pArr->len; i++) {
        STD_Rslt code = pCb(pSlots, pUser_data);
        if (code != STD_SUCCESS) {
            return code;
        }
        pSlots += pArr->slot_size;
    }

    return STD_SUCCESS;
}

/* ---- GETTER/SETTER ---- */

STD_API void STD_CALL CONT_ArrayGetUnchecked(const CONT_Array *pArr,
                                             STD_Size idx, void **ppDest) {

    DIAG_Assert(ppDest != nullptr);
    *ppDest = nullptr;
    CONT_ArrayIsValidAssert(pArr);
    DIAG_AssertMsg(idx < pArr->len, "The given idx is out of bounds.");

    *ppDest = pArr->pSlots + (pArr->slot_size * idx);
}

STD_API STD_Rslt STD_CALL CONT_ArrayGet(const CONT_Array *pArr, STD_Size idx,
                                        void **ppDest) {
    if (!ppDest) {
        return STD_ERR_INV_ARG;
    }
    *ppDest = nullptr;
    if (!CONT_ArrayIsValid(pArr)) {
        return STD_ERR_INV_ARG;
    }
    if (idx >= pArr->len) {
        return STD_ERR_OOB;
    }

    *ppDest = pArr->pSlots + (pArr->slot_size * idx);

    return STD_SUCCESS;
}

STD_API void STD_CALL CONT_ArraySetUnchecked(const CONT_Array *pArr,
                                             STD_Size idx, void *pData) {
    CONT_ArrayIsValidAssert(pArr);
    DIAG_AssertMsg(idx < pArr->len, "The given idx is out of bounds.");
    DIAG_Assert(pData != nullptr);

    memcpy(pArr->pSlots + (pArr->slot_size * idx), pData, pArr->slot_size);
}

STD_API STD_Rslt STD_CALL CONT_ArraySet(const CONT_Array *pArr, STD_Size idx,
                                        void *pData) {
    if (!CONT_ArrayIsValid(pArr) || !pData) {
        return STD_ERR_INV_ARG;
    }
    if (idx >= pArr->len) {
        return STD_ERR_OOB;
    }

    memcpy(pArr->pSlots + (pArr->slot_size * idx), pData, pArr->slot_size);

    return STD_SUCCESS;
}

STD_API void STD_CALL CONT_ArraySwapUnchecked(const CONT_Array *pArr,
                                              STD_Size idx1, STD_Size idx2,
                                              void *pSwap_bffr) {
    CONT_ArrayIsValidAssert(pArr);
    DIAG_AssertMsg(idx1 < pArr->len, "The given idx1 is out of bounds.");
    DIAG_AssertMsg(idx2 < pArr->len, "The given idx2 is out of bounds.");
    DIAG_Assert(pSwap_bffr != nullptr);

    STD_Byte *pElem1 = pArr->pSlots + (idx1 * pArr->slot_size);
    STD_Byte *pElem2 = pArr->pSlots + (idx2 * pArr->slot_size);
    memcpy(pSwap_bffr, pElem1, pArr->slot_size);
    memcpy(pElem1, pElem2, pArr->slot_size);
    memcpy(pElem2, pSwap_bffr, pArr->slot_size);
}

STD_API STD_Rslt STD_CALL CONT_ArraySwap(const CONT_Array *pArr, STD_Size idx1,
                                         STD_Size idx2, void *pSwap_bffr) {
    if (!CONT_ArrayIsValid(pArr) || !pSwap_bffr) {
        return STD_ERR_INV_ARG;
    }
    if (idx1 >= pArr->len || idx2 >= pArr->len) {
        return STD_ERR_OOB;
    }

    CONT_ArraySwapUnchecked(pArr, idx1, idx2, pSwap_bffr);

    return STD_SUCCESS;
}

/* ---- ADDING ELEMENTS ---- */

STD_API STD_Rslt STD_CALL CONT_ArrayPushUnchecked(CONT_Array *pArr,
                                                  const void *pData) {
    CONT_ArrayIsValidAssert(pArr);
    DIAG_Assert(pData != nullptr);

    if (pArr->len == pArr->cap) {
        STD_Rslt code = CONT_ArrayGrow(pArr);
        // The code can't be
        if (code != STD_SUCCESS) {
            return code;
        }
    }

    memcpy(pArr->pSlots + (pArr->slot_size * (pArr->len++)), pData,
           pArr->slot_size);

    return STD_SUCCESS;
}

STD_API STD_Rslt STD_CALL CONT_ArrayPush(CONT_Array *pArr, const void *pData) {
    if (!CONT_ArrayIsValid(pArr) || !pData) {
        return STD_ERR_INV_ARG;
    }

    return CONT_ArrayPushUnchecked(pArr, pData);
}

STD_API STD_Rslt STD_CALL CONT_ArrayAppendUnchecked(CONT_Array *pArr,
                                                    const void *pData_arr,
                                                    STD_Size    count) {
    CONT_ArrayIsValidAssert(pArr);
    DIAG_Assert(pData_arr != nullptr);
    DIAG_AssertMsg(count > 0, "The number of elements to append in the array "
                              "must be greater than 0.");

    STD_Rslt code = CONT_ArrayReserveUnchecked(pArr, count);
    if (code != STD_SUCCESS) {
        return code;
    }

    memcpy(pArr->pSlots + (pArr->slot_size * pArr->len), pData_arr,
           pArr->slot_size * count);
    pArr->len += count;

    return STD_SUCCESS;
}

STD_API STD_Rslt STD_CALL CONT_ArrayAppend(CONT_Array *pArr,
                                           const void *pData_arr,
                                           STD_Size    count) {
    if (!CONT_ArrayIsValid(pArr) || !pData_arr || count == 0) {
        return STD_ERR_INV_ARG;
    }

    return CONT_ArrayAppendUnchecked(pArr, pData_arr, count);
}

STD_API STD_Rslt STD_CALL CONT_ArrayExtendUnchecked(CONT_Array       *pArr1,
                                                    const CONT_Array *pArr2) {
    CONT_ArrayIsValidAssert(pArr1);
    CONT_ArrayIsValidAssert(pArr2);
    DIAG_AssertMsg(pArr1->slot_size == pArr2->slot_size,
                   "To extend two arrays, their slot sizes must match.");

    STD_Rslt code = CONT_ArrayReserveUnchecked(pArr1, pArr2->len);
    if (code != STD_SUCCESS) {
        return code;
    }

    memcpy(pArr1->pSlots + (pArr1->slot_size * pArr1->len), pArr2->pSlots,
           pArr2->slot_size * pArr2->len);
    pArr1->len += pArr2->len;

    return STD_SUCCESS;
}

STD_API STD_Rslt STD_CALL CONT_ArrayExtend(CONT_Array       *pArr1,
                                           const CONT_Array *pArr2) {
    if (!CONT_ArrayIsValid(pArr1) || !CONT_ArrayIsValid(pArr2) ||
        pArr1->slot_size != pArr2->slot_size) {
        return STD_ERR_INV_ARG;
    }

    return CONT_ArrayExtendUnchecked(pArr1, pArr2);
}

STD_API STD_Rslt STD_CALL CONT_ArrayInsertUnchecked(CONT_Array *pArr,
                                                    STD_Size    idx,
                                                    const void *pData) {
    CONT_ArrayIsValidAssert(pArr);
    DIAG_AssertMsg(idx <= pArr->len, "The given idx is out of bounds.");
    DIAG_Assert(pData != nullptr);

    if (pArr->len == pArr->cap) {
        STD_Rslt code = CONT_ArrayGrow(pArr);
        if (code != STD_SUCCESS) {
            return code;
        }
    }

    memmove(pArr->pSlots + ((idx + 1) * pArr->slot_size),
            pArr->pSlots + (idx * pArr->slot_size),
            (pArr->len - idx) * pArr->slot_size);
    memcpy(pArr->pSlots + (pArr->slot_size * idx), pData, pArr->slot_size);
    pArr->len++;

    return STD_SUCCESS;
}

STD_API STD_Rslt STD_CALL CONT_ArrayInsert(CONT_Array *pArr, STD_Size idx,
                                           const void *pData) {
    if (!CONT_ArrayIsValid(pArr) || !pData) {
        return STD_ERR_INV_ARG;
    }
    if (idx > pArr->len) {
        return STD_ERR_OOB;
    }

    return CONT_ArrayInsertUnchecked(pArr, idx, pData);
}

STD_API STD_Rslt STD_CALL CONT_ArrayInsertManyUnchecked(CONT_Array *pArr,
                                                        STD_Size    idx,
                                                        const void *pData_arr,
                                                        STD_Size    count) {
    CONT_ArrayIsValidAssert(pArr);
    DIAG_AssertMsg(idx <= pArr->len, "The given idx is out of bounds.");
    DIAG_Assert(pData_arr != nullptr);
    DIAG_AssertMsg(count > 0, "The number of elements to insert in the array "
                              "must be greater than 0.");

    STD_Rslt code = CONT_ArrayReserveUnchecked(pArr, count);
    if (code != STD_SUCCESS) {
        return code;
    }

    memmove(pArr->pSlots + ((idx + count) * pArr->slot_size),
            pArr->pSlots + (idx * pArr->slot_size),
            (pArr->len - idx) * pArr->slot_size);
    memcpy(pArr->pSlots + (pArr->slot_size * idx), pData_arr,
           pArr->slot_size * count);
    pArr->len += count;

    return STD_SUCCESS;
}

STD_API STD_Rslt STD_CALL CONT_ArrayInsertMany(CONT_Array *pArr, STD_Size idx,
                                               const void *pData_arr,
                                               STD_Size    count) {
    if (!CONT_ArrayIsValid(pArr) || !pData_arr || count == 0) {
        return STD_ERR_INV_ARG;
    }
    if (idx > pArr->len) {
        return STD_ERR_OOB;
    }

    return CONT_ArrayInsertManyUnchecked(pArr, idx, pData_arr, count);
}

STD_API STD_Rslt STD_CALL CONT_ArrayEmpalceUnchecked(CONT_Array *pArr,
                                                     void      **ppSlot) {
    DIAG_Assert(ppSlot != nullptr);
    *ppSlot = nullptr;
    CONT_ArrayIsValidAssert(pArr);

    if (pArr->len == pArr->cap) {
        STD_Rslt code = CONT_ArrayGrow(pArr);
        if (code != STD_SUCCESS) {
            return code;
        }
    }
    *ppSlot = pArr->pSlots + (pArr->slot_size * (pArr->len++));

    return STD_SUCCESS;
}

STD_API STD_Rslt STD_CALL CONT_ArrayEmpalce(CONT_Array *pArr, void **ppSlot) {
    if (!ppSlot) {
        return STD_ERR_INV_ARG;
    }
    *ppSlot = nullptr;
    if (!CONT_ArrayIsValid(pArr)) {
        return STD_ERR_INV_ARG;
    }

    return CONT_ArrayEmpalceUnchecked(pArr, ppSlot);
}

STD_API STD_Rslt STD_CALL CONT_ArrayReserveUnchecked(CONT_Array *pArr,
                                                     STD_Size    count) {
    CONT_ArrayIsValidAssert(pArr);
    DIAG_AssertMsg(count > 0, "The number of elements to reserve in the array "
                              "must be greater than 0.");

    if (pArr->cap - pArr->len >= count) {
        return STD_SUCCESS;
    }
    if (CONT_ArrayMaxCap(pArr->slot_size) - pArr->cap < count) {
        return STD_ERR_RES_EXHAUSTED;
    }

    return CONT_ArrayChangeSize(pArr, pArr->len + count);
}

STD_API STD_Rslt STD_CALL CONT_ArrayReserve(CONT_Array *pArr, STD_Size count) {
    if (!CONT_ArrayIsValid(pArr) || count == 0) {
        return STD_ERR_INV_ARG;
    }

    return CONT_ArrayReserveUnchecked(pArr, count);
}

/* ---- REMOVING ELEMENTS ---- */

STD_API STD_Rslt STD_CALL CONT_ArrayPopUnchecked(CONT_Array *pArr) {
    CONT_ArrayIsValidAssert(pArr);

    if (pArr->len == 0) {
        return STD_ERR_RES_EXHAUSTED;
    }
    pArr->len--;

    return STD_SUCCESS;
}

STD_API STD_Rslt STD_CALL CONT_ArrayPop(CONT_Array *pArr) {
    if (!CONT_ArrayIsValid(pArr)) {
        return STD_ERR_INV_ARG;
    }

    return CONT_ArrayPopUnchecked(pArr);
}

STD_API void STD_CALL CONT_ArrayRemoveUnchecked(CONT_Array *pArr,
                                                STD_Size    idx) {
    CONT_ArrayIsValidAssert(pArr);
    DIAG_AssertMsg(idx < pArr->len, "The given idx is out of bounds.");

    memmove(pArr->pSlots + (idx * pArr->slot_size),
            pArr->pSlots + ((idx + 1) * pArr->slot_size),
            (pArr->len - idx - 1) * pArr->slot_size);
    pArr->len--;
}

STD_API STD_Rslt STD_CALL CONT_ArrayRemove(CONT_Array *pArr, STD_Size idx) {
    if (!CONT_ArrayIsValid(pArr)) {
        return STD_ERR_INV_ARG;
    }
    if (idx >= pArr->len) {
        return STD_ERR_OOB;
    }

    CONT_ArrayRemoveUnchecked(pArr, idx);

    return STD_SUCCESS;
}

STD_API void STD_CALL CONT_ArrayRemoveManyUnchecked(CONT_Array *pArr,
                                                    STD_Size    idx,
                                                    STD_Size    count) {
    CONT_ArrayIsValidAssert(pArr);
    DIAG_AssertMsg(idx < pArr->len, "The given idx is out of bounds.");
    DIAG_AssertMsg(count > 0, "The number of elements to remove in the array "
                              "must be greater than 0.");
    DIAG_AssertMsg(
        pArr->len - idx >= count,
        "The number of elements to remove exceed what is available.");

    memmove(pArr->pSlots + (idx * pArr->slot_size),
            pArr->pSlots + ((idx + count) * pArr->slot_size),
            (pArr->len - idx - count) * pArr->slot_size);
    pArr->len -= count;
}

STD_API STD_Rslt STD_CALL CONT_ArrayRemoveMany(CONT_Array *pArr, STD_Size idx,
                                               STD_Size count) {
    if (!CONT_ArrayIsValid(pArr) || count == 0 || count > pArr->len - idx) {
        return STD_ERR_INV_ARG;
    }
    if (idx >= pArr->len) {
        return STD_ERR_OOB;
    }

    CONT_ArrayRemoveManyUnchecked(pArr, idx, count);

    return STD_SUCCESS;
}
