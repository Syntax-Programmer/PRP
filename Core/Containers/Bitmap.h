#ifndef DEF_CONT_BITMAP_
#define DEF_CONT_BITMAP_

#ifdef __cplusplus
extern "C" {
#endif

#include "Core/Diagnostics/Debug.h"
#include "Core/Std/Defs.h"
#include "Core/Std/Platform.h"
#include "Core/Std/Types.h"

typedef STD_U64 CONT_BitmapWord;

typedef struct CONT_Bitmap CONT_Bitmap;

#define CONT_BITMAP_DEFAULT_BIT_CAP (STD_BITS_IN_TYPE(CONT_BitmapWord))
#define CONT_BITMAP_MAX_BIT_CAP     (STD_SIZE_MAX)

#define CONT_BitmapWordIdx(idx)    ((idx) >> 6)
#define CONT_BitmapWordBitIdx(idx) ((idx) & 63)
#define CONT_BitmapWordMask(idx)   ((CONT_BitmapWord)1 << ((idx) & 63))

/**
 * Checks whether the given bitmap object is valid.
 *
 * @param pBmp The bitmap object to check.
 *
 * @return true if valid, false otherwise.
 */
[[nodiscard("Must not call functions when you don't use their result.")]]
STD_API STD_Bool STD_CALL CONT_BitmapIsValid(const CONT_Bitmap *pBmp);

#define CONT_BitmapIsValidAssert(pBmp)                                         \
    DIAG_AssertMsg(CONT_BitmapIsValid(pBmp),                                   \
                   "The given bitmap object is corrupted/invalid.")

/* ---- CONSTRUCTORS/DESTRUCTORS ---- */

/**
 * Creates a new bitmap object with the specified number of bits.
 *
 * @param bit_cap The number of bits that the bitmap shall track and maintain.
 *                Must bee greater than 0.
 * @param ppBmp   Output pointer that receives the created bitmap object.
 *                Must not equal nullptr.
 *
 * @return STD_SUCCESS     on success.
 * @return STD_ERR_INV_ARG if arguments are invalid.
 * @return STD_ERR_OOM     if memory allocation fails.
 */
[[nodiscard("This function can fail, please check result for success.")]]
STD_API STD_Rslt STD_CALL CONT_BitmapCreate(STD_Size      bit_cap,
                                            CONT_Bitmap **ppBmp);
/**
 * Creates a new bitmap object cloning the given bitmap object.
 *
 * @param pTo_clone The bitmap object to clone the new bitmap from.
 *                  Must be a valid  bitmap object.
 * @param ppBmp    Output pointer that receives the created bitmap object.
 *                 Must not equal nullptr.
 *
 * @return STD_SUCCESS     on success.
 * @return STD_ERR_INV_ARG if arguments are invalid.
 * @return STD_ERR_OOM     if memory allocation fails.
 */
[[nodiscard("This function can fail, please check result for success.")]]
STD_API STD_Rslt STD_CALL CONT_BitmapCreateClone(const CONT_Bitmap *pTo_clone,
                                                 CONT_Bitmap      **ppBmp);
/**
 * Destroys the bitmap and nullifies the object pointer.
 *
 * @param ppBmp Pointer to the bitmap object to delete.
 *              Must not equal nullptr.
 *              Its dereference must not equal nullptr.
 *
 * @return STD_SUCCESS     on success.
 * @return STD_ERR_INV_ARG if arguments are invalid.
 */
STD_API STD_Rslt STD_CALL CONT_BitmapDestroy(CONT_Bitmap **ppBmp);
/**
 * Resets and clears the bitmap.
 *
 * @param ppBmp The bitmap object to reset.
 *              Must be a valid bitmap object.
 *
 * @return STD_SUCCESS     on success.
 * @return STD_ERR_INV_ARG if arguments are invalid.
 */
[[nodiscard("This function can fail, please check result for success.")]]
STD_API STD_Rslt STD_CALL CONT_BitmapReset(CONT_Bitmap *pBmp);

/* ---- BITMAP SIZING ---- */

/**
 * Changes the bit capacity of the bitmap to the desired size.
 * Truncates bits when shrinking below the current bit cap.
 *
 * @param pBmp        The bitmap object to resize.
 *                    Must be a valid bitmap object.
 * @param new_bit_cap The new bit capacity to set the bitmap to.
 *                    Must be greater than 0.
 *
 * @return STD_SUCCESS     on success.
 * @return STD_ERR_INV_ARG if arguments are invalid.
 * @return STD_ERR_OOM     if memory allocation fails.
 */
[[nodiscard("This function can fail, please check result for success.")]]
STD_API STD_Rslt STD_CALL CONT_BitmapChangeSize(CONT_Bitmap *pBmp,
                                                STD_Size     new_bit_cap);
/**
 * Shrinks bit capacity to the MSB set in the bitmap.
 *
 * @param pBmp The bitmap object to resize.
 *             Must be valid bitmap object.
 *
 * @return STD_SUCCESS     on success.
 * @return STD_ERR_INV_ARG if arguments are invalid.
 * @return STD_ERR_OOM     if memory allocation fails.
 */
[[nodiscard("This function can fail, please check result for success.")]]
STD_API STD_Rslt STD_CALL CONT_BitmapShrinkFit(CONT_Bitmap *pBmp);

/* ---- BITMAP ATTRIBUTES ---- */

/**
 * Gets the set count of the given bitmap.
 *
 * @param pBmp The bitmap object to get set count of.
 *             Must be a valid bitmap object.
 *
 * @return The set count of the bitmap.
 */
[[nodiscard("Must not call functions when you don't use their result.")]]
STD_API STD_Size STD_CALL CONT_BitmapSetCount(const CONT_Bitmap *pBmp);
/**
 * Gets the clear count of the given bitmap.
 *
 * @param pBmp The bitmap object to get clear count of.
 *             Must be a valid bitmap object.
 *
 * @return The clear count of the bitmap.
 */
[[nodiscard("Must not call functions when you don't use their result.")]]
STD_API STD_Size STD_CALL CONT_BitmapClearCount(const CONT_Bitmap *pBmp);
/**
 * Gets the bit cap of the given bitmap.
 *
 * @param pBmp The bitmap object to get bit cap of.
 *             Must be a valid bitmap object.
 *
 * @return The bit cap of the bitmap.
 */
[[nodiscard("Must not call functions when you don't use their result.")]]
STD_API STD_Size STD_CALL CONT_BitmapBitCap(const CONT_Bitmap *pBmp);
/**
 * Gets the number of CONT_BitmapWords the given bitmap uses internally.
 *
 * @param pBmp The bitmap object to get word cap of.
 *             Must be a valid bitmap object.
 *
 * @return The word cap of the bitmap.
 */
[[nodiscard("Must not call functions when you don't use their result.")]]
STD_API STD_Size STD_CALL CONT_BitmapWordCap(const CONT_Bitmap *pBmp);
/**
 * Gets the 0-based index of the first set bit in the bitmap.
 *
 * @param pBmp The bitmap object to get first set index of.
 *             Must be a valid bitmap object.
 *
 * @return 0-based index if at least one bit is set. STD_INVALID_IDX otherwise.
 */
[[nodiscard("Must not call functions when you don't use their result.")]]
STD_API STD_Size STD_CALL CONT_BitmapFFS(const CONT_Bitmap *pBmp);
/**
 * Gets the rank of the given idx in the bitmap.
 *
 * @param pBmp The bitmap to get the rank from.
 *             Must be valid bitmap object.
 * @param idx  The index to get the rank of.
 *             Must not be out of bounds.
 *
 * @return The rank of bit at the desired index of the bitmap.
 */
[[nodiscard("Must not call functions when you don't use their result.")]]
STD_API STD_Size STD_CALL CONT_BitmapBitRank(const CONT_Bitmap *pBmp,
                                             STD_Size           idx);
/**
 * Gets the raw memory of the given bitmap.
 * If any sizing or element adding/removing operation is performed after this
 * function, the memory pointer returned is not guaranteed to be valid anymore.
 *
 * @param pBmp The bitmap object to get raw memory of.
 *             Must be a valid bitmap object.
 *
 * @return The raw memory of the bitmap.
 */
[[nodiscard("Must not call functions when you don't use their result.")]]
STD_API const CONT_BitmapWord *STD_CALL CONT_BitmapRaw(const CONT_Bitmap *pBmp);

/* ---- TRIVIAL ---- */

/**
 * Checks if the given bitmap has no bits set.
 *
 * @param pBmp The bitmap object to check.
 *             Must be a valid bitmap object.
 *
 * @return true if all clear, false otherwise.
 */
[[nodiscard("Must not call functions when you don't use their result.")]]
STD_API STD_Bool STD_CALL CONT_BitmapIsAllClear(const CONT_Bitmap *pBmp);
/**
 * Checks if the given bitmap has all bits set.
 *
 * @param pBmp The bitmap object to check.
 *             Must be a valid bitmap object.
 *
 * @return true if all set, false otherwise.
 */
[[nodiscard("Must not call functions when you don't use their result.")]]
STD_API STD_Bool STD_CALL CONT_BitmapIsAllSet(const CONT_Bitmap *pBmp);
/**
 * Checks if the bit at the given index is set in the given bitmap.
 *
 * @param pBmp The bitmap object to check.
 *             Must be a valid bitmap object.
 * @param idx  The index of bit to check.
 *             Must not be out of bounds.
 *
 * @return true if set, false otherwise.
 */
[[nodiscard("Must not call functions when you don't use their result.")]]
STD_API STD_Bool STD_CALL CONT_BitmapIsSet(const CONT_Bitmap *pBmp,
                                           STD_Size           idx);
/**
 * Checks if the bit at the given index is clear in the given bitmap.
 *
 * @param pBmp The bitmap object to check.
 *             Must be a valid bitmap object.
 * @param idx  The index of bit to check.
 *             Must not be out of bounds.
 *
 * @return true if clear, false otherwise.
 */
[[nodiscard("Must not call functions when you don't use their result.")]]
STD_API STD_Bool STD_CALL CONT_BitmapIsClear(const CONT_Bitmap *pBmp,
                                             STD_Size           idx);
/**
 * Checks if any bit is set between the given range in the given bitmap.
 *
 * @param pBmp The bitmap object to check.
 *             Must be a valid bitmap object.
 * @param idx1 The lower limit in the index range(inclusive).
 *             Must not be out of bounds.
 * @param idx2 The upper limit in the index range(exclusive).
 *             Must not be out of bounds.
 *
 * @return true if any bit is set in the range, false otherwise.
 */
[[nodiscard("Must not call functions when you don't use their result.")]]
STD_API STD_Bool STD_CALL CONT_BitmapIsSetAny(const CONT_Bitmap *pBmp,
                                              STD_Size idx1, STD_Size idx2);
/**
 * Checks if all bit are set between the given range in the given bitmap.
 *
 * @param pBmp The bitmap object to check.
 *             Must be a valid bitmap object.
 * @param idx1 The lower limit in the index range(inclusive).
 *             Must not be out of bounds.
 * @param idx2 The upper limit in the index range(exclusive).
 *             Must not be out of bounds.
 *
 * @return true if all bits are set in the range, false otherwise.
 */
[[nodiscard("Must not call functions when you don't use their result.")]]
STD_API STD_Bool STD_CALL CONT_BitmapIsSetAll(const CONT_Bitmap *pBmp,
                                              STD_Size idx1, STD_Size idx2);
/**
 * Checks if any bit is clear between the given range in the given bitmap.
 *
 * @param pBmp The bitmap object to check.
 *             Must be a valid bitmap object.
 * @param idx1 The lower limit in the index range(inclusive).
 *             Must not be out of bounds.
 * @param idx2 The upper limit in the index range(exclusive).
 *             Must not be out of bounds.
 *
 * @return true if any clear is set in the range, false otherwise.
 */
[[nodiscard("Must not call functions when you don't use their result.")]]
STD_API STD_Bool STD_CALL CONT_BitmapIsClearAny(const CONT_Bitmap *pBmp,
                                                STD_Size idx1, STD_Size idx2);
/**
 * Checks if all bit are clear between the given range in the given bitmap.
 *
 * @param pBmp The bitmap object to check.
 *             Must be a valid bitmap object.
 * @param idx1 The lower limit in the index range(inclusive).
 *             Must not be out of bounds.
 * @param idx2 The upper limit in the index range(exclusive).
 *             Must not be out of bounds.
 *
 * @return true if all bits are clear in the range, false otherwise.
 */
[[nodiscard("Must not call functions when you don't use their result.")]]
STD_API STD_Bool STD_CALL CONT_BitmapIsClearAll(const CONT_Bitmap *pBmp,
                                                STD_Size idx1, STD_Size idx2);
/**
 * Checks if the first bitmap has all the bits set that the second bitmap also
 * has.
 *
 * @param pBmp1 The first bitmap object to check into.
 *              Must be a valid bitmap object.
 * @param pBmp2 The second bitmap object to check from.
 *              Must be a valid bitmap object.
 *
 * @return true if the first bitmap contains all the set bits second bitmap has,
 * false otherwise.
 */
[[nodiscard("Must not call functions when you don't use their result.")]]
STD_API STD_Bool STD_CALL CONT_BitmapHasAll(const CONT_Bitmap *pBmp1,
                                            const CONT_Bitmap *pBmp2);
/**
 * Checks if the first bitmap has any bits set that the second bitmap also has.
 *
 * @param pBmp1 The first bitmap object to check into.
 *              Must be a valid bitmap object.
 * @param pBmp2 The second bitmap object to check from.
 *              Must be a valid bitmap object.
 *
 * @return true if the first bitmap contains any set bits second bitmap has,
 * false otherwise.
 */
[[nodiscard("Must not call functions when you don't use their result.")]]
STD_API STD_Bool STD_CALL CONT_BitmapHasAny(const CONT_Bitmap *pBmp1,
                                            const CONT_Bitmap *pBmp2);
/**
 * Checks if both the bitmap have exactly the same bit cap, alongside all the
 * set bits are same.
 *
 * @param pBmp1 The first bitmap object to compare.
 *              Must be a valid bitmap object.
 * @param pBmp2 The second bitmap object to compare.
 *              Must be a valid bitmap object.
 *
 * return true if both the bitmaps are exactly equal, false otherwise
 */
[[nodiscard("Must not call functions when you don't use their result.")]]
STD_API STD_Bool STD_CALL CONT_BitmapCmp(const CONT_Bitmap *pBmp1,
                                         const CONT_Bitmap *pBmp2);

/* ---- MODIFICATION FUNCTIONS ---- */

/**
 * Sets the bit at the given index in the bitmap.
 *
 * @param pBmp The bitmap object to set into.
 *             Must be a valid bitmap object.
 * @param idx  The index of the bit to set.
 *             Must not be out of bounds.
 *
 * @note Unchecked Invariant:
 * - Argument validity only exists in debug mode, in release mode invalid
 *   arguments cause undefined behavior.
 */
STD_API void STD_CALL CONT_BitmapSetUnchecked(CONT_Bitmap *pBmp, STD_Size idx);
/**
 * Sets the bit at the given index in the bitmap.
 *
 * @param pBmp The bitmap object to set into.
 *             Must be a valid bitmap object.
 * @param idx  The index of the bit to set.
 *             Must not be out of bounds.
 *
 * @return STD_SUCCESS     on success.
 * @return STD_ERR_INV_ARG if arguments are invalid.
 * @return STD_ERR_OOB     if index out of bounds.
 */
[[nodiscard("This function can fail, please check result for success.")]]
STD_API STD_Rslt STD_CALL CONT_BitmapSet(CONT_Bitmap *pBmp, STD_Size idx);
/**
 * Clears the bit at the given index in the bitmap.
 *
 * @param pBmp The bitmap object to clear from.
 *             Must be a valid bitmap object.
 * @param idx  The index of the bit to set.
 *             Must not be out of bounds.
 *
 * @note Unchecked Invariant:
 * - Argument validity only exists in debug mode, in release mode invalid
 *   arguments cause undefined behavior.
 */
STD_API void STD_CALL CONT_BitmapClearUnchecked(CONT_Bitmap *pBmp,
                                                STD_Size     idx);
/**
 * Clears the bit at the given index in the bitmap.
 *
 * @param pBmp The bitmap object to clear from.
 *             Must be a valid bitmap object.
 * @param idx  The index of the bit to set.
 *             Must not be out of bounds.
 *
 * @return STD_SUCCESS     on success.
 * @return STD_ERR_INV_ARG if arguments are invalid.
 * @return STD_ERR_OOB     if index out of bounds.
 */
[[nodiscard("This function can fail, please check result for success.")]]
STD_API STD_Rslt STD_CALL CONT_BitmapClear(CONT_Bitmap *pBmp, STD_Size idx);
/**
 * Toggles the bit at the given index in the bitmap.
 *
 * @param pBmp The bitmap object to toggle into.
 *             Must be a valid bitmap object.
 * @param idx  The index of the bit to set.
 *             Must not be out of bounds.
 *
 * @note Unchecked Invariant:
 * - Argument validity only exists in debug mode, in release mode invalid
 *   arguments cause undefined behavior.
 */
STD_API void STD_CALL CONT_BitmapToggleUnchecked(CONT_Bitmap *pBmp,
                                                 STD_Size     idx);
/**
 * Toggles the bit at the given index in the bitmap.
 *
 * @param pBmp The bitmap object to toggle into.
 *             Must be a valid bitmap object.
 * @param idx  The index of the bit to set.
 *             Must not be out of bounds.
 *
 * @return STD_SUCCESS     on success.
 * @return STD_ERR_INV_ARG if arguments are invalid.
 * @return STD_ERR_OOB     if index out of bounds.
 */
[[nodiscard("This function can fail, please check result for success.")]]
STD_API STD_Rslt STD_CALL CONT_BitmapToggle(CONT_Bitmap *pBmp, STD_Size idx);

/**
 * Sets all the bits in a given range in the bitmap.
 *
 * @param pBmp The bitmap object to set into.
 *             Must be a valid bitmap object.
 * @param idx1 The lower limit in the index range(inclusive).
 *             Must not be out of bounds.
 * @param idx2 The upper limit in the index range(exclusive).
 *             Must not be out of bounds.
 *
 * @note Unchecked Invariant:
 * - Argument validity only exists in debug mode, in release mode invalid
 *   arguments cause undefined behavior.
 */
STD_API void STD_CALL CONT_BitmapSetManyUnchecked(CONT_Bitmap *pBmp,
                                                  STD_Size idx1, STD_Size idx2);
/**
 * Sets all the bits in a given range in the bitmap.
 *
 * @param pBmp The bitmap object to set into.
 *             Must be a valid bitmap object.
 * @param idx1 The lower limit in the index range(inclusive).
 *             Must not be out of bounds.
 * @param idx2 The upper limit in the index range(exclusive).
 *             Must not be out of bounds.
 *
 * @return STD_SUCCESS     on success.
 * @return STD_ERR_INV_ARG if arguments are invalid.
 * @return STD_ERR_OOB     if index out of bounds.
 */
[[nodiscard("This function can fail, please check result for success.")]]
STD_API STD_Rslt STD_CALL CONT_BitmapSetMany(CONT_Bitmap *pBmp, STD_Size idx1,
                                             STD_Size idx2);
/**
 * Clears all the bits in a given range in the bitmap.
 *
 * @param pBmp The bitmap object to clear from.
 *             Must be a valid bitmap object.
 * @param idx1 The lower limit in the index range(inclusive).
 *             Must not be out of bounds.
 * @param idx2 The upper limit in the index range(exclusive).
 *             Must not be out of bounds.
 *
 * @note Unchecked Invariant:
 * - Argument validity only exists in debug mode, in release mode invalid
 *   arguments cause undefined behavior.
 */
STD_API void STD_CALL CONT_BitmapClearManyUnchecked(CONT_Bitmap *pBmp,
                                                    STD_Size     idx1,
                                                    STD_Size     idx2);
/**
 * Clears all the bits in a given range in the bitmap.
 *
 * @param pBmp The bitmap object to clear from.
 *             Must be a valid bitmap object.
 * @param idx1 The lower limit in the index range(inclusive).
 *             Must not be out of bounds.
 * @param idx2 The upper limit in the index range(exclusive).
 *             Must not be out of bounds.
 *
 * @return STD_SUCCESS     on success.
 * @return STD_ERR_INV_ARG if arguments are invalid.
 * @return STD_ERR_OOB     if index out of bounds.
 */
[[nodiscard("This function can fail, please check result for success.")]]
STD_API STD_Rslt STD_CALL CONT_BitmapClearMany(CONT_Bitmap *pBmp, STD_Size idx1,
                                               STD_Size idx2);
/**
 * Toggles all the bits in a given range in the bitmap.
 *
 * @param pBmp The bitmap object to toggle into.
 *             Must be a valid bitmap object.
 * @param idx1 The lower limit in the index range(inclusive).
 *             Must not be out of bounds.
 * @param idx2 The upper limit in the index range(exclusive).
 *             Must not be out of bounds.
 *
 * @note Unchecked Invariant:
 * - Argument validity only exists in debug mode, in release mode invalid
 *   arguments cause undefined behavior.
 */
STD_API void STD_CALL CONT_BitmapToggleManyUnchecked(CONT_Bitmap *pBmp,
                                                     STD_Size     idx1,
                                                     STD_Size     idx2);
/**
 * Toggles all the bits in a given range in the bitmap.
 *
 * @param pBmp The bitmap object to toggle into.
 *             Must be a valid bitmap object.
 * @param idx1 The lower limit in the index range(inclusive).
 *             Must not be out of bounds.
 * @param idx2 The upper limit in the index range(exclusive).
 *             Must not be out of bounds.
 *
 * @return STD_SUCCESS     on success.
 * @return STD_ERR_INV_ARG if arguments are invalid.
 * @return STD_ERR_OOB     if index out of bounds.
 */
[[nodiscard("This function can fail, please check result for success.")]]
STD_API STD_Rslt STD_CALL CONT_BitmapToggleMany(CONT_Bitmap *pBmp,
                                                STD_Size idx1, STD_Size idx2);

/* ---- BOOLEAN FUNCTIONS ---- */

/**
 * Binary complements the entire bitmap.
 *
 * @param pBmp The bitmap object to complement.
 *             Must be a valid bitmap object.
 *
 * @note Unchecked Invariant:
 * - Argument validity only exists in debug mode, in release mode invalid
 *   arguments cause undefined behavior.
 */
STD_API void STD_CALL CONT_BitmapNotUnchecked(CONT_Bitmap *pBmp);
/**
 * Binary complements the entire bitmap.
 *
 * @param pBmp The bitmap object to complement.
 *             Must be a valid bitmap object.
 *
 * @return STD_SUCCESS     on success.
 * @return STD_ERR_INV_ARG if arguments are invalid.
 */
[[nodiscard("This function can fail, please check result for success.")]]
STD_API STD_Rslt STD_CALL CONT_BitmapNot(CONT_Bitmap *pBmp);

/**
 * Performs a binary AND operation between two bitmaps and stores the result in
 * the first bitmap.
 *
 * @param pBmp1 The first bitmap operand for the AND operation.
 *              Must be a valid bitmap object.
 * @param pBmp2 The second bitmap operand for the AND operation.
 *              Must be a valid bitmap object.
 *
 * @note Unchecked Invariant:
 * - Argument validity only exists in debug mode, in release mode invalid
 *   arguments cause undefined behavior.
 */
STD_API void STD_CALL CONT_BitmapAndUnchecked(CONT_Bitmap       *pBmp1,
                                              const CONT_Bitmap *pBmp2);
/**
 * Performs a binary AND operation between two bitmaps and stores the result in
 * the first bitmap.
 *
 * @param pBmp1 The first bitmap operand for the AND operation.
 *              Must be a valid bitmap object.
 * @param pBmp2 The second bitmap operand for the AND operation.
 *              Must be a valid bitmap object.
 *
 * @return STD_SUCCESS     on success.
 * @return STD_ERR_INV_ARG if arguments are invalid.
 */
[[nodiscard("This function can fail, please check result for success.")]]
STD_API STD_Rslt STD_CALL CONT_BitmapAnd(CONT_Bitmap       *pBmp1,
                                         const CONT_Bitmap *pBmp2);
/**
 * Performs a binary OR operation between two bitmaps and stores the result in
 * the first bitmap.
 *
 * @param pBmp1 The first bitmap operand for the OR operation.
 *              Must be a valid bitmap object.
 * @param pBmp2 The second bitmap operand for the OR operation.
 *              Must be a valid bitmap object.
 *
 * @note Unchecked Invariant:
 * - Argument validity only exists in debug mode, in release mode invalid
 *   arguments cause undefined behavior.
 */
STD_API void STD_CALL CONT_BitmapOrUnchecked(CONT_Bitmap       *pBmp1,
                                             const CONT_Bitmap *pBmp2);
/**
 * Performs a binary OR operation between two bitmaps and stores the result in
 * the first bitmap.
 *
 * @param pBmp1 The first bitmap operand for the OR operation.
 *              Must be a valid bitmap object.
 * @param pBmp2 The second bitmap operand for the OR operation.
 *              Must be a valid bitmap object.
 *
 * @return STD_SUCCESS     on success.
 * @return STD_ERR_INV_ARG if arguments are invalid.
 */
[[nodiscard("This function can fail, please check result for success.")]]
STD_API STD_Rslt STD_CALL CONT_BitmapOr(CONT_Bitmap       *pBmp1,
                                        const CONT_Bitmap *pBmp2);
/**
 * Performs a binary XOR operation between two bitmaps and stores the result in
 * the first bitmap.
 *
 * @param pBmp1 The first bitmap operand for the XOR operation.
 *              Must be a valid bitmap object.
 * @param pBmp2 The second bitmap operand for the XOR operation.
 *              Must be a valid bitmap object.
 *
 * @note Unchecked Invariant:
 * - Argument validity only exists in debug mode, in release mode invalid
 *   arguments cause undefined behavior.
 */
STD_API void STD_CALL CONT_BitmapXorUnchecked(CONT_Bitmap       *pBmp1,
                                              const CONT_Bitmap *pBmp2);
/**
 * Performs a binary XOR operation between two bitmaps and stores the result in
 * the first bitmap.
 *
 * @param pBmp1 The first bitmap operand for the XOR operation.
 *              Must be a valid bitmap object.
 * @param pBmp2 The second bitmap operand for the XOR operation.
 *              Must be a valid bitmap object.
 *
 * @return STD_SUCCESS     on success.
 * @return STD_ERR_INV_ARG if arguments are invalid.
 */
[[nodiscard("This function can fail, please check result for success.")]]
STD_API STD_Rslt STD_CALL CONT_BitmapXor(CONT_Bitmap       *pBmp1,
                                         const CONT_Bitmap *pBmp2);

// /**
//  * Shifts the bits of the bitmap left(towards MSB) by the given offset.
//  *
//  * @param pBmp The bitmap to shift bits into.
//  *             Must be a valid bitmap object.
//  * @param ofs  The offset by which to shift the bits.
//  *
//  * @note Unchecked Invariant:
//  * - Argument validity only exists in debug mode, in release mode invalid
//  *   arguments cause undefined behavior.
//  */
// STD_API void STD_CALL CONT_BitmapLShiftUnchecked(CONT_Bitmap *pBmp,
//                                                  STD_Size     ofs);
// /**
//  * Shifts the bits of the bitmap left(towards MSB) by the given offset.
//  *
//  * @param pBmp The bitmap to shift bits into.
//  *             Must be a valid bitmap object.
//  * @param ofs  The offset by which to shift the bits.
//  *
//  * @return STD_SUCCESS     on success.
//  * @return STD_ERR_INV_ARG if arguments are invalid.
//  */
// [[nodiscard("This function can fail, please check result for success.")]]
// STD_API STD_Rslt STD_CALL CONT_BitmapLShift(CONT_Bitmap *pBmp, STD_Size ofs);
// /**
//  * Shifts the bits of the bitmap right(towards LSB) by the given offset.
//  *
//  * @param pBmp The bitmap to shift bits into.
//  *             Must be a valid bitmap object.
//  * @param ofs  The offset by which to shift the bits.
//  *
//  * @note Unchecked Invariant:
//  * - Argument validity only exists in debug mode, in release mode invalid
//  *   arguments cause undefined behavior.
//  */
// STD_API void STD_CALL CONT_BitmapRShiftUnchecked(CONT_Bitmap *pBmp,
//                                                  STD_Size     ofs);
// /**
//  * Shifts the bits of the bitmap right(towards LSB) by the given offset.
//  *
//  * @param pBmp The bitmap to shift bits into.
//  *             Must be a valid bitmap object.
//  * @param ofs  The offset by which to shift the bits.
//  *
//  * @return STD_SUCCESS     on success.
//  * @return STD_ERR_INV_ARG if arguments are invalid.
//  */
// [[nodiscard("This function can fail, please check result for success.")]]
// STD_API STD_Rslt STD_CALL CONT_BitmapRShift(CONT_Bitmap *pBmp, STD_Size ofs);

#ifdef __cplusplus
}
#endif

#endif
