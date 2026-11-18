#ifndef DEF_ALOC_ARENA_
#define DEF_ALOC_ARENA_

#ifdef __cplusplus
extern "C" {
#endif

#include "Core/Diagnostics/Debug.h"
#include "Core/Std/Platform.h"
#include "Core/Std/Types.h"

typedef struct ALOC_Pool ALOC_Pool;

/**
 * Checks whether the given pool object is valid.
 *
 * @param pPool The pool object to check.
 *
 * @return true if valid, false otherwise.
 */
[[nodiscard("Must not call functions when you don't use their result.")]]
STD_API STD_Bool STD_CALL ALOC_PoolIsValid(const ALOC_Pool *pPool);

#define ALOC_PoolIsValidAssert(pPool)                                          \
    DIAG_AssertMsg(ALOC_PoolIsValid(pPool),                                    \
                   "The given pool object is corrupted/invalid.")

/**
 * Creates a new pool object with the specified size.
 *
 * @param slot_size The size bytes for the slots inside the pool.
 *                  Must be greater than 0.
 * @param cap       The number of slots for the pool to have.
 *                  Must be greater than 0 and less than max cap.
 * @param ppPool    Output pointer that receives the created pool object.
 *                  Must not equal nullptr.
 *
 * @return STD_SUCCESS     on success.
 * @return STD_ERR_INV_ARG if arguments are invalid.
 * @return STD_ERR_OOM     if memory allocation fails.
 */
[[nodiscard("This function can fail, please check result for success.")]]
STD_API STD_Rslt STD_CALL ALOC_PoolCreate(STD_Size slot_size, STD_Size cap,
                                          ALOC_Pool **ppPool);
/**
 * Creates a new pool object with the specified size.
 *
 * @param slot_size The size bytes for the slots inside the pool.
 *                  Must be greater than 0.
 * @param cap       The number of slots for the pool to have.
 *                  Must be greater than 0 and less than max cap.
 * @param align     The alignment the pool slots shall follow.
 *                  Must be a non zero power of two.
 * @param ppPool    Output pointer that receives the created pool object.
 *                  Must not equal nullptr.
 *
 * @return STD_SUCCESS     on success.
 * @return STD_ERR_INV_ARG if arguments are invalid.
 * @return STD_ERR_OOM     if memory allocation fails.
 */
[[nodiscard("This function can fail, please check result for success.")]]
STD_API STD_Rslt STD_CALL ALOC_PoolCreateAligned(STD_Size slot_size,
                                                 STD_Size cap, STD_Size align,
                                                 ALOC_Pool **ppPool);
/**
 * Destroys the pool and nullifies the object pointer.
 *
 * @param ppPool Pointer to the pool object to delete.
 *               Must not equal nullptr.
 *               Its dereference must not equal nullptr.
 *
 * @return STD_SUCCESS     on success.
 * @return STD_ERR_INV_ARG if arguments are invalid.
 */
STD_API STD_Rslt STD_CALL ALOC_PoolDestroy(ALOC_Pool **ppPool);
/**
 * Resets and invalidates all allocations inside the pool all at once.
 *
 * @param pPool The pool object to reset.
 *              Must be a valid pool object.
 *
 * @return STD_SUCCESS     on success.
 * @return STD_ERR_INV_ARG if arguments are invalid.
 */
[[nodiscard("This function can fail, please check result for success.")]]
STD_API STD_Rslt STD_CALL ALOC_PoolReset(ALOC_Pool *pPool);

/**
 * Allocates a single slot from the pool.
 *
 * @param pPool  The pool object to allocate from.
 *               Must be a valid pool object.
 * @param zeroed If true the allocated memory block is zero initialized.
 * @param ppDest Output pointer to where the allocated memory pointer will be
 *               written to.
 *               Must not equal nullptr.
 *
 * @return STD_SUCCESS           on success.
 * @return STD_ERR_INV_ARG       if arguments are invalid.
 * @return STD_ERR_RES_EXHAUSTED if no free slots remain in the pool.
 */
[[nodiscard("This function can fail, please check result for success.")]]
STD_API STD_Rslt STD_CALL ALOC_PoolAlloc(ALOC_Pool *pPool, STD_Bool zeroed,
                                         void **ppDest);
/**
 * Allocates multiple slots from the pool.
 *
 * @param pPool  The pool object to allocate from.
 *               Must be a valid pool object.
 * @param count  The number of slots to allocate.
 *               Must not equal zero.
 * @param zeroed If true the allocated memory block is zero initialized.
 * @param ppDest Output pointer to where the allocated memory pointer will be
 *               written to.
 *               Must not equal nullptr.
 *
 * @return STD_SUCCESS           on success.
 * @return STD_ERR_INV_ARG       if arguments are invalid.
 * @return STD_ERR_RES_EXHAUSTED if count is greater than free slots in the
 *                               pool.
 */
[[nodiscard("This function can fail, please check result for success.")]]
STD_API STD_Rslt STD_CALL ALOC_PoolAllocMany(ALOC_Pool *pPool, STD_Size count,
                                             STD_Bool zeroed,
                                             void   **ppDest_arr);
/**
 * Frees the given slot in the pool.
 *
 * @param pPool  The pool object to allocate from.
 *               Must be a valid pool object.
 * @param pBlock The slot to be free.
 *               Must not equal nullptr and be a valid slot ptr inside the pool.
 *
 * @return STD_SUCCESS     on success.
 * @return STD_ERR_INV_ARG if arguments are invalid.
 */
[[nodiscard("This function can fail, please check result for success.")]]
STD_API STD_Rslt STD_CALL ALOC_PoolFree(ALOC_Pool *pPool, void *pBlock);

/**
 * Gets the max cap a pool can have of the given slot_size.
 *
 * @param slot_size The slot size to get max cap of.
 *                  Must be greater than 0.
 *
 * @return The max cap that the pool can track.
 */
[[nodiscard("Must not call functions when you don't use their result.")]]
STD_API STD_Size STD_CALL ALOC_PoolMaxCap(STD_Size slot_size);
/**
 * Gets the slot size of the given pool.
 *
 * @param pPool The pool object to get slot size of.
 *              Must be a valid pool object.
 *
 * @return The slot size of the pool.
 */
[[nodiscard("Must not call functions when you don't use their result.")]]
STD_API STD_Size STD_CALL ALOC_PoolGetSlotSize(const ALOC_Pool *pPool);
/**
 * Gets the cap of the given pool.
 *
 * @param pPool The pool object to get cap of.
 *              Must be a valid pool object.
 *
 * @return The cap of the pool.
 */
[[nodiscard("Must not call functions when you don't use their result.")]]
STD_API STD_Size STD_CALL ALOC_PoolGetCap(const ALOC_Pool *pPool);
/**
 * Gets the free slots available in the given pool.
 *
 * @param pPool The pool object to get free slots of.
 *              Must be a valid pool object.
 *
 * @return The number of free slots of the pool.
 */
[[nodiscard("Must not call functions when you don't use their result.")]]
STD_API STD_Size STD_CALL ALOC_PoolGetFreeSlotCount(const ALOC_Pool *pPool);
/**
 * Gets the occupied slots available in the given pool.
 *
 * @param pPool The pool object to get occupied slots of.
 *              Must be a valid pool object.
 *
 * @return The number of occupied slots of the pool.
 */
[[nodiscard("Must not call functions when you don't use their result.")]]
STD_API STD_Size STD_CALL ALOC_PoolGetOccupiedSlotCount(const ALOC_Pool *pPool);
/**
 * Checks if the given pool is completely empty.
 *
 * @param pPool The pool object to check.
 *              Must be a valid pool object.
 *
 * @return true if empty, false otherwise.
 */
[[nodiscard("Must not call functions when you don't use their result.")]]
STD_API STD_Bool STD_CALL ALOC_PoolIsEmpty(const ALOC_Pool *pPool);
/**
 * Checks if the given pool is completely full.
 *
 * @param pPool The pool object to check.
 *              Must be a valid pool object.
 *
 * @return true if full, false otherwise.
 */
[[nodiscard("Must not call functions when you don't use their result.")]]
STD_API STD_Bool STD_CALL ALOC_PoolIsFull(const ALOC_Pool *pPool);
/**
 * Checks if the given pointer is a valid pool slot pointer.
 *
 * @param pPool  The pool object to check into.
 *               Must be a valid pool object.
 * @param pBlock The slot to check.
 *
 * @return true if given pointer is a slot in the give pool, false otherwise.
 */
[[nodiscard("Must not call functions when you don't use their result.")]]
STD_API STD_Bool STD_CALL ALOC_PoolIsOwnedByPool(const ALOC_Pool *pPool,
                                                 const void      *pBlock);

#ifdef __cplusplus
}
#endif

#endif
