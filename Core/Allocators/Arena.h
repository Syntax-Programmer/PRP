#ifndef DEF_ALOC_ARENA_
#define DEF_ALOC_ARENA_

#ifdef __cplusplus
extern "C" {
#endif

#include "Core/Diagnostics/Debug.h"
#include "Core/Std/Platform.h"
#include "Core/Std/Types.h"

/**
 * A simple bump allocator nothing special.
 */
typedef struct ALOC_Arena ALOC_Arena;

/**
 * Checks whether the given arena object is valid.
 *
 * @param pArena The arena object to check.
 *
 * @return true if valid, false otherwise.
 */
STD_API STD_Bool STD_CALL ALOC_ArenaIsValid(const ALOC_Arena *pArena);

#define ALOC_ArenaIsValidAssert(pArena)                                        \
    DIAG_AssertMsg(ALOC_ArenaIsValid(pArena),                                  \
                   "The given arena object is corrupted/invalid.")

/**
 * Creates a new arena object with the specified size.
 *
 * @param size    The size in bytes for the arena.
 *                Must be greater than 0 and less than max block size.
 * @param ppArena Output pointer that receives the created arena object.
 *                Must not equal nullptr.
 *
 * @return STD_SUCCESS     on success.
 * @return STD_ERR_INV_ARG if arguments are invalid.
 * @return STD_ERR_OOM     if memory allocation fails.
 */
STD_API STD_Rslt STD_CALL ALOC_ArenaCreate(STD_Size size, ALOC_Arena **ppArena);
/**
 * Destroys the arena and nullifies the object pointer.
 *
 * @param ppArena Pointer to the arena object to delete.
 *                Must not equal nullptr.
 *                Its dereference must not equal nullptr.
 *
 * @return STD_SUCCESS     on success.
 * @return STD_ERR_INV_ARG if arguments are invalid.
 */
STD_API STD_Rslt STD_CALL ALOC_ArenaDestroy(ALOC_Arena **ppArena);
/**
 * Resets and invalidates all allocations inside the arena all at once.
 *
 * @param pArena The arena object to reset.
 *               Must be a valid arena object.
 *
 * @return STD_SUCCESS     on success.
 * @return STD_ERR_INV_ARG if arguments are invalid.
 */
STD_API STD_Rslt STD_CALL ALOC_ArenaReset(ALOC_Arena *pArena);

/**
 * Allocates a chunk of memory from the arena.
 *
 * @param pArena The arena object to allocate from.
 *               Must be a valid arena object.
 * @param size   The size of the memory block to allocate.
 *               Must not equal zero.
 * @param zeroed If true the allocated memory block is zero initialized.
 * @param ppDest Output pointer to where the allocated memory pointer will be
 *               written to.
 *               Must not equal nullptr.
 *
 * @return STD_SUCCESS           on success.
 * @return STD_ERR_INV_ARG       if arguments are invalid.
 * @return STD_ERR_RES_EXHAUSTED if arena can't accommodate the asked allocation
 *                               size.
 */
STD_API STD_Rslt STD_CALL ALOC_ArenaAlloc(ALOC_Arena *pArena, STD_Size size,
                                          STD_Bool zeroed, void **ppDest);
/**
 * Allocates multiple number of a fixed size chunk of memory from the arena, in
 * a contiguous allocation.
 *
 * @param pArena The arena object to allocate from.
 *               Must be a valid arena object.
 * @param size   The size of the memory block to allocate.
 *               Must not equal zero.
 * @param zeroed If true the allocated memory block is zero initialized.
 * @param count  The number of the memory blocks to allocate.
 *               Must not equal zero.
 * @param ppDest Output pointer to where the allocated memory pointer will be
 *               written to.
 *               Must not equal nullptr.
 *
 * @return STD_SUCCESS           on success.
 * @return STD_ERR_INV_ARG       if arguments are invalid.
 * @return STD_ERR_RES_EXHAUSTED if arena can't accommodate the asked allocation
 *                               size.
 */
STD_API STD_Rslt STD_CALL ALOC_ArenaAllocMany(ALOC_Arena *pArena, STD_Size size,
                                              STD_Bool zeroed, STD_Size count,
                                              void **ppDest);
/**
 * Allocates a chunk of aligned memory from the arena.
 *
 * @param pArena The arena object to allocate from.
 *               Must be a valid arena object.
 * @param size   The size of the memory block to allocate.
 *               Must not equal zero.
 * @param align  The alignment the allocation shall follow.
 *               Must be a non zero power of two.
 * @param zeroed If true the allocated memory block is zero initialized.
 * @param ppDest Output pointer to where the allocated memory pointer will be
 *               written to.
 *               Must not equal nullptr.
 *
 * @return STD_SUCCESS           on success.
 * @return STD_ERR_INV_ARG       if arguments are invalid.
 * @return STD_ERR_RES_EXHAUSTED if arena can't accommodate the asked allocation
 *                               size.
 */
STD_API STD_Rslt STD_CALL ALOC_ArenaAllocAligned(ALOC_Arena *pArena,
                                                 STD_Size size, STD_Size align,
                                                 STD_Bool zeroed,
                                                 void   **ppDest);
/**
 * Allocates multiple number of a fixed size chunk of aligned memory from the
 * arena, in a contiguous allocation.
 *
 * @param pArena The arena object to allocate from.
 *               Must be a valid arena object.
 * @param size   The size of the memory block to allocate.
 *               Must not equal zero.
 * @param align  The alignment the allocation shall follow.
 *               Must be a non zero power of two.
 * @param zeroed If true the allocated memory block is zero initialized.
 * @param count  The number of the memory blocks to allocate.
 *               Must not equal zero.
 * @param ppDest Output pointer to where the allocated memory pointer will be
 *               written to.
 *               Must not equal nullptr.
 *
 * @return STD_SUCCESS           on success.
 * @return STD_ERR_INV_ARG       if arguments are invalid.
 * @return STD_ERR_RES_EXHAUSTED if arena can't accommodate the asked allocation
 *                               size.
 */
STD_API STD_Rslt STD_CALL
ALOC_ArenaAllocManyAligned(ALOC_Arena *pArena, STD_Size size, STD_Size align,
                           STD_Bool zeroed, STD_Size count, void **ppDest);

/**
 * Gets the max memory size in bytes that the arena can track and allocate.
 *
 * @return The max size of arena's memory block.
 */
STD_API STD_Size STD_CALL ALOC_ArenaMaxBlockSize(void);

/**
 * Gets the free space left in the given arena.
 *
 * @param pArena The arena object to get free space of.
 *               Must be a valid arena object.
 *
 * @return The free space in the arena.
 */
STD_API STD_Size STD_CALL ALOC_ArenaGetFreeSpace(const ALOC_Arena *pArena);
/**
 * Checks if the given arena is completely empty.
 *
 * @param pArena The arena object to check.
 *               Must be a valid arena object.
 *
 * @return true if empty, false otherwise.
 */
STD_API STD_Bool STD_CALL ALOC_ArenaIsEmpty(const ALOC_Arena *pArena);
/**
 * Checks if the given arena is completely full.
 *
 * @param pArena The arena object to check.
 *               Must be a valid arena object.
 *
 * @return true if full, false otherwise.
 */
STD_API STD_Bool STD_CALL ALOC_ArenaIsFull(const ALOC_Arena *pArena);
/**
 * Checks if the given memory pointer is owned by the arena.
 *
 * @param pArena The arena object to check into.
 *               Must be a valid arena object.
 * @param pBlock The memory pointer to check.
 *               Must not equal nullptr.
 *
 * @return true if pBlock belongs to the arena, false otherwise.
 */
STD_API STD_Bool STD_CALL ALOC_ArenaIsOwnedByArena(const ALOC_Arena *pArena,
                                                   const void       *pBlock);

#ifdef __cplusplus
}
#endif

#endif
