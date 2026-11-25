#ifndef DEF_CONT_ARRAY_
#define DEF_CONT_ARRAY_

#ifdef __cplusplus
extern "C" {
#endif

#include "Core/Diagnostics/Debug.h"
#include "Core/Std/Platform.h"
#include "Core/Std/Types.h"

typedef struct CONT_Array CONT_Array;

#define CONT_ARRAY_DEFAULT_CAP (16)

/**
 * Checks whether the given array object is valid.
 *
 * @param pArr The array object to check.
 *
 * @return true if valid, false otherwise.
 */
[[nodiscard("Must not call functions when you don't use their result.")]]
STD_API STD_Bool STD_CALL CONT_ArrayIsValid(const CONT_Array *pArr);

#define CONT_ArrayIsValidAssert(pArr)                                          \
    DIAG_AssertMsg(CONT_ArrayIsValid(pArr),                                    \
                   "The given array object is corrupted/invalid.")

/* ---- CONSTRUCTORS/DESTRUCTORS ---- */

/**
 * Creates a new array object with the specified size.
 *
 * @param slot_size The size bytes for the slots inside the array.
 *                  Must be greater than 0.
 * @param cap       The number of slots for the array to have.
 *                  Must be greater than 0 and less than max cap.
 * @param ppArr     Output pointer that receives the created array object.
 *                  Must not equal nullptr.
 *
 * @return STD_SUCCESS     on success.
 * @return STD_ERR_INV_ARG if arguments are invalid.
 * @return STD_ERR_OOM     if memory allocation fails.
 */
[[nodiscard("This function can fail, please check result for success.")]]
STD_API STD_Rslt STD_CALL CONT_ArrayCreate(STD_Size slot_size, STD_Size cap,
                                           CONT_Array **ppArr);
/**
 * Creates a new array object cloning the given array object.
 *
 * @param pTo_clone The array object to clone the new array from.
 *                  Must be a valid array object.
 * @param ppArr     Output pointer that receives the created array object.
 *                  Must not equal nullptr.
 *
 * @return STD_SUCCESS     on success.
 * @return STD_ERR_INV_ARG if arguments are invalid.
 * @return STD_ERR_OOM     if memory allocation fails.
 */
[[nodiscard("This function can fail, please check result for success.")]]
STD_API STD_Rslt STD_CALL CONT_ArrayCreateClone(const CONT_Array *pTo_clone,
                                                CONT_Array      **ppArr);
/**
 * Creates a new array object with the given element array as its members.
 *
 * @param slot_size  The size bytes for the slots inside the array.
 *                   Must be greater than 0.
 * @param len        The number of slots for the array to have.
 *                   Must be greater than 0 and less than max cap.
 * @param pSlots_arr The array pointer of the elements to add to the array.
 *                   Must not equal nullptr.
 *                   Must have size of at-least (slot_size * len) bytes.
 * @param ppArr      Output pointer that receives the created array object.
 *                   Must not equal nullptr.
 *
 * @return STD_SUCCESS     on success.
 * @return STD_ERR_INV_ARG if arguments are invalid.
 * @return STD_ERR_OOM     if memory allocation fails.
 */
[[nodiscard("This function can fail, please check result for success.")]]
STD_API STD_Rslt STD_CALL CONT_ArrayCreateFrom(STD_Size slot_size, STD_Size len,
                                               const void  *pSlot_membs,
                                               CONT_Array **ppArr);
/**
 * Creates a new array object filled completely with the given element.
 *
 * @param slot_size The size bytes for the slots inside the array.
 *                  Must be greater than 0.
 * @param cap       The number of slots for the array to have.
 *                  Must be greater than 0 and less than max cap.
 * @param pFill     The element to fill the array with.
 *                  Must not equal nullptr.
 *                  Must have size of at-least (slot_size) bytes.
 * @param ppArr     Output pointer that receives the created array object.
 *                  Must not equal nullptr.
 *
 * @return STD_SUCCESS     on success.
 * @return STD_ERR_INV_ARG if arguments are invalid.
 * @return STD_ERR_OOM     if memory allocation fails.
 */
[[nodiscard("This function can fail, please check result for success.")]]
STD_API STD_Rslt STD_CALL CONT_ArrayCreateFilled(STD_Size     slot_size,
                                                 STD_Size     cap,
                                                 const void  *pFill,
                                                 CONT_Array **ppArr);
/**
 * Destroys the array and nullifies the object pointer.
 *
 * @param ppArr Pointer to the array object to delete.
 *              Must not equal nullptr.
 *              Its dereference must not equal nullptr.
 *
 * @return STD_SUCCESS     on success.
 * @return STD_ERR_INV_ARG if arguments are invalid.
 */
STD_API STD_Rslt STD_CALL CONT_ArrayDestroy(CONT_Array **ppArr);
/**
 * Resets and clears the array.
 * Make sure all internal allocation of all elements is resolved beforehand.
 *
 * @param pArr The array object to reset.
 *             Must be a valid array object.
 *
 * @return STD_SUCCESS     on success.
 * @return STD_ERR_INV_ARG if arguments are invalid.
 */
[[nodiscard("This function can fail, please check result for success.")]]
STD_API STD_Rslt STD_CALL CONT_ArrayReset(CONT_Array *pArr);

/* ---- ARRAY SIZING ---- */

/**
 * Changes the capacity of the array to the desired size.
 * Truncates elements when shrinking below the current len, make sure the
 * internal allocation of those elements has been handled.
 *
 * @param pArr    The array object to resize.
 *                Must be a valid array object.
 * @param new_cap The new capacity to set the array to.
 *                Must be greater than 0 and less than max cap.
 *
 * @return STD_SUCCESS     on success.
 * @return STD_ERR_INV_ARG if arguments are invalid.
 * @return STD_ERR_OOM     if memory allocation fails.
 */
[[nodiscard("This function can fail, please check result for success.")]]
STD_API STD_Rslt STD_CALL CONT_ArrayChangeSize(CONT_Array *pArr,
                                               STD_Size    new_cap);
/**
 * Grows the array using the default grow strategy.
 *
 * @param pArr The array object to resize.
 *             Must be a valid array object.
 *
 * @return STD_SUCCESS           on success.
 * @return STD_ERR_INV_ARG       if arguments are invalid.
 * @return STD_ERR_RES_EXHAUSTED if the max capacity has already been reached.
 * @return STD_ERR_OOM           if memory allocation fails.
 */
[[nodiscard("This function can fail, please check result for success.")]]
STD_API STD_Rslt STD_CALL CONT_ArrayGrow(CONT_Array *pArr);
/**
 * Shrink fits the array to the current len of the array.  If len of the array
 * is zero, the CONT_ARRAY_DEFAULT_CAP becomes the new cap of the array.
 *
 * @param pArr The array object to resize.
 *             Must be a valid array object.
 *
 * @return STD_SUCCESS     on success.
 * @return STD_ERR_INV_ARG if arguments are invalid.
 * @return STD_ERR_OOM     if memory allocation fails.
 */
[[nodiscard("This function can fail, please check result for success.")]]
STD_API STD_Rslt STD_CALL CONT_ArrayShrinkFit(CONT_Array *pArr);

/* ---- ARRAY ATTRIBUTES ---- */

/**
 * Gets the max cap an array can have of the given slot_size.
 *
 * @param slot_size The slot size to get max cap of.
 *                  Must be greater than 0.
 *
 * @return The max cap that the array can track.
 */
[[nodiscard("Must not call functions when you don't use their result.")]]
STD_API STD_Size STD_CALL CONT_ArrayMaxCap(STD_Size slot_size);
/**
 * Gets the length of the given array.
 *
 * @param pArr The array object to get length of.
 *             Must be a valid array object.
 *
 * @return The length of the array.
 */
[[nodiscard("Must not call functions when you don't use their result.")]]
STD_API STD_Size STD_CALL CONT_ArrayLen(const CONT_Array *pArr);
/**
 * Gets the cap of the given array.
 *
 * @param pArr The array object to get cap of.
 *             Must be a valid array object.
 *
 * @return The cap of the array.
 */
[[nodiscard("Must not call functions when you don't use their result.")]]
STD_API STD_Size STD_CALL CONT_ArrayCap(const CONT_Array *pArr);
/**
 * Gets the slot size of the given array.
 *
 * @param pArr The array object to get slot size of.
 *             Must be a valid array object.
 *
 * @return The slot size of the array.
 */
[[nodiscard("Must not call functions when you don't use their result.")]]
STD_API STD_Size STD_CALL CONT_ArraySlotSize(const CONT_Array *pArr);
/**
 * Gets the empty slots available in the given array.
 *
 * @param pArr The array object to get empty slots of.
 *             Must be a valid array object.
 *
 * @return The number of empty slots of the array.
 */
[[nodiscard("Must not call functions when you don't use their result.")]]
STD_API STD_Size STD_CALL CONT_ArrayEmptySlotCount(const CONT_Array *pArr);
/**
 * Checks if the given array is completely empty.
 *
 * @param pArr The array object to check.
 *             Must be a valid array object.
 *
 * @return true if empty, false otherwise.
 */
[[nodiscard("Must not call functions when you don't use their result.")]]
STD_API STD_Bool STD_CALL CONT_ArrayIsEmpty(const CONT_Array *pArr);
/**
 * Checks if the given array is completely full.
 *
 * @param pArr The array object to check.
 *             Must be a valid array object.
 *
 * @return true if full, false otherwise.
 */
[[nodiscard("Must not call functions when you don't use their result.")]]
STD_API STD_Bool STD_CALL CONT_ArrayIsFull(const CONT_Array *pArr);
/**
 * Checks if the given pointer is a valid array slot pointer.
 *
 * @param pArr  The array object to check into.
 *              Must be a valid pool object.
 * @param pSlot The slot to check.
 *              Must not equal nullptr.
 *
 * @return true if given pointer is a slot in the given array, false otherwise.
 */
[[nodiscard("Must not call functions when you don't use their result.")]]
STD_API STD_Bool STD_CALL CONT_ArrayIsOwnedByArray(const CONT_Array *pArr,
                                                   const void       *pSlot);
/**
 * Gets the raw memory of the given array.
 * If any sizing or element adding/removing operation is performed after this
 * function, the memory pointer returned is not guaranteed to be valid anymore.
 *
 * @param pArr The array object to get raw memory of.
 *             Must be a valid array object.
 *
 * @return The raw memory of the array.
 */
[[nodiscard("Must not call functions when you don't use their result.")]]
STD_API const void *STD_CALL CONT_ArrayRaw(const CONT_Array *pArr);

/* ---- TRIVIAL ---- */

/**
 * Compares if the given arrays contain the same number of same elements in the
 * same order.
 *
 * @param pArr1 The first array object.
 *              Must be a valid array object.
 * @param pArr2 The second array object.
 *              Must be a valid array object.
 *
 * @return true if the arrays are equal, false otherwise.
 */
[[nodiscard("Must not call functions when you don't use their result.")]]
STD_API STD_Bool STD_CALL CONT_ArrayCmp(const CONT_Array *pArr1,
                                        const CONT_Array *pArr2);
/**
 * Iterates over the elements of the array using the given callback.
 *
 * @param pArr       The array object to iterate over.
 *                   Must be a valid array object.
 * @param pCb        The callback to be called on each element of the array.
 *                   Must not equal nullptr.
 * @param pUser_data The caller context required for iteration.
 *
 * @return STD_SUCCESS on success.
 * @return Non-STD_SUCCESS value returned by pCb.
 */
STD_API STD_Rslt STD_CALL CONT_ArrayForeach(const CONT_Array *pArr,
                                            STD_Rslt (*pCb)(void *,
                                                            void *pUser_data),
                                            void *pUser_data);

/* ---- GETTER/SETTER ---- */

/**
 * Gets an element's pointer from the array.
 *
 * @param pArr   The array object to get from.
 *               Must be a valid array object.
 * @param idx    The index of the element to get.
 *               Must not be out of bounds.
 * @param ppDest Output pointer to where the slot pointer will be stored.
 *               Must not equal nullptr.
 *
 * @note Unchecked Invariant:
 * - Argument validity only exists in debug mode, in release mode invalid
 *   arguments cause undefined behavior.
 */
STD_API void STD_CALL CONT_ArrayGetUnchecked(const CONT_Array *pArr,
                                             STD_Size idx, void **ppDest);
/**
 * Gets an element's pointer from the array.
 *
 * @param pArr   The array object to get from.
 *               Must be a valid array object.
 * @param idx    The index of the element to get.
 *               Must not be out of bounds.
 * @param ppDest Output pointer to where the slot pointer will be stored.
 *               Must not equal nullptr.
 *
 * @return STD_SUCCESS     on success.
 * @return STD_ERR_INV_ARG if arguments are invalid.
 * @return STD_ERR_OOB     if index out of bounds.
 */
[[nodiscard("This function can fail, please check result for success.")]]
STD_API STD_Rslt STD_CALL CONT_ArrayGet(const CONT_Array *pArr, STD_Size idx,
                                        void **ppDest);
/**
 * Sets the value of one slot in the array.
 * If slot already has value in it, its internal allocation shall be handled
 * beforehand.
 *
 * @param pArr  The array object to get from.
 *              Must be a valid array object.
 * @param idx   The index of the slot to set.
 *              Must not be out of bounds.
 * @param pData The pointer to the value to set into the array.
 *              Must not equal nullptr.
 *              Must have size of at-least (slot_size) bytes.
 *
 * @note Unchecked Invariant:
 * - Argument validity only exists in debug mode, in release mode invalid
 *   arguments cause undefined behavior.
 */
STD_API void STD_CALL CONT_ArraySetUnchecked(const CONT_Array *pArr,
                                             STD_Size idx, void *pData);
/**
 * Sets the value of one slot in the array.
 * If slot already has value in it, its internal allocation shall be handled
 * beforehand.
 *
 * @param pArr  The array object to get from.
 *              Must be a valid array object.
 * @param idx   The index of the slot to set.
 *              Must not be out of bounds.
 * @param pData The pointer to the value to set into the array.
 *              Must not equal nullptr.
 *              Must have size of at-least (slot_size) bytes.
 *
 * @return STD_SUCCESS     on success.
 * @return STD_ERR_INV_ARG if arguments are invalid.
 * @return STD_ERR_OOB     if index out of bounds.
 */
[[nodiscard("This function can fail, please check result for success.")]]
STD_API STD_Rslt STD_CALL CONT_ArraySet(const CONT_Array *pArr, STD_Size idx,
                                        void *pData);
/**
 * Swaps the values of two slots in the array.
 *
 * @param pArr       The array object to swap into.
 *                   Must be a valid array object.
 * @param idx1       The index of the first slot.
 *                   Must not be out of bounds.
 * @param idx2       The index of the second slot.
 *                   Must not be out of bounds.
 * @param pSwap_bffr Temporary buffer to be used for swapping.
 *                   Must not equal nullptr.
 *                   Must have size of at-least (slot_size) bytes.
 *
 * @note Unchecked Invariant:
 * - Argument validity only exists in debug mode, in release mode invalid
 *   arguments cause undefined behavior.
 */
STD_API void STD_CALL CONT_ArraySwapUnchecked(const CONT_Array *pArr,
                                              STD_Size idx1, STD_Size idx2,
                                              void *pSwap_bffr);
/**
 * Swaps the values of two slots in the array.
 *
 * @param pArr       The array object to swap into.
 *                   Must be a valid array object.
 * @param idx1       The index of the first slot.
 *                   Must not be out of bounds.
 * @param idx2       The index of the second slot.
 *                   Must not be out of bounds.
 * @param pSwap_bffr Temporary buffer to be used for swapping.
 *                   Must not equal nullptr.
 *                   Must have size of at-least (slot_size) bytes.
 *
 * @return STD_SUCCESS     on success.
 * @return STD_ERR_INV_ARG if arguments are invalid.
 * @return STD_ERR_OOB     if index out of bounds.
 */
[[nodiscard("This function can fail, please check result for success.")]]
STD_API STD_Rslt STD_CALL CONT_ArraySwap(const CONT_Array *pArr, STD_Size idx1,
                                         STD_Size idx2, void *pSwap_bffr);

/* ---- ADDING ELEMENTS ---- */

/**
 * Adds an element to the end of the array.
 *
 * @param pArr  The array object to push into.
 *              Must be a valid array object.
 * @param pData The pointer to the value to push into the array.
 *              Must not equal nullptr.
 *              Must have size of at-least (slot_size) bytes.
 *
 * @return STD_SUCCESS           on success.
 * @return STD_ERR_INV_ARG       if arguments are invalid. This is only returned
 *                               if an invalid array is passed to it in release
 *                               mode, and is thrown by the internal call of
 *                               CONT_ArrayGrow(...);
 * @return STD_ERR_RES_EXHAUSTED if the max capacity has already been reached.
 * @return STD_ERR_OOM           if memory allocation fails.
 *
 * @note Unchecked Invariant:
 * - Argument validity only exists in debug mode, in release mode invalid
 *   arguments cause undefined behavior.
 */
[[nodiscard("This function can fail, please check result for success.")]]
STD_API STD_Rslt STD_CALL CONT_ArrayPushUnchecked(CONT_Array *pArr,
                                                  const void *pData);
/**
 * Adds an element to the end of the array.
 *
 * @param pArr  The array object to push into.
 *              Must be a valid array object.
 * @param pData The pointer to the value to push into the array.
 *              Must not equal nullptr.
 *              Must have size of at-least (slot_size) bytes.
 *
 * @return STD_SUCCESS           on success.
 * @return STD_ERR_INV_ARG       if arguments are invalid.
 * @return STD_ERR_RES_EXHAUSTED if the max capacity has already been reached.
 * @return STD_ERR_OOM           if memory allocation fails.
 */
[[nodiscard("This function can fail, please check result for success.")]]
STD_API STD_Rslt STD_CALL CONT_ArrayPush(CONT_Array *pArr, const void *pData);

/**
 * Adds an array of elements to the end of the array.
 *
 * @param pArr      The array object to append into.
 *                  Must be a valid array object.
 * @param pData_arr The pointer to the array to append into the array.
 *                  Must not equal nullptr.
 *                  Must have size of at-least (slot_size * count) bytes.
 * @param count     The number of elements in the given array to append.
 *                  Must be greater than 0.
 *
 * @return STD_SUCCESS           on success.
 * @return STD_ERR_INV_ARG       if arguments are invalid.
 * @return STD_ERR_RES_EXHAUSTED if the array.len + count is greater than max
 *                               cap.
 * @return STD_ERR_OOM           if memory allocation fails.
 *
 * @note Unchecked Invariant:
 * - Argument validity only exists in debug mode, in release mode invalid
 *   arguments cause undefined behavior.
 */
[[nodiscard("This function can fail, please check result for success.")]]
STD_API STD_Rslt STD_CALL CONT_ArrayAppendUnchecked(CONT_Array *pArr,
                                                    const void *pData_arr,
                                                    STD_Size    count);
/**
 * Adds an array of elements to the end of the array.
 *
 * @param pArr      The array object to append into.
 *                  Must be a valid array object.
 * @param pData_arr The pointer to the array to append into the array.
 *                  Must not equal nullptr.
 *                  Must have size of at-least (slot_size * count) bytes.
 * @param count     The number of elements in the given array to append.
 *                  Must be greater than 0.
 *
 * @return STD_SUCCESS           on success.
 * @return STD_ERR_INV_ARG       if arguments are invalid.
 * @return STD_ERR_RES_EXHAUSTED if the array.len + count is greater than max
 *                               cap.
 * @return STD_ERR_OOM           if memory allocation fails.
 */
[[nodiscard("This function can fail, please check result for success.")]]
STD_API STD_Rslt STD_CALL CONT_ArrayAppend(CONT_Array *pArr,
                                           const void *pData_arr,
                                           STD_Size    count);

/**
 * Merges content of the second array after the contents of first array inside
 * the first array.
 *
 * @param pArr1 The array object to extend into.
 *              Must be a valid array object.
 * @param pArr2 The array object to extend with.
 *              Must be a valid array object.
 *
 * @return STD_SUCCESS           on success.
 * @return STD_ERR_INV_ARG       if arguments are invalid.
 * @return STD_ERR_RES_EXHAUSTED if the slots of pArr2 can't be accommodated in
 *                               pArr1.
 * @return STD_ERR_OOM           if memory allocation fails.
 *
 * @note Unchecked Invariant:
 * - Argument validity only exists in debug mode, in release mode invalid
 *   arguments cause undefined behavior.
 */
[[nodiscard("This function can fail, please check result for success.")]]
STD_API STD_Rslt STD_CALL CONT_ArrayExtendUnchecked(CONT_Array       *pArr1,
                                                    const CONT_Array *pArr2);
/**
 * Merges content of the second array after the contents of first array inside
 * the first array.
 *
 * @param pArr1 The array object to extend into.
 *              Must be a valid array object.
 * @param pArr2 The array object to extend with.
 *              Must be a valid array object.
 *
 * @return STD_SUCCESS           on success.
 * @return STD_ERR_INV_ARG       if arguments are invalid.
 * @return STD_ERR_RES_EXHAUSTED if the slots of pArr2 can't be accommodated in
 *                               pArr1.
 * @return STD_ERR_OOM           if memory allocation fails.
 */
[[nodiscard("This function can fail, please check result for success.")]]
STD_API STD_Rslt STD_CALL CONT_ArrayExtend(CONT_Array       *pArr1,
                                           const CONT_Array *pArr2);

/**
 * Inserts an element at a specific index in the array.
 *
 * @param pArr  The array object to insert into
 *              Must be a valid array object.
 * @param idx   The index of where the element is to be inserted.
 *              Must not be out of bounds.
 * @param pData The pointer to the value to insert into the array.
 *              Must not equal nullptr.
 *              Must have size of at-least (slot_size) bytes.
 *
 * @return STD_SUCCESS           on success.
 * @return STD_ERR_INV_ARG       if arguments are invalid.
 * @return STD_ERR_RES_EXHAUSTED if the max capacity has already been reached.
 * @return STD_ERR_OOM           if memory allocation fails.
 *
 * @note Unchecked Invariant:
 * - Argument validity only exists in debug mode, in release mode invalid
 *   arguments cause undefined behavior.
 */
[[nodiscard("This function can fail, please check result for success.")]]
STD_API STD_Rslt STD_CALL CONT_ArrayInsertUnchecked(CONT_Array *pArr,
                                                    STD_Size    idx,
                                                    const void *pData);
/**
 * Inserts an element at a specific index in the array.
 *
 * @param pArr  The array object to insert into
 *              Must be a valid array object.
 * @param idx   The index of where the element is to be inserted.
 *              Must not be out of bounds.
 * @param pData The pointer to the value to insert into the array.
 *              Must not equal nullptr.
 *              Must have size of at-least (slot_size) bytes.
 *
 * @return STD_SUCCESS           on success.
 * @return STD_ERR_INV_ARG       if arguments are invalid.
 * @return STD_ERR_OOB           if index out of bounds.
 * @return STD_ERR_RES_EXHAUSTED if the max capacity has already been reached.
 * @return STD_ERR_OOM           if memory allocation fails.
 */
[[nodiscard("This function can fail, please check result for success.")]]
STD_API STD_Rslt STD_CALL CONT_ArrayInsert(CONT_Array *pArr, STD_Size idx,
                                           const void *pData);
/**
 * Inserts many elements at a specified index in the array.
 *
 * @param pArr      The array object to insert into
 *                  Must be a valid array object.
 * @param idx       The index of where the element is to be inserted.
 *                  Must not be out of bounds.
 * @param pData_arr The pointer to the array to insert into the array.
 *                  Must not equal nullptr.
 *                  Must have size of at-least (slot_size * count) bytes.
 * @param count     The number of elements to insert.
 *                  Must be greater than 0.
 *
 * @return STD_SUCCESS           on success.
 * @return STD_ERR_INV_ARG       if arguments are invalid.
 * @return STD_ERR_RES_EXHAUSTED if the max capacity has already been reached.
 * @return STD_ERR_OOM           if memory allocation fails.
 *
 * @note Unchecked Invariant:
 * - Argument validity only exists in debug mode, in release mode invalid
 *   arguments cause undefined behavior.
 */
[[nodiscard("This function can fail, please check result for success.")]]
STD_API STD_Rslt STD_CALL CONT_ArrayInsertManyUnchecked(CONT_Array *pArr,
                                                        STD_Size    idx,
                                                        const void *pData_arr,
                                                        STD_Size    count);
/**
 * Inserts many elements at a specified index in the array.
 *
 * @param pArr      The array object to insert into
 *                  Must be a valid array object.
 * @param idx       The index of where the element is to be inserted.
 *                  Must not be out of bounds.
 * @param pData_arr The pointer to the array to insert into the array.
 *                  Must not equal nullptr.
 *                  Must have size of at-least (slot_size * count) bytes.
 * @param count     The number of elements to insert.
 *                  Must be greater than 0.
 *
 * @return STD_SUCCESS           on success.
 * @return STD_ERR_INV_ARG       if arguments are invalid.
 * @return STD_ERR_OOB           if index out of bounds.
 * @return STD_ERR_RES_EXHAUSTED if the max capacity has already been reached.
 * @return STD_ERR_OOM           if memory allocation fails.
 */
[[nodiscard("This function can fail, please check result for success.")]]
STD_API STD_Rslt STD_CALL CONT_ArrayInsertMany(CONT_Array *pArr, STD_Size idx,
                                               const void *pData_arr,
                                               STD_Size    count);

/**
 * Makes sure at-least one slot is available in the array, and give its slot
 * pointer.
 *
 * @param pArr    The array object to reserve into.
 *                Must be a valid array object.
 * @param ppSlots Output pointer to where the reserved slot's pointer will be
 *                stored.
 *                Must not equal nullptr;
 *
 * @return STD_SUCCESS           on success.
 * @return STD_ERR_INV_ARG       if arguments are invalid. This is only returned
 *                               if an invalid array is passed to it in release
 *                               mode, and is thrown by the internal call of
 *                               CONT_ArrayGrow(...);
 * @return STD_ERR_RES_EXHAUSTED if the max capacity has already been reached.
 * @return STD_ERR_OOM           if memory allocation fails.
 *
 * @note Unchecked Invariant:
 * - Argument validity only exists in debug mode, in release mode invalid
 *   arguments cause undefined behavior.
 */
[[nodiscard("This function can fail, please check result for success.")]]
STD_API STD_Rslt STD_CALL CONT_ArrayEmpalceUnchecked(CONT_Array *pArr,
                                                     void      **ppSlot);
/**
 * Makes sure at-least one slot is available in the array, and give its slot
 * pointer.
 *
 * @param pArr    The array object to reserve into.
 *                Must be a valid array object.
 * @param ppSlots Output pointer to where the reserved slot's pointer will be
 *                stored.
 *                Must not equal nullptr;
 *
 * @return STD_SUCCESS           on success.
 * @return STD_ERR_INV_ARG       if arguments are invalid. This is only returned
 *                               if an invalid array is passed to it in release
 *                               mode, and is thrown by the internal call of
 *                               CONT_ArrayGrow(...);
 * @return STD_ERR_RES_EXHAUSTED if the max capacity has already been reached.
 * @return STD_ERR_OOM           if memory allocation fails.
 */
[[nodiscard("This function can fail, please check result for success.")]]
STD_API STD_Rslt STD_CALL CONT_ArrayEmpalce(CONT_Array *pArr, void **ppSlot);

/**
 * Makes sure at-least (count) slots are available in the array.
 *
 * @param pArr  The array object to reserve into.
 *              Must be a valid array object.
 * @param count The number of slots to reserve.
 *              Must be greater than 0.
 *
 * @return STD_SUCCESS           on success.
 * @return STD_ERR_INV_ARG       if arguments are invalid.
 * @return STD_ERR_RES_EXHAUSTED if max cap - array's cap is less than count.
 * @return STD_ERR_OOM           if memory allocation fails.
 *
 * @note Unchecked Invariant:
 * - Argument validity only exists in debug mode, in release mode invalid
 *   arguments cause undefined behavior.
 */
[[nodiscard("This function can fail, please check result for success.")]]
STD_API STD_Rslt STD_CALL CONT_ArrayReserveUnchecked(CONT_Array *pArr,
                                                     STD_Size    count);
/**
 * Makes sure at-least (count) slots are available in the array.
 *
 * @param pArr  The array object to reserve into.
 *              Must be a valid array object.
 * @param count The number of slots to reserve.
 *              Must be greater than 0.
 *
 * @return STD_SUCCESS           on success.
 * @return STD_ERR_INV_ARG       if arguments are invalid.
 * @return STD_ERR_RES_EXHAUSTED if max cap - array's cap is less than count.
 * @return STD_ERR_OOM           if memory allocation fails.
 */
[[nodiscard("This function can fail, please check result for success.")]]
STD_API STD_Rslt STD_CALL CONT_ArrayReserve(CONT_Array *pArr, STD_Size count);

/* ---- REMOVING ELEMENTS ---- */

/**
 * Removes the last element from the array.
 * If the last element contain internal allocation, it needs to be handled
 * beforehand.
 *
 * @param pArr The array object to pop from.
 *             Must be a valid array object.
 *
 * @return STD_SUCCESS           on success.
 * @return STD_ERR_RES_EXHAUSTED if no elements remain in the array.
 *
 * @note Unchecked Invariant:
 * - Argument validity only exists in debug mode, in release mode invalid
 *   arguments cause undefined behavior.
 */
[[nodiscard("This function can fail, please check result for success.")]]
STD_API STD_Rslt STD_CALL CONT_ArrayPopUnchecked(CONT_Array *pArr);
/**
 * Removes the last element from the array.
 * If the last element contain internal allocation, it needs to be handled
 * beforehand.
 *
 * @param pArr The array object to pop from.
 *             Must be a valid array object.
 *
 * @return STD_SUCCESS           on success.
 * @return STD_ERR_INV_ARG       if arguments are invalid.
 * @return STD_ERR_RES_EXHAUSTED if no elements remain in the array.
 */
[[nodiscard("This function can fail, please check result for success.")]]
STD_API STD_Rslt STD_CALL CONT_ArrayPop(CONT_Array *pArr);

/**
 * Removes a specific element from the array.
 * If the last element contain internal allocation, it needs to be handled
 * beforehand.
 *
 * @param pArr The array object to pop from.
 *             Must be a valid array object.
 * @param idx  The index of the element to remove.
 *             Must not be out of bounds.
 *
 * @note Unchecked Invariant:
 * - Argument validity only exists in debug mode, in release mode invalid
 *   arguments cause undefined behavior.
 */
STD_API void STD_CALL CONT_ArrayRemoveUnchecked(CONT_Array *pArr, STD_Size idx);
/**
 * Removes a specific element from the array.
 * If the element contain internal allocation, it needs to be handled
 * beforehand.
 *
 * @param pArr The array object to pop from.
 *             Must be a valid array object.
 * @param idx  The index of the element to remove.
 *             Must not be out of bounds.
 *
 * @return STD_SUCCESS     on success.
 * @return STD_ERR_INV_ARG if arguments are invalid.
 * @return STD_ERR_OOB     if index out of bounds.
 */
[[nodiscard("This function can fail, please check result for success.")]]
STD_API STD_Rslt STD_CALL CONT_ArrayRemove(CONT_Array *pArr, STD_Size idx);

/**
 * Removes many elements from a specified index in the array.
 * If the elements contain internal allocation, it needs to be handled
 * beforehand.
 *
 * @param pArr  The array object to pop from.
 *              Must be a valid array object.
 * @param idx   The index of the element to remove.
 *              Must not be out of bounds.
 * @param count The number of elements to remove.
 *              Must be greater than 0, and less than the len of the array.
 *
 * @note Unchecked Invariant:
 * - Argument validity only exists in debug mode, in release mode invalid
 *   arguments cause undefined behavior.
 */
STD_API void STD_CALL CONT_ArrayRemoveManyUnchecked(CONT_Array *pArr,
                                                    STD_Size    idx,
                                                    STD_Size    count);
/**
 * Removes many elements from a specified index in the array.
 * If the elements contain internal allocation, it needs to be handled
 * beforehand.
 *
 * @param pArr  The array object to pop from.
 *              Must be a valid array object.
 * @param idx   The index of the element to remove.
 *              Must not be out of bounds.
 * @param count The number of elements to remove.
 *              Must be greater than 0, and less than the len of the array.
 *
 * @return STD_SUCCESS     on success.
 * @return STD_ERR_INV_ARG if arguments are invalid.
 * @return STD_ERR_OOB     if index out of bounds.
 */
[[nodiscard("This function can fail, please check result for success.")]]
STD_API STD_Rslt STD_CALL CONT_ArrayRemoveMany(CONT_Array *pArr, STD_Size idx,
                                               STD_Size count);

#ifdef __cplusplus
}
#endif

#endif
