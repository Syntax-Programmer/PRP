#ifndef DEF_CONT_REGION_
#define DEF_CONT_REGION_

#ifdef __cplusplus
extern "C" {
#endif

#include "Core/Diagnostics/Debug.h"
#include "Core/Std/Platform.h"
#include "Core/Std/Types.h"

typedef struct CONT_Region CONT_Region;

#define CONT_REGION_MAX_SIZE     (STD_SIZE_MAX)
#define CONT_REGION_DEFAULT_SIZE (128)

/**
 * Checks whether the given array object is valid.
 *
 * @param pArr The array object to check.
 *
 * @return true if valid, false otherwise.
 */
[[nodiscard("Must not call functions when you don't use their result.")]]
STD_API STD_Bool STD_CALL CONT_RegionIsValid(const CONT_Region *pRegion);

#define CONT_RegionIsValidAssert(pRegion)                                      \
    DIAG_AssertMsg(CONT_RegionIsValid(pRegion),                                \
                   "The given region object is corrupted/invalid.")

/* ---- CONSTRUCTORS/DESTRUCTORS ---- */

/**
 * Creates a new region object with the specified size.
 *
 * @param size     The size bytes of the region.
 *                 Must be greater than 0.
 * @param ppRegion Output pointer that receives the created region object.
 *                 Must not equal nullptr.
 *
 * @return STD_SUCCESS     on success.
 * @return STD_ERR_INV_ARG if arguments are invalid.
 * @return STD_ERR_OOM     if memory allocation fails.
 */
[[nodiscard("This function can fail, please check result for success.")]]
STD_API STD_Rslt STD_CALL CONT_RegionCreate(STD_Size      size,
                                            CONT_Region **ppRegion);
/**
 * Creates a new region object cloning the given region object.
 *
 * @param pTo_clone The region object to clone the new region from.
 *                  Must be a valid region object.
 * @param ppRegion  Output pointer that receives the created region object.
 *                  Must not equal nullptr.
 *
 * @return STD_SUCCESS     on success.
 * @return STD_ERR_INV_ARG if arguments are invalid.
 * @return STD_ERR_OOM     if memory allocation fails.
 */
[[nodiscard("This function can fail, please check result for success.")]]
STD_API STD_Rslt STD_CALL CONT_RegionCreateClone(const CONT_Region *pTo_clone,
                                                 CONT_Region      **ppRegion);
/**
 * Destroys the region and nullifies the object pointer.
 * Make sure if the memory inside the region has pointer to allocation, it is
 * cleared beforehand, or we get memory leaks.
 *
 * @param ppRegion Pointer to the region object to delete.
 *                 Must not equal nullptr.
 *                 Its dereference must not equal nullptr.
 *
 * @return STD_SUCCESS     on success.
 * @return STD_ERR_INV_ARG if arguments are invalid.
 */
STD_API STD_Rslt STD_CALL CONT_RegionDestroy(CONT_Region **ppRegion);
/**
 * Resets and clears the region.
 * Make sure if the memory inside the region has pointer to allocation, it is
 * cleared beforehand, or we get memory leaks.
 *
 * @param pRegion The region object to reset.
 *                Must be a valid region object.
 *
 * @return STD_SUCCESS     on success.
 * @return STD_ERR_INV_ARG if arguments are invalid.
 */
[[nodiscard("This function can fail, please check result for success.")]]
STD_API STD_Rslt STD_CALL CONT_RegionReset(CONT_Region *pRegion);

/* ---- REGION SIZING ---- */

/**
 * Changes the capacity of the region to the desired size.
 * Truncates memory when shrinking below the current size, make sure if the
 * memory inside the region has pointer to allocation, it is cleared beforehand,
 * or we get memory leaks.
 *
 * @param pRegion  The region object to resize.
 *                 Must be a valid region object.
 * @param new_size The new size to set the region of.
 *                 Must be greater than 0.
 *
 * @return STD_SUCCESS     on success.
 * @return STD_ERR_INV_ARG if arguments are invalid.
 * @return STD_ERR_OOM     if memory allocation fails.
 */
[[nodiscard("This function can fail, please check result for success.")]]
STD_API STD_Rslt STD_CALL CONT_RegionChangeSize(CONT_Region *pRegion,
                                                STD_Size     new_size);
/**
 * Makes sure at least (size) bytes are available in the region from the given
 * (ofs) in the region.
 *
 * @param pRegion The region object to resize into.
 *                Must be a valid region object.
 * @param ofs     The offset from where to reserve.
 *                Must not be out of bounds.
 * @param size    The bytes to ensure exist in the region.
 *                Must be greater than 0.
 *
 * @return STD_SUCCESS           on success.
 * @return STD_ERR_INV_ARG       if arguments are invalid.
 * @return STD_ERR_RES_EXHAUSTED if adding (size) bytes more to the region
 *                               exceeds max cap.
 * @return STD_ERR_OOM           if memory allocation fails.
 * @return STD_ERR_OOB           if offset out of bounds.
 *
 */
[[nodiscard("This function can fail, please check result for success.")]]
STD_API STD_Rslt STD_CALL CONT_RegionReserveMemory(CONT_Region *pRegion,
                                                   STD_Size ofs, STD_Size size);

/* ---- REGION ATTRIBUTES ---- */

/**
 * Gets the size of the given region.
 *
 * @param pRegion The region object to resize into.
 *                Must be a valid region object.
 *
 * @return The size of the region.
 */
[[nodiscard("Must not call functions when you don't use their result.")]]
STD_API STD_Size STD_CALL CONT_RegionSize(const CONT_Region *pRegion);
/**
 * Gets the raw memory of the given region.
 * If any sizing operation is performed after this  function, the memory pointer
 * returned is not guaranteed to be valid anymore.
 *
 * @param pRegion The region object to get raw memory of.
 *                Must be a valid region object.
 *
 * @return The raw memory of the region.
 */
[[nodiscard("Must not call functions when you don't use their result.")]]
STD_API const void *STD_CALL CONT_RegionRaw(const CONT_Region *pRegion);
/**
 * Checks if the given pointer is a valid region pointer.
 *
 * @param pRegion The region object to get raw memory of.
 *                Must be a valid region object.
 * @param pMem    The memory pointer to check.
 *                Must not equal nullptr.
 *
 * @return true if given pointer belongs to the given region, false otherwise.
 */
[[nodiscard("Must not call functions when you don't use their result.")]]
STD_API STD_Bool STD_CALL CONT_RegionIsOwnedByRegion(const CONT_Region *pRegion,
                                                     const void        *pMem);

/* ---- TRIVIAL ---- */

/**
 * Compares if the given regions contain exactly same memory and size.
 *
 * @param pRegion1 The first region object.
 *                 Must be a valid region object.
 * @param pRegion1 The second region object.
 *                 Must be a valid region object.
 *
 * @return true if the arrays are equal, false otherwise.
 */
[[nodiscard("Must not call functions when you don't use their result.")]]
STD_API STD_Bool STD_CALL CONT_RegionCmp(const CONT_Region *pRegion1,
                                         const CONT_Region *pRegion2);

/* ---- MODIFICATION FUNCTIONS ---- */

/**
 * Gets the memory pointer of the given ofs in the region.
 *
 * @param pRegion The region object to get from.
 *                Must be a valid region object.
 * @param ofs     The offset of the memory location to get.
 *                Must not be out of bounds.
 * @param ppDest  Output pointer to where the slot pointer will be stored.
 *                Must not equal nullptr.
 *
 * @return STD_SUCCESS     on success.
 * @return STD_ERR_INV_ARG if arguments are invalid.
 * @return STD_ERR_OOB     if offset out of bounds.
 */
[[nodiscard("This function can fail, please check result for success.")]]
STD_API STD_Rslt STD_CALL CONT_RegionGet(const CONT_Region *pRegion,
                                         STD_Size ofs, void **ppDest);

/**
 * Sets the value of a memory region at the given ofs in the region.
 *
 * @param pRegion The region object to get from.
 *                Must be a valid region object.
 * @param ofs     The offset of the memory location to get.
 *                Must not be out of bounds.
 * @param size    The size in bytes for the region to set.
 *                Must be greater than 0.
 * @param pData   The pointer to the value to set into the region.
 *                Must not equal nullptr.
 *                Must have size of at-least (size) bytes.
 *
 * @return STD_SUCCESS     on success.
 * @return STD_ERR_INV_ARG if arguments are invalid.
 * @return STD_ERR_OOB     if offset out of bounds.
 */
[[nodiscard("This function can fail, please check result for success.")]]
STD_API STD_Rslt STD_CALL CONT_RegionSet(const CONT_Region *pRegion,
                                         STD_Size ofs, STD_Size size,
                                         void *pData);

/**
 * Copies a region of memory from one region object to another.
 *
 * @param pRegion1 The first region object to copy from.
 *                 Must be a valid region object.
 * @param pRegion2 The second region object to copy to.
 *                 Must be a valid region object.
 * @param ofs1     The offset of the region to copy from in fist region object.
 *                 Must not be out of bounds.
 * @param ofs2     The offset of the region to copy to in second region object.
 *                 Must not be out of bounds.
 * @param size     The size in bytes of the region to copy.
 *                 Must be greater than 0.
 *
 * @return STD_SUCCESS           on success.
 * @return STD_ERR_RES_EXHAUSTED if either of the region objects don't contain
 *                               at least (size) bytes from their respective
 *                               offsets.
 *
 * @note Unchecked Invariant:
 * - Argument validity only exists in debug mode, in release mode invalid
 *   arguments cause undefined behavior.
 */
[[nodiscard("This function can fail, please check result for success.")]]
STD_API STD_Rslt STD_CALL CONT_RegionCopyUnchecked(const CONT_Region *pRegion1,
                                                   CONT_Region       *pRegion2,
                                                   STD_Size ofs1, STD_Size ofs2,
                                                   STD_Size size);
/**
 * Copies a region of memory from one region object to another.
 *
 * @param pRegion1 The first region object to copy from.
 *                 Must be a valid region object.
 * @param pRegion2 The second region object to copy to.
 *                 Must be a valid region object.
 * @param ofs1     The offset of the region to copy from in first region object.
 *                 Must not be out of bounds.
 * @param ofs2     The offset of the region to copy to in second region object.
 *                 Must not be out of bounds.
 * @param size     The size in bytes of the region to copy.
 *                 Must be greater than 0.
 *
 * @return STD_SUCCESS           on success.
 * @return STD_ERR_INV_ARG       if arguments are invalid.
 * @return STD_ERR_RES_EXHAUSTED if either of the region objects don't contain
 *                               at least (size) bytes from their respective
 *                               offsets.
 * @return STD_ERR_OOB           if offset out of bounds.
 */
[[nodiscard("This function can fail, please check result for success.")]]
STD_API STD_Rslt STD_CALL CONT_RegionCopy(const CONT_Region *pRegion1,
                                          CONT_Region *pRegion2, STD_Size ofs1,
                                          STD_Size ofs2, STD_Size size);

/**
 * Fills a region of memory with the given byte value.
 *
 * @param pRegion The region object to fill.
 *                Must be a valid region object.
 * @param ofs     The offset of the region to fill.
 *                Must not be out of bounds.
 * @param size    The size in bytes of the region to fill.
 *                Must be greater than 0.
 * @param byte    The value to fill into the region.
 *
 * @return STD_SUCCESS           on success.
 * @return STD_ERR_RES_EXHAUSTED if region object don't contain at least (size)
 *                               bytes from their respective offsets.
 *
 * @note Unchecked Invariant:
 * - Argument validity only exists in debug mode, in release mode invalid
 *   arguments cause undefined behavior.
 */
[[nodiscard("This function can fail, please check result for success.")]]
STD_API STD_Rslt STD_CALL CONT_RegionFillUnchecked(const CONT_Region *pRegion,
                                                   STD_Size ofs, STD_Size size,
                                                   STD_Byte byte);
/**
 * Fills a region of memory with the given byte value.
 *
 * @param pRegion The region object to fill.
 *                Must be a valid region object.
 * @param ofs     The offset of the region to fill.
 *                Must not be out of bounds.
 * @param size    The size in bytes of the region to fill.
 *                Must be greater than 0.
 * @param byte    The value to fill into the region.
 *
 * @return STD_SUCCESS           on success.
 * @return STD_ERR_INV_ARG       if arguments are invalid.
 * @return STD_ERR_RES_EXHAUSTED if region object don't contain at least (size)
 *                               bytes from their respective offsets.
 * @return STD_ERR_OOB           if offset out of bounds.
 */
[[nodiscard("This function can fail, please check result for success.")]]
STD_API STD_Rslt STD_CALL CONT_RegionFill(const CONT_Region *pRegion,
                                          STD_Size ofs, STD_Size size,
                                          STD_Byte byte);

/**
 * Swaps the value of two regions.
 *
 * @param pRegion    The region object to fill.
 *                   Must be a valid region object.
 * @param ofs1       The first offset of the region to swap from.
 *                   Must not be out of bounds.
 * @param ofs2       The second offset of the region to swap from.
 *                   Must not be out of bounds.
 * @param size       The size in bytes of the region to swap.
 *                   Must be greater than 0.
 * @param pSwap_bffr Temporary buffer to be used for swapping.
 *                   Must not equal nullptr.
 *                   Must have size of at-least (size) bytes.
 *
 * @return STD_SUCCESS           on success.
 * @return STD_ERR_RES_EXHAUSTED if region objects don't contain at least (size)
 *                               bytes from their respective offsets.
 *
 * @note Unchecked Invariant:
 * - Argument validity only exists in debug mode, in release mode invalid
 *   arguments cause undefined behavior.
 */
[[nodiscard("This function can fail, please check result for success.")]]
STD_API STD_Rslt STD_CALL CONT_RegionSwapUnchecked(const CONT_Region *pRegion,
                                                   STD_Size ofs1, STD_Size ofs2,
                                                   STD_Size size,
                                                   void    *pSwap_bffr);
/**
 * Swaps the value of two regions.
 *
 * @param pRegion    The region object to fill.
 *                   Must be a valid region object.
 * @param ofs1       The first offset of the region to swap from.
 *                   Must not be out of bounds.
 * @param ofs2       The second offset of the region to swap from.
 *                   Must not be out of bounds.
 * @param size       The size in bytes of the region to swap.
 *                   Must be greater than 0.
 * @param pSwap_bffr Temporary buffer to be used for swapping.
 *                   Must not equal nullptr.
 *                   Must have size of at-least (size) bytes.
 *
 * @return STD_SUCCESS           on success.
 * @return STD_ERR_INV_ARG       if arguments are invalid.
 * @return STD_ERR_RES_EXHAUSTED if region object don't contain at least (size)
 *                               bytes from their respective offsets.
 * @return STD_ERR_OOB           if offset out of bounds.
 */
[[nodiscard("This function can fail, please check result for success.")]]
STD_API STD_Rslt STD_CALL CONT_RegionSwap(const CONT_Region *pRegion,
                                          STD_Size ofs1, STD_Size ofs2,
                                          STD_Size size, void *pSwap_bffr);

/**
 * Merges the second region object into the first.
 *
 * @param pRegion1 The first region object to copy from.
 *                 Must be a valid region object.
 * @param pRegion2 The second region object to copy to.
 *                 Must be a valid region object.
 *
 * @return STD_SUCCESS           on success.
 * @return STD_ERR_RES_EXHAUSTED if first region object can't accommodate the
 *                               entire second region.
 *
 * @note Unchecked Invariant:
 * - Argument validity only exists in debug mode, in release mode invalid
 *   arguments cause undefined behavior.
 */
[[nodiscard("This function can fail, please check result for success.")]]
STD_API STD_Rslt STD_CALL
CONT_RegionExtendUnchecked(CONT_Region *pRegion1, const CONT_Region *pRegion2);
/**
 * Merges the second region object into the first.
 *
 * @param pRegion1 The first region object to copy from.
 *                 Must be a valid region object.
 * @param pRegion2 The second region object to copy to.
 *                 Must be a valid region object.
 *
 * @return STD_SUCCESS           on success.
 * @return STD_ERR_INV_ARG       if arguments are invalid.
 * @return STD_ERR_RES_EXHAUSTED if first region object can't accommodate the
 *                               entire second region.
 */
[[nodiscard("This function can fail, please check result for success.")]]
STD_API STD_Rslt STD_CALL CONT_RegionExtend(CONT_Region       *pRegion1,
                                            const CONT_Region *pRegion2);

#ifdef __cplusplus
}
#endif

#endif
