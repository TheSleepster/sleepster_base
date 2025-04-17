#if !defined(SLEEPSTER_BASE_MEMORY_H)
/* ========================================================================
   $File: sleepster_base_memory.h $
   $Date: Wed, 19 Mar 25: 03:54PM $
   $Revision: $
   $Creator: Justin Lewis $
   ======================================================================== */

#define SLEEPSTER_BASE_MEMORY_H
#include "sleepster_base_types.h"
#include <stdlib.h>

struct memory_pool
{
    void  *Data;
    uint8 *Offset;
    uint64 Used;
    uint64 PoolSize;
};

struct memory_arena
{
    uint64 Capacity;
    uint64 Allocated;
    uint8 *Base;
};

#define KB(x) (x     * 1000)
#define MB(x) (KB(x) * 1000)
#define GB(x) (MB(x) * 1000)

#define ArenaAllocateDEBUG(arena, size, ...) ArenaAllocate(arena, size, __FILE__, __LINE__, ##__VA_ARGS__)

#define ArenaPushSize(Arena, size, ...)                 ArenaAllocateDEBUG(Arena, size, ##__VA_ARGS__)
#define ArenaPushStruct(Arena, type, ...)       (type *)ArenaAllocateDEBUG(Arena, sizeof(type), ##__VA_ARGS__)
#define ArenaPushArray(Arena, type, Count, ...) (type *)ArenaAllocateDEBUG(Arena, sizeof(type) * (Count), ##__VA_ARGS__)

internal void* ArenaAllocate(memory_arena *Arena, uint64 Size, const char *File = null, int32 Line = -1, uint32 Alignment = 4);

#ifndef SLEEPSTER_BASE_MEMORY_OVERLOAD
internal void*
PlatformHeapAlloc(uint64 Size)
{
    return(malloc(Size));
}

internal void
PlatformHeapFree(void *Data)
{
    free(Data);
}

internal void*
PlatformHeapRealloc(void *Data, uint64 NewSize)
{
    return(realloc(Data, NewSize));
}

internal void*
PlatformVirtualAlloc(uint64 Size)
{
    return(malloc(Size));
}

internal void
PlatformVirtualFree(void *Data)
{
    return(free(Data));
}
#endif


#endif
