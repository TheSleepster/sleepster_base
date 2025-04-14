/* ========================================================================
   $File: sleepster_base_memory.cpp $
   $Date: Wed, 12 Mar 25: 06:53AM $
   $Revision: $
   $Creator: Justin Lewis $
   ======================================================================== */
#if !defined(SLEEPSTER_BASE_MEMORY_CPP)
#define SLEEPSTER_BASE_MEMORY_CPP

#include "sleepster_base_memory.h"

/*
 // NOTE(Sleepster):
    - These are static arenas. They do not grow.
*/

internal memory_pool
MemoryPoolCreate(uint64 Size)
{
    memory_pool Result;
    Result.PoolSize = Size;
    Result.Data     = PlatformVirtualAlloc(Size);

    if(Result.Data)
    {
        Result.Offset = (uint8 *)Result.Data;

#ifdef INTERNAL_DEBUG
        Log(SL_LOG_TRACE, "A Memory Pool of size: '%.2f'GB has been allocated from the OS...",
            Size * 0.001 * 0.001 * 0.001);
#endif
    }
    else
    {
        Log(SL_LOG_ERROR, "Allocation of size '%d' Failed...\n", Size);

        memory_pool Error = {};
        return(Error);
    }
    
    return(Result);
}

internal memory_arena
ArenaCreate(memory_pool *ParentPool, uint64 ArenaSize)
{
    if(uint32(ParentPool->Used + ArenaSize) > ParentPool->PoolSize)
    {
        Log(SL_LOG_FATAL, "The passed pool only has a size of '%d', but the allocation would push it's used to '%d'...",
            ParentPool->PoolSize, ArenaSize + ParentPool->Used);
    }

#ifdef INTERNAL_DEBUG
    Log(SL_LOG_TRACE, "Arena with size: '%d' has been created... Space remaining in pool: '%d'...",
        ArenaSize, ParentPool->PoolSize - ParentPool->Used);
#endif
    
    memory_arena Result;
    Result.Capacity  = ArenaSize;
    Result.Allocated = 0;
    Result.Base      = ParentPool->Offset;

    ParentPool->Used   += ArenaSize;
    ParentPool->Offset += ArenaSize;
    return(Result);
}

internal uint32
ArenaGetAlignmentOffset(memory_arena *Arena, uint32 Alignment = 4)
{
    size_t Offset = (size_t)Arena->Base + Arena->Allocated;
    uint32 AlignmentMask   = Alignment - 1;
    uint32 AlignmentOffset = 0;

    if(Offset & AlignmentMask) // Checking if alignment is a power of two
    {
        AlignmentOffset = Alignment - (Offset & AlignmentMask); // If not, make it one.
    }

    return(AlignmentOffset);
}

internal void*
ArenaAllocate(memory_arena *Arena, uint64 Size, const char *Filename, int32 Line, uint32 Alignment)
{
    void *Result;
    
    uint32 AlignmentOffset = ArenaGetAlignmentOffset(Arena, Alignment);
    Size += AlignmentOffset;

    Assert((Arena->Allocated + Size) <= Arena->Capacity,
           "Arena allocation with size '%d' would exceed the capacity of '%d'", Size, Arena->Capacity);
    Result = (void *)(Arena->Base + Arena->Allocated + AlignmentOffset);

#ifdef INTERNAL_DEBUG
    if(Filename && Line > 0)
    {
        Log(SL_LOG_TRACE, "'%s', Line: '%d'...\n \t\t  Size of: '%d', has been arena allocated... Arena has: '%d' bytes remaining...",
            Filename, Line, Size, Arena->Capacity - Arena->Allocated);
    }
#endif

    Arena->Allocated += Size;
    return(Result);
}

internal memory_arena
ArenaCreateSubArena(memory_arena *ParentArena, uint64 Capacity, uint32 Alignment = 4)
{
    Assert(ParentArena->Allocated + Capacity < ParentArena->Capacity,
           "Allocation of Subarena with size '%d' would Exceed the parent arena's capacity of '%d'...",
           Capacity,
           ParentArena->Capacity);
    
    memory_arena Result;
    Result.Capacity = Capacity;
    Result.Base = (uint8 *)ArenaPushSize(ParentArena, Capacity, Alignment);

#ifdef INTERNAL_DEBUG
    Log(SL_LOG_TRACE, "Child Arena with size: '%d' has been created... Space remaining in Parent Arena: '%'...",
        Capacity, ParentArena->Capacity - ParentArena->Allocated);
#endif

    return(Result);
}

internal inline void
ArenaClear(memory_arena *Arena)
{
    Arena->Allocated = 0;
}

#endif
