#include "Bitmap.h"
#include <stdbit.h>
#include <string.h>

struct CONT_Bitmap {
    STD_Size         set_count;
    STD_Size         first_set_idx;
    STD_Size         bit_cap;
    STD_Size         word_cap;
    CONT_BitmapWord *pWords;
};

STD_API STD_Bool STD_CALL CONT_BitmapIsValid(const CONT_Bitmap *pBmp) {
    return (pBmp != nullptr && pBmp->pWords != nullptr && pBmp->bit_cap > 0 &&
            pBmp->bit_cap <= CONT_BITMAP_MAX_BIT_CAP && pBmp->word_cap > 0 &&
            CONT_BitmapWordIdx(pBmp->bit_cap) == pBmp->word_cap - 1 &&
            pBmp->set_count < pBmp->bit_cap &&
            pBmp->first_set_idx < pBmp->bit_cap);
}

/* ---- CONSTRUCTORS/DESTRUCTORS ---- */

STD_API STD_Rslt STD_CALL CONT_BitmapCreate(STD_Size      bit_cap,
                                            CONT_Bitmap **ppBmp) {
    if (!ppBmp) {
        return STD_ERR_INV_ARG;
    }
    *ppBmp = nullptr;
    if (bit_cap == 0) {
        return STD_ERR_INV_ARG;
    }

    *ppBmp = malloc(sizeof(CONT_Bitmap));
    if (!(*ppBmp)) {
        return STD_ERR_OOM;
    }
    STD_Size word_cap = CONT_BitmapWordIdx(bit_cap) + 1;
    (*ppBmp)->pWords  = calloc(1, sizeof(CONT_BitmapWord) * word_cap);
    if (!(*ppBmp)->pWords) {
        free(*ppBmp);
        *ppBmp = nullptr;
        return STD_ERR_OOM;
    }
    (*ppBmp)->word_cap      = word_cap;
    (*ppBmp)->bit_cap       = bit_cap;
    (*ppBmp)->set_count     = 0;
    (*ppBmp)->first_set_idx = STD_INVALID_IDX;

    return STD_SUCCESS;
}

STD_API STD_Rslt STD_CALL CONT_BitmapCreateClone(const CONT_Bitmap *pTo_clone,
                                                 CONT_Bitmap      **ppBmp) {
    if (ppBmp) {
        return STD_ERR_INV_ARG;
    }
    *ppBmp = nullptr;
    if (!CONT_BitmapIsValid(pTo_clone)) {
        return STD_ERR_INV_ARG;
    }

    STD_Rslt code = CONT_BitmapCreate(pTo_clone->bit_cap, ppBmp);
    if (code != STD_SUCCESS) {
        return code;
    }
    (*ppBmp)->set_count     = pTo_clone->set_count;
    (*ppBmp)->first_set_idx = pTo_clone->first_set_idx;
    memcpy((*ppBmp)->pWords, pTo_clone->pWords,
           sizeof(CONT_BitmapWord) * pTo_clone->word_cap);

    return STD_SUCCESS;
}

STD_API STD_Rslt STD_CALL CONT_BitmapDestroy(CONT_Bitmap **ppBmp) {
    if (!ppBmp || !(*ppBmp)) {
        return STD_ERR_INV_ARG;
    }

    if ((*ppBmp)->pWords) {
        free((*ppBmp)->pWords);
    }
    free(*ppBmp);
    *ppBmp = nullptr;

    return STD_SUCCESS;
}

STD_API STD_Rslt STD_CALL CONT_BitmapReset(CONT_Bitmap *pBmp) {
    if (!CONT_BitmapIsValid(pBmp)) {
        return STD_ERR_INV_ARG;
    }

    pBmp->set_count     = 0;
    pBmp->first_set_idx = STD_INVALID_IDX;
    memset(pBmp->pWords, 0, sizeof(CONT_BitmapWord) * pBmp->word_cap);

    return STD_SUCCESS;
}

/* ---- BITMAP SIZING ---- */

STD_API STD_Rslt STD_CALL CONT_BitmapChangeSize(CONT_Bitmap *pBmp,
                                                STD_Size     new_bit_cap) {
    if (!CONT_BitmapIsValid(pBmp) || new_bit_cap == 0) {
        return STD_ERR_INV_ARG;
    }

    STD_Size        new_word_cap   = CONT_BitmapWordIdx(new_bit_cap) + 1;
    STD_Size        truncated_bits = 0;
    CONT_BitmapWord mask           = (CONT_BitmapWord)~0;
    if (new_bit_cap < pBmp->bit_cap) {
        STD_Size bit_idx         = CONT_BitmapWordBitIdx(new_bit_cap);
        STD_Size spillover_bits  = STD_BITS_IN_TYPE(CONT_BitmapWord) - bit_idx;
        STD_Size mask_rshift_ofs = CONT_BitmapWordBitIdx(spillover_bits);
        mask >>= mask_rshift_ofs;

        truncated_bits +=
            stdc_count_ones_ull(pBmp->pWords[new_word_cap - 1] & ~mask);
        for (STD_Size idx = new_word_cap; idx < pBmp->word_cap; idx++) {
            truncated_bits += stdc_count_ones_ull(pBmp->pWords[idx]);
        }
    }
    CONT_BitmapWord *pWords =
        realloc(pBmp->pWords, sizeof(CONT_BitmapWord) * new_word_cap);
    if (!pWords) {
        return STD_ERR_OOM;
    }
    if (new_word_cap > pBmp->word_cap) {
        memset(&(pWords[pBmp->word_cap]), 0,
               sizeof(CONT_BitmapWord) * (new_word_cap - pBmp->word_cap));
    }
    pBmp->pWords = pWords;
    // If the mask was modified this will be correct, otherwise the word won't
    // change.
    pBmp->pWords[new_word_cap - 1] &= mask;
    pBmp->word_cap                  = new_word_cap;
    pBmp->bit_cap                   = new_bit_cap;
    pBmp->set_count                -= truncated_bits;
    if (pBmp->first_set_idx >= pBmp->bit_cap) {
        /*
         * Truncating the first set bit, make the total set count = 0, meaning
         * there is no bits set.
         */
        pBmp->first_set_idx = STD_INVALID_IDX;
    }

    return STD_SUCCESS;
}

STD_API STD_Rslt STD_CALL CONT_BitmapShrinkFit(CONT_Bitmap *pBmp) {
    if (!CONT_BitmapIsValid(pBmp)) {
        return STD_ERR_INV_ARG;
    }

    STD_Size new_bit_cap = CONT_BITMAP_DEFAULT_BIT_CAP;
    if (pBmp->set_count) {
        STD_Size idx = pBmp->word_cap;
        for (; idx-- > 0 && !pBmp->pWords[idx];)
            ;

        new_bit_cap = (idx + 1) * STD_BITS_IN_TYPE(CONT_BitmapWord);
    }
    // This can happen due to the above full word calculation.
    if (new_bit_cap >= pBmp->bit_cap) {
        return STD_SUCCESS;
    }

    return CONT_BitmapChangeSize(pBmp, new_bit_cap);
}

/* ---- BITMAP ATTRIBUTES ---- */

STD_API STD_Size STD_CALL CONT_BitmapSetCount(const CONT_Bitmap *pBmp) {
    DIAG_Verify(CONT_BitmapIsValid(pBmp));

    return pBmp->set_count;
}

STD_API STD_Size STD_CALL CONT_BitmapClearCount(const CONT_Bitmap *pBmp) {
    DIAG_Verify(CONT_BitmapIsValid(pBmp));

    return pBmp->bit_cap - pBmp->set_count;
}

STD_API STD_Size STD_CALL CONT_BitmapBitCap(const CONT_Bitmap *pBmp) {
    DIAG_Verify(CONT_BitmapIsValid(pBmp));

    return pBmp->bit_cap;
}

STD_API STD_Size STD_CALL CONT_BitmapWordCap(const CONT_Bitmap *pBmp) {
    DIAG_Verify(CONT_BitmapIsValid(pBmp));

    return pBmp->word_cap;
}

STD_API STD_Size STD_CALL CONT_BitmapFFS(const CONT_Bitmap *pBmp) {
    DIAG_Verify(CONT_BitmapIsValid(pBmp));

    return pBmp->first_set_idx;
}

STD_API STD_Size STD_CALL CONT_BitmapBitRank(const CONT_Bitmap *pBmp,
                                             STD_Size           idx) {
    DIAG_Verify(CONT_BitmapIsValid(pBmp));
    DIAG_VerifyMsg(idx < pBmp->bit_cap, "The given idx is out of bounds.");

    STD_Size rank = 0;
    for (STD_Size i = 0; i < CONT_BitmapWordIdx(idx); i++) {
        rank += stdc_count_ones_ull(pBmp->pWords[i]);
    }
    rank += stdc_count_ones_ull(pBmp->pWords[CONT_BitmapWordIdx(idx)] &
                                (CONT_BitmapWordMask(idx) - 1));

    return rank;
}

STD_API const CONT_BitmapWord *STD_CALL
CONT_BitmapRaw(const CONT_Bitmap *pBmp) {
    DIAG_Verify(CONT_BitmapIsValid(pBmp));

    return pBmp->pWords;
}

/* ---- TRIVIAL ---- */

STD_API STD_Bool STD_CALL CONT_BitmapIsAllClear(const CONT_Bitmap *pBmp) {
    DIAG_Verify(CONT_BitmapIsValid(pBmp));

    return pBmp->set_count == 0;
}

STD_API STD_Bool STD_CALL CONT_BitmapIsAllSet(const CONT_Bitmap *pBmp) {
    DIAG_Verify(CONT_BitmapIsValid(pBmp));

    return pBmp->set_count == pBmp->bit_cap;
}

STD_API STD_Bool STD_CALL CONT_BitmapIsSet(const CONT_Bitmap *pBmp,
                                           STD_Size           idx) {
    DIAG_Verify(CONT_BitmapIsValid(pBmp));
    DIAG_VerifyMsg(idx < pBmp->bit_cap, "The given idx is out of bounds.");

    return (pBmp->pWords[CONT_BitmapWordIdx(idx)] & CONT_BitmapWordMask(idx)) !=
           0;
}

STD_API STD_Bool STD_CALL CONT_BitmapIsClear(const CONT_Bitmap *pBmp,
                                             STD_Size           idx) {
    DIAG_Verify(CONT_BitmapIsValid(pBmp));
    DIAG_VerifyMsg(idx < pBmp->bit_cap, "The given idx is out of bounds.");

    return (pBmp->pWords[CONT_BitmapWordIdx(idx)] & CONT_BitmapWordMask(idx)) ==
           0;
}

#define MakeSameWordMask(mask, idx1, idx2)                                     \
    do {                                                                       \
        (mask) = (CONT_BitmapWord)~0 << CONT_BitmapWordBitIdx((idx1));         \
        if (CONT_BitmapWordBitIdx((idx2)) < 63) {                              \
            (mask) &=                                                          \
                ~((CONT_BitmapWord)~0 << (CONT_BitmapWordBitIdx((idx1)) + 1)); \
        }                                                                      \
    } while (0)

#define MakePartialFirstWordMask(idx)                                          \
    (((CONT_BitmapWord)~0) << CONT_BitmapWordBitIdx((idx)))

#define MakePartialLastWordMask(idx)                                           \
    (CONT_BitmapWordBitIdx((idx)) == 63) ? (CONT_BitmapWord)~0                 \
                                         : (CONT_BitmapWordMask(idx + 1) - 1)

STD_API STD_Bool STD_CALL CONT_BitmapIsSetAny(const CONT_Bitmap *pBmp,
                                              STD_Size idx1, STD_Size idx2) {
    DIAG_Verify(CONT_BitmapIsValid(pBmp));
    DIAG_VerifyMsg(idx1 <= idx2,
                   "The given idx1 must be smaller than given idx2.");
    DIAG_VerifyMsg(idx1 < pBmp->bit_cap, "The given idx1 is out of bounds.");
    DIAG_VerifyMsg(idx2 <= pBmp->bit_cap, "The given idx2 is out of bounds.");

    // Since the is set an exclusive
    STD_Size        last      = idx2 - 1;
    STD_Size        word_idx1 = CONT_BitmapWordIdx(idx1);
    STD_Size        word_idx2 = CONT_BitmapWordIdx(last);
    CONT_BitmapWord mask;
    if (word_idx1 == word_idx2) {
        MakeSameWordMask(mask, idx1, last);
        return ((pBmp->pWords[word_idx1] & mask) != 0);
    } else {
        mask = MakePartialFirstWordMask(idx1);
        if ((pBmp->pWords[word_idx1] & mask) != 0) {
            return true;
        }
        mask = MakePartialLastWordMask(last);
        if ((pBmp->pWords[word_idx2] & mask) != 0) {
            return true;
        }
        for (++word_idx1; word_idx1 < word_idx2; word_idx1++) {
            if (pBmp->pWords[word_idx1]) {
                return true;
            }
        }
    }

    return false;
}

STD_API STD_Bool STD_CALL CONT_BitmapIsSetAll(const CONT_Bitmap *pBmp,
                                              STD_Size idx1, STD_Size idx2) {
    DIAG_Verify(CONT_BitmapIsValid(pBmp));
    DIAG_VerifyMsg(idx1 <= idx2,
                   "The given idx1 must be smaller than given idx2.");
    DIAG_VerifyMsg(idx1 < pBmp->bit_cap, "The given idx1 is out of bounds.");
    DIAG_VerifyMsg(idx2 <= pBmp->bit_cap, "The given idx2 is out of bounds.");

    // Since the is set an exclusive
    STD_Size        last      = idx2 - 1;
    STD_Size        word_idx1 = CONT_BitmapWordIdx(idx1);
    STD_Size        word_idx2 = CONT_BitmapWordIdx(last);
    CONT_BitmapWord mask;
    if (word_idx1 == word_idx2) {
        MakeSameWordMask(mask, idx1, last);
        return ((pBmp->pWords[word_idx1] & mask) == mask);
    } else {
        mask = MakePartialFirstWordMask(idx1);
        if ((pBmp->pWords[word_idx1] & mask) != mask) {
            return false;
        }
        mask = MakePartialLastWordMask(last);
        if ((pBmp->pWords[word_idx2] & mask) != mask) {
            return false;
        }
        for (++word_idx1; word_idx1 < word_idx2; word_idx1++) {
            if (pBmp->pWords[word_idx1] != (CONT_BitmapWord)~0) {
                return false;
            }
        }
    }

    return true;
}

STD_API STD_Bool STD_CALL CONT_BitmapIsClearAny(const CONT_Bitmap *pBmp,
                                                STD_Size idx1, STD_Size idx2) {
    DIAG_Verify(CONT_BitmapIsValid(pBmp));
    DIAG_VerifyMsg(idx1 <= idx2,
                   "The given idx1 must be smaller than given idx2.");
    DIAG_VerifyMsg(idx1 < pBmp->bit_cap, "The given idx1 is out of bounds.");
    DIAG_VerifyMsg(idx2 <= pBmp->bit_cap, "The given idx2 is out of bounds.");

    // Since the is set an exclusive
    STD_Size        last      = idx2 - 1;
    STD_Size        word_idx1 = CONT_BitmapWordIdx(idx1);
    STD_Size        word_idx2 = CONT_BitmapWordIdx(last);
    CONT_BitmapWord mask;
    if (word_idx1 == word_idx2) {
        MakeSameWordMask(mask, idx1, last);
        return ((pBmp->pWords[word_idx1] & mask) != mask);
    } else {
        mask = MakePartialFirstWordMask(idx1);
        if ((pBmp->pWords[word_idx1] & mask) != mask) {
            return true;
        }
        mask = MakePartialLastWordMask(last);
        if ((pBmp->pWords[word_idx2] & mask) != mask) {
            return true;
        }
        for (++word_idx1; word_idx1 < word_idx2; word_idx1++) {
            if (pBmp->pWords[word_idx1] != (CONT_BitmapWord)~0) {
                return true;
            }
        }
    }

    return false;
}

STD_API STD_Bool STD_CALL CONT_BitmapIsClearAll(const CONT_Bitmap *pBmp,
                                                STD_Size idx1, STD_Size idx2) {
    DIAG_Verify(CONT_BitmapIsValid(pBmp));
    DIAG_VerifyMsg(idx1 <= idx2,
                   "The given idx1 must be smaller than given idx2.");
    DIAG_VerifyMsg(idx1 < pBmp->bit_cap, "The given idx1 is out of bounds.");
    DIAG_VerifyMsg(idx2 <= pBmp->bit_cap, "The given idx2 is out of bounds.");

    // Since the is set an exclusive
    STD_Size        last      = idx2 - 1;
    STD_Size        word_idx1 = CONT_BitmapWordIdx(idx1);
    STD_Size        word_idx2 = CONT_BitmapWordIdx(last);
    CONT_BitmapWord mask;
    if (word_idx1 == word_idx2) {
        MakeSameWordMask(mask, idx1, last);
        return ((pBmp->pWords[word_idx1] & mask) == 0);
    } else {
        mask = MakePartialFirstWordMask(idx1);
        if ((pBmp->pWords[word_idx1] & mask) != 0) {
            return false;
        }
        mask = MakePartialLastWordMask(last);
        if ((pBmp->pWords[word_idx2] & mask) != 0) {
            return false;
        }
        for (++word_idx1; word_idx1 < word_idx2; word_idx1++) {
            if (pBmp->pWords[word_idx1]) {
                return false;
            }
        }
    }

    return true;
}

STD_API STD_Bool STD_CALL CONT_BitmapHasAll(const CONT_Bitmap *pBmp1,
                                            const CONT_Bitmap *pBmp2) {
    DIAG_Verify(CONT_BitmapIsValid(pBmp1));
    DIAG_Verify(CONT_BitmapIsValid(pBmp2));

    STD_Size min_words =
        (pBmp1->word_cap > pBmp2->word_cap) ? pBmp2->word_cap : pBmp1->word_cap;
    for (STD_Size idx = 0; idx < min_words; idx++) {
        if ((pBmp1->pWords[idx] & pBmp2->pWords[idx]) != pBmp1->pWords[idx]) {
            return false;
        }
    }
    for (STD_Size idx = min_words; idx < pBmp2->word_cap; idx++) {
        if (pBmp2->pWords[idx]) {
            return false;
        }
    }

    return true;
}

STD_API STD_Bool STD_CALL CONT_BitmapHasAny(const CONT_Bitmap *pBmp1,
                                            const CONT_Bitmap *pBmp2) {
    DIAG_Verify(CONT_BitmapIsValid(pBmp1));
    DIAG_Verify(CONT_BitmapIsValid(pBmp2));

    STD_Size min_words =
        (pBmp1->word_cap > pBmp2->word_cap) ? pBmp2->word_cap : pBmp1->word_cap;
    for (STD_Size idx = 0; idx < min_words; idx++) {
        if (pBmp1->pWords[idx] & pBmp2->pWords[idx]) {
            return false;
        }
    }

    // Not checking beyond min_words as in this case this is irrelevant.
    return false;
}

STD_API STD_Bool STD_CALL CONT_BitmapCmp(const CONT_Bitmap *pBmp1,
                                         const CONT_Bitmap *pBmp2) {
    DIAG_Verify(CONT_BitmapIsValid(pBmp1));
    DIAG_Verify(CONT_BitmapIsValid(pBmp2));

    if (pBmp1->set_count != pBmp2->set_count ||
        pBmp1->first_set_idx != pBmp2->first_set_idx) {
        return false;
    }

    STD_Size min_words =
        (pBmp1->word_cap > pBmp2->word_cap) ? pBmp2->word_cap : pBmp1->word_cap;
    STD_Size max_words =
        (pBmp1->word_cap < pBmp2->word_cap) ? pBmp2->word_cap : pBmp1->word_cap;
    if (memcmp(pBmp1->pWords, pBmp2->pWords,
               sizeof(CONT_BitmapWord) * min_words) != 0) {
        return false;
    }

    // The below loop will not run for edge cases. This is how loops behave.
    const CONT_Bitmap *pMax_bmp =
        (pBmp1->word_cap == max_words) ? pBmp1 : pBmp2;
    for (STD_Size idx = min_words; idx < max_words; idx++) {
        if (pMax_bmp->pWords[idx]) {
            return false;
        }
    }

    return true;
}

/* ---- MODIFICATION FUNCTIONS ---- */

#define FindNewFirstSet(pBmp)                                                  \
    do {                                                                       \
        STD_Size old_first_set_idx = (pBmp)->first_set_idx;                    \
        (pBmp)->first_set_idx      = STD_INVALID_IDX;                          \
        STD_Size word_idx          = CONT_BitmapWordIdx(old_first_set_idx);    \
        STD_Size i                 = word_idx;                                 \
        for (; i < (pBmp)->word_cap; i++) {                                    \
            if ((pBmp)->pWords[i]) {                                           \
                (pBmp)->first_set_idx =                                        \
                    (stdc_first_trailing_one_ull((pBmp)->pWords[i]) - 1) +     \
                    (i * STD_BITS_IN_TYPE(CONT_BitmapWord));                   \
            }                                                                  \
        }                                                                      \
    } while (0)

STD_API void STD_CALL CONT_BitmapSetUnchecked(CONT_Bitmap *pBmp, STD_Size idx) {
    CONT_BitmapIsValidAssert(pBmp);
    DIAG_AssertMsg(idx < pBmp->bit_cap, "The given idx is out of bounds.");

    STD_Size        word_idx = CONT_BitmapWordIdx(idx);
    CONT_BitmapWord mask     = CONT_BitmapWordMask(idx);
    if (pBmp->pWords[word_idx] & mask) {
        return;
    }
    pBmp->pWords[word_idx] |= mask;
    pBmp->set_count++;
    if (idx < pBmp->first_set_idx) {
        pBmp->first_set_idx = idx;
    }
}

STD_API STD_Rslt STD_CALL CONT_BitmapSet(CONT_Bitmap *pBmp, STD_Size idx) {
    if (!CONT_BitmapIsValid(pBmp)) {
        return STD_ERR_INV_ARG;
    }
    if (idx >= pBmp->bit_cap) {
        return STD_ERR_OOB;
    }

    CONT_BitmapSetUnchecked(pBmp, idx);

    return STD_SUCCESS;
}

STD_API void STD_CALL CONT_BitmapClearUnchecked(CONT_Bitmap *pBmp,
                                                STD_Size     idx) {
    CONT_BitmapIsValidAssert(pBmp);
    DIAG_AssertMsg(idx < pBmp->bit_cap, "The given idx is out of bounds.");

    STD_Size        word_idx = CONT_BitmapWordIdx(idx);
    CONT_BitmapWord mask     = CONT_BitmapWordMask(idx);
    if (pBmp->pWords[word_idx] & mask) {
        pBmp->pWords[word_idx] &= ~mask;
        pBmp->set_count--;
        if (idx == pBmp->first_set_idx) {
            FindNewFirstSet(pBmp);
        }
    }
}

STD_API STD_Rslt STD_CALL CONT_BitmapClear(CONT_Bitmap *pBmp, STD_Size idx) {
    if (!CONT_BitmapIsValid(pBmp)) {
        return STD_ERR_INV_ARG;
    }
    if (idx >= pBmp->bit_cap) {
        return STD_ERR_OOB;
    }

    CONT_BitmapClearUnchecked(pBmp, idx);

    return STD_SUCCESS;
}

STD_API void STD_CALL CONT_BitmapToggleUnchecked(CONT_Bitmap *pBmp,
                                                 STD_Size     idx) {
    CONT_BitmapIsValidAssert(pBmp);
    DIAG_AssertMsg(idx < pBmp->bit_cap, "The given idx is out of bounds.");

    STD_Size        word_idx  = CONT_BitmapWordIdx(idx);
    CONT_BitmapWord mask      = CONT_BitmapWordMask(idx);
    pBmp->pWords[word_idx]   ^= mask;

    if (pBmp->pWords[word_idx] & mask) {
        pBmp->set_count++;
        if (idx < pBmp->first_set_idx) {
            pBmp->first_set_idx = idx;
        }
    } else {
        pBmp->set_count--;
        if (idx == pBmp->first_set_idx) {
            FindNewFirstSet(pBmp);
        }
    }
}

STD_API STD_Rslt STD_CALL CONT_BitmapToggle(CONT_Bitmap *pBmp, STD_Size idx) {
    if (!CONT_BitmapIsValid(pBmp)) {
        return STD_ERR_INV_ARG;
    }
    if (idx >= pBmp->bit_cap) {
        return STD_ERR_OOB;
    }

    CONT_BitmapToggleUnchecked(pBmp, idx);

    return STD_SUCCESS;
}

STD_API void STD_CALL CONT_BitmapSetManyUnchecked(CONT_Bitmap *pBmp,
                                                  STD_Size     idx1,
                                                  STD_Size     idx2) {
    CONT_BitmapIsValidAssert(pBmp);
    DIAG_AssertMsg(idx1 <= idx2,
                   "The given idx1 must be smaller than given idx2.");
    DIAG_AssertMsg(idx1 < pBmp->bit_cap, "The given idx1 is out of bounds.");
    DIAG_AssertMsg(idx2 <= pBmp->bit_cap, "The given idx2 is out of bounds.");

    // Since the is set an exclusive
    STD_Size        last      = idx2 - 1;
    STD_Size        word_idx1 = CONT_BitmapWordIdx(idx1);
    STD_Size        word_idx2 = CONT_BitmapWordIdx(last);
    CONT_BitmapWord mask;
    if (word_idx1 == word_idx2) {
        MakeSameWordMask(mask, idx1, last);
        pBmp->set_count +=
            (stdc_count_ones_ull(mask) -
             stdc_count_ones_ull(pBmp->pWords[word_idx1] & mask));
        pBmp->pWords[word_idx1] |= mask;
    } else {
        mask = MakePartialFirstWordMask(idx1);
        pBmp->set_count +=
            (stdc_count_ones_ull(mask) -
             stdc_count_ones_ull(pBmp->pWords[word_idx1] & mask));
        pBmp->pWords[word_idx1] |= mask;

        mask = MakePartialLastWordMask(last);
        pBmp->set_count +=
            (stdc_count_ones_ull(mask) -
             stdc_count_ones_ull(pBmp->pWords[word_idx2] & mask));
        pBmp->pWords[word_idx2] |= mask;

        for (++word_idx1; word_idx1 < word_idx2; word_idx1++) {
            if (pBmp->pWords[word_idx1]) {
                pBmp->set_count += STD_BITS_IN_TYPE(CONT_BitmapWord) -
                                   stdc_count_ones_ull(pBmp->pWords[word_idx1]);
                pBmp->pWords[word_idx1] = (CONT_BitmapWord)~0;
            }
        }
    }

    if (pBmp->first_set_idx == STD_INVALID_IDX || idx1 < pBmp->first_set_idx) {
        pBmp->first_set_idx = idx1;
    }
}

STD_API STD_Rslt STD_CALL CONT_BitmapSetMany(CONT_Bitmap *pBmp, STD_Size idx1,
                                             STD_Size idx2) {
    if (!CONT_BitmapIsValid(pBmp) || idx1 > idx2) {
        return STD_ERR_INV_ARG;
    }
    if (idx1 >= pBmp->bit_cap || idx2 > pBmp->bit_cap) {
        return STD_ERR_OOB;
    }

    CONT_BitmapSetManyUnchecked(pBmp, idx1, idx2);

    return STD_SUCCESS;
}

STD_API void STD_CALL CONT_BitmapClearManyUnchecked(CONT_Bitmap *pBmp,
                                                    STD_Size     idx1,
                                                    STD_Size     idx2) {
    CONT_BitmapIsValidAssert(pBmp);
    DIAG_AssertMsg(idx1 <= idx2,
                   "The given idx1 must be smaller than given idx2.");
    DIAG_AssertMsg(idx1 < pBmp->bit_cap, "The given idx1 is out of bounds.");
    DIAG_AssertMsg(idx2 <= pBmp->bit_cap, "The given idx2 is out of bounds.");

    if (!pBmp->set_count) {
        return;
    }

    // Since the is set an exclusive
    STD_Size        last      = idx2 - 1;
    STD_Size        word_idx1 = CONT_BitmapWordIdx(idx1);
    STD_Size        word_idx2 = CONT_BitmapWordIdx(last);
    CONT_BitmapWord mask;
    if (word_idx1 == word_idx2) {
        MakeSameWordMask(mask, idx1, last);
        pBmp->set_count -= stdc_count_ones_ull(pBmp->pWords[word_idx1] & mask);
        pBmp->pWords[word_idx1] &= ~mask;
    } else {
        mask             = MakePartialFirstWordMask(idx1);
        pBmp->set_count -= stdc_count_ones_ull(pBmp->pWords[word_idx1] & mask);
        pBmp->pWords[word_idx1] &= ~mask;

        mask             = MakePartialLastWordMask(last);
        pBmp->set_count -= stdc_count_ones_ull(pBmp->pWords[word_idx2] & mask);
        pBmp->pWords[word_idx2] &= ~mask;

        for (++word_idx1; word_idx1 < word_idx2; word_idx1++) {
            if (pBmp->pWords[word_idx1]) {
                pBmp->set_count -= stdc_count_ones_ull(pBmp->pWords[word_idx1]);
                pBmp->pWords[word_idx1] = 0;
            }
        }
    }

    if (pBmp->first_set_idx >= idx1 || pBmp->first_set_idx < idx2) {
        pBmp->first_set_idx = last; // optimization little.
        FindNewFirstSet(pBmp);
    }
}

STD_API STD_Rslt STD_CALL CONT_BitmapClearMany(CONT_Bitmap *pBmp, STD_Size idx1,
                                               STD_Size idx2) {
    if (!CONT_BitmapIsValid(pBmp) || idx1 > idx2) {
        return STD_ERR_INV_ARG;
    }
    if (idx1 >= pBmp->bit_cap || idx2 > pBmp->bit_cap) {
        return STD_ERR_OOB;
    }

    CONT_BitmapClearManyUnchecked(pBmp, idx1, idx2);

    return STD_SUCCESS;
}

STD_API void STD_CALL CONT_BitmapToggleManyUnchecked(CONT_Bitmap *pBmp,
                                                     STD_Size     idx1,
                                                     STD_Size     idx2) {
    CONT_BitmapIsValidAssert(pBmp);
    DIAG_AssertMsg(idx1 <= idx2,
                   "The given idx1 must be smaller than given idx2.");
    DIAG_AssertMsg(idx1 < pBmp->bit_cap, "The given idx1 is out of bounds.");
    DIAG_AssertMsg(idx2 <= pBmp->bit_cap, "The given idx2 is out of bounds.");

    // Since the is set an exclusive
    STD_Size        last      = idx2 - 1;
    STD_Size        word_idx1 = CONT_BitmapWordIdx(idx1);
    STD_Size        word_idx2 = CONT_BitmapWordIdx(last);
    CONT_BitmapWord mask;
    if (word_idx1 == word_idx2) {
        MakeSameWordMask(mask, idx1, last);
        pBmp->set_count -= stdc_count_ones_ull(pBmp->pWords[word_idx1] & mask);
        pBmp->pWords[word_idx1] ^= mask;
        pBmp->set_count += stdc_count_ones_ull(pBmp->pWords[word_idx1] & mask);
    } else {
        mask             = MakePartialFirstWordMask(idx1);
        pBmp->set_count -= stdc_count_ones_ull(pBmp->pWords[word_idx1] & mask);
        pBmp->pWords[word_idx1] ^= mask;
        pBmp->set_count += stdc_count_ones_ull(pBmp->pWords[word_idx1] & mask);

        mask             = MakePartialLastWordMask(last);
        pBmp->set_count -= stdc_count_ones_ull(pBmp->pWords[word_idx2] & mask);
        pBmp->pWords[word_idx2] ^= mask;
        pBmp->set_count += stdc_count_ones_ull(pBmp->pWords[word_idx2] & mask);

        for (++word_idx1; word_idx1 < word_idx2; word_idx1++) {
            pBmp->set_count -= stdc_count_ones_ull(pBmp->pWords[word_idx1]);
            pBmp->pWords[word_idx1] ^= mask;
            pBmp->set_count += stdc_count_ones_ull(pBmp->pWords[word_idx1]);
        }
    }

    if (pBmp->first_set_idx >= idx1 || pBmp->first_set_idx < idx2) {
        pBmp->first_set_idx = idx1 - 1; // optimization little.
        FindNewFirstSet(pBmp);
    }
}

STD_API STD_Rslt STD_CALL CONT_BitmapToggleMany(CONT_Bitmap *pBmp,
                                                STD_Size idx1, STD_Size idx2) {
    if (!CONT_BitmapIsValid(pBmp) || idx1 > idx2) {
        return STD_ERR_INV_ARG;
    }
    if (idx1 >= pBmp->bit_cap || idx2 > pBmp->bit_cap) {
        return STD_ERR_OOB;
    }

    CONT_BitmapToggleManyUnchecked(pBmp, idx1, idx2);

    return STD_SUCCESS;
}

/* ---- BOOLEAN FUNCTIONS ---- */

STD_API void STD_CALL CONT_BitmapNotUnchecked(CONT_Bitmap *pBmp) {
    CONT_BitmapIsValidAssert(pBmp);

    pBmp->first_set_idx = STD_INVALID_IDX;
    for (STD_Size idx = 0; idx < pBmp->word_cap; idx++) {
        pBmp->pWords[idx] = ~(pBmp->pWords[idx]);

        if (pBmp->first_set_idx == STD_INVALID_IDX && pBmp->pWords[idx]) {
            pBmp->first_set_idx =
                (stdc_first_trailing_one_ull(pBmp->pWords[idx]) - 1) +
                (idx * STD_BITS_IN_TYPE(CONT_BitmapWord));
        }
    }
    pBmp->set_count = pBmp->bit_cap - pBmp->set_count;

    // Clearing the spillover bits that were also set due to not operation.
    STD_Size bit_idx         = CONT_BitmapWordBitIdx(pBmp->bit_cap);
    STD_Size spillover_bits  = STD_BITS_IN_TYPE(CONT_BitmapWord) - bit_idx;
    STD_Size mask_rshift_ofs = CONT_BitmapWordBitIdx(spillover_bits);
    CONT_BitmapWord mask     = (CONT_BitmapWord)~0 >> mask_rshift_ofs;
    pBmp->pWords[pBmp->word_cap - 1] &= mask;
}

STD_API STD_Rslt STD_CALL CONT_BitmapNot(CONT_Bitmap *pBmp) {
    if (!CONT_BitmapIsValid(pBmp)) {
        return STD_ERR_INV_ARG;
    }

    CONT_BitmapNotUnchecked(pBmp);

    return STD_SUCCESS;
}

STD_API void STD_CALL CONT_BitmapAndUnchecked(CONT_Bitmap       *pBmp1,
                                              const CONT_Bitmap *pBmp2) {
    CONT_BitmapIsValidAssert(pBmp1);
    CONT_BitmapIsValidAssert(pBmp2);

    pBmp1->set_count     = 0;
    pBmp1->first_set_idx = STD_INVALID_IDX;
    STD_Size min_cap =
        (pBmp1->word_cap > pBmp2->word_cap) ? pBmp2->word_cap : pBmp1->word_cap;
    for (STD_Size idx = 0; idx < min_cap; idx++) {
        pBmp1->pWords[idx] &= pBmp2->pWords[idx];

        STD_Size one_count  = stdc_count_ones_ull(pBmp1->pWords[idx]);
        pBmp1->set_count   += one_count;

        if (pBmp1->first_set_idx == STD_INVALID_IDX && one_count) {
            pBmp1->first_set_idx =
                (stdc_first_trailing_one_ull(pBmp1->pWords[idx]) - 1) +
                (idx * STD_BITS_IN_TYPE(CONT_BitmapWord));
        }
    }
    for (STD_Size idx = min_cap; idx < pBmp1->word_cap; idx++) {
        pBmp1->pWords[idx] = 0;
    }
}

STD_API STD_Rslt STD_CALL CONT_BitmapAnd(CONT_Bitmap       *pBmp1,
                                         const CONT_Bitmap *pBmp2) {
    if (!CONT_BitmapIsValid(pBmp1) || !CONT_BitmapIsValid(pBmp2)) {
        return STD_ERR_INV_ARG;
    }

    CONT_BitmapAndUnchecked(pBmp1, pBmp2);

    return STD_SUCCESS;
}

STD_API void STD_CALL CONT_BitmapOrUnchecked(CONT_Bitmap       *pBmp1,
                                             const CONT_Bitmap *pBmp2) {
    CONT_BitmapIsValidAssert(pBmp1);
    CONT_BitmapIsValidAssert(pBmp2);

    pBmp1->set_count     = 0;
    pBmp1->first_set_idx = STD_INVALID_IDX;
    STD_Size min_cap =
        (pBmp1->word_cap > pBmp2->word_cap) ? pBmp2->word_cap : pBmp1->word_cap;
    for (STD_Size idx = 0; idx < min_cap; idx++) {
        pBmp1->pWords[idx] |= pBmp2->pWords[idx];

        STD_Size one_count  = stdc_count_ones_ull(pBmp1->pWords[idx]);
        pBmp1->set_count   += one_count;

        if (pBmp1->first_set_idx == STD_INVALID_IDX && one_count) {
            pBmp1->first_set_idx =
                (stdc_first_trailing_one_ull(pBmp1->pWords[idx]) - 1) +
                (idx * STD_BITS_IN_TYPE(CONT_BitmapWord));
        }
    }
    for (STD_Size idx = min_cap; idx < pBmp1->word_cap; idx++) {
        STD_Size one_count  = stdc_count_ones_ull(pBmp1->pWords[idx]);
        pBmp1->set_count   += one_count;

        if (pBmp1->first_set_idx == STD_INVALID_IDX && one_count) {
            pBmp1->first_set_idx =
                (stdc_first_trailing_one_ull(pBmp1->pWords[idx]) - 1) +
                (idx * STD_BITS_IN_TYPE(CONT_BitmapWord));
            break;
        }
    }
}

STD_API STD_Rslt STD_CALL CONT_BitmapOr(CONT_Bitmap       *pBmp1,
                                        const CONT_Bitmap *pBmp2) {
    if (!CONT_BitmapIsValid(pBmp1) || !CONT_BitmapIsValid(pBmp2)) {
        return STD_ERR_INV_ARG;
    }

    CONT_BitmapOrUnchecked(pBmp1, pBmp2);

    return STD_SUCCESS;
}

STD_API void STD_CALL CONT_BitmapXorUnchecked(CONT_Bitmap       *pBmp1,
                                              const CONT_Bitmap *pBmp2) {
    CONT_BitmapIsValidAssert(pBmp1);
    CONT_BitmapIsValidAssert(pBmp2);

    pBmp1->set_count     = 0;
    pBmp1->first_set_idx = STD_INVALID_IDX;
    STD_Size min_cap =
        (pBmp1->word_cap > pBmp2->word_cap) ? pBmp2->word_cap : pBmp1->word_cap;
    for (STD_Size idx = 0; idx < min_cap; idx++) {
        pBmp1->pWords[idx] ^= pBmp2->pWords[idx];

        STD_Size one_count  = stdc_count_ones_ull(pBmp1->pWords[idx]);
        pBmp1->set_count   += one_count;

        if (pBmp1->first_set_idx == STD_INVALID_IDX && one_count) {
            pBmp1->first_set_idx =
                (stdc_first_trailing_one_ull(pBmp1->pWords[idx]) - 1) +
                (idx * STD_BITS_IN_TYPE(CONT_BitmapWord));
        }
    }
    for (STD_Size idx = min_cap; idx < pBmp1->word_cap; idx++) {
        STD_Size one_count  = stdc_count_ones_ull(pBmp1->pWords[idx]);
        pBmp1->set_count   += one_count;

        if (pBmp1->first_set_idx == STD_INVALID_IDX && one_count) {
            pBmp1->first_set_idx =
                (stdc_first_trailing_one_ull(pBmp1->pWords[idx]) - 1) +
                (idx * STD_BITS_IN_TYPE(CONT_BitmapWord));
            break;
        }
    }
}

STD_API STD_Rslt STD_CALL CONT_BitmapXor(CONT_Bitmap       *pBmp1,
                                         const CONT_Bitmap *pBmp2) {
    if (!CONT_BitmapIsValid(pBmp1) || !CONT_BitmapIsValid(pBmp2)) {
        return STD_ERR_INV_ARG;
    }

    CONT_BitmapXorUnchecked(pBmp1, pBmp2);

    return STD_SUCCESS;
}

// STD_API void STD_CALL CONT_BitmapLShiftUnchecked(CONT_Bitmap *pBmp,
//                                                  STD_Size     ofs) {
//     CONT_BitmapIsValidAssert(pBmp);

//     if (ofs == 0 || pBmp->set_count == 0) {
//         return;
//     }
//     if (ofs >= pBmp->bit_cap || pBmp->bit_cap - ofs <= pBmp->first_set_idx) {
//         // Quick path since all the bits are going to be gone, shifted away.
//         STD_Rslt code = CONT_BitmapReset(pBmp);
//         if (code != STD_SUCCESS) {
//             DIAG_Unreachable(); // Since bitmap was already validated above.
//         }
//         return;
//     }

//     STD_Size full_word_ofs    = CONT_BitmapWordIdx(ofs);
//     STD_Size partial_word_ofs = ofs - full_word_ofs;
//     if (full_word_ofs) {
//         for (STD_Size idx = pBmp->word_cap - full_word_ofs;
//              idx < pBmp->word_cap; idx++) {
//         }
//     }
//     if (partial_word_ofs) {
//     }

// }

// STD_API STD_Rslt STD_CALL CONT_BitmapLShift(CONT_Bitmap *pBmp, STD_Size ofs)
// {
//     if (!CONT_BitmapIsValid(pBmp)) {
//         return STD_ERR_INV_ARG;
//     }

//     CONT_BitmapLShiftUnchecked(pBmp, ofs);

//     return STD_SUCCESS;
// }

// STD_API void STD_CALL CONT_BitmapRShiftUnchecked(CONT_Bitmap *pBmp,
//                                                  STD_Size     ofs) {
//     CONT_BitmapIsValidAssert(pBmp);

//     if (ofs == 0 || pBmp->set_count == 0) {
//         return;
//     }
//     if (ofs >= pBmp->bit_cap) {
//         // Quick path since all the bits are going to be gone, shifted away.
//         STD_Rslt code = CONT_BitmapReset(pBmp);
//         if (code != STD_SUCCESS) {
//             DIAG_Unreachable(); // Since bitmap was already validated above.
//         }
//         return;
//     }

//     DIAG_Unimplemented();
// }

// STD_API STD_Rslt STD_CALL CONT_BitmapRShift(CONT_Bitmap *pBmp, STD_Size ofs)
// {
//     if (!CONT_BitmapIsValid(pBmp)) {
//         return STD_ERR_INV_ARG;
//     }

//     CONT_BitmapRShiftUnchecked(pBmp, ofs);

//     return STD_SUCCESS;
// }
