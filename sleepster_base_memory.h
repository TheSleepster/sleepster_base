#if !defined(SLEEPSTER_BASE_MEMORY_H)
/* ========================================================================
   $File: sleepster_base_memory.h $
   $Date: Sat, 19 Apr 25: 07:59PM $
   $Revision: $
   $Creator: Justin Lewis $
   ======================================================================== */

#define SLEEPSTER_BASE_MEMORY_H
#include <stdlib.h>
#include <stddef.h>
#include <sleepster_base_types.h>
#include <sleepster_base_debug.h>

#define KB(x) (x     * 1000)
#define MB(x) (KB(x) * 1000)
#define GB(x) (MB(x) * 1000)

#ifndef SL_OVERRIDE_C_STBLIB_MEMORY

internal inline
PLATFORM_HEAP_ALLOC(PlatformHeapAlloc)
{
    return(malloc(Size));
}

internal inline
PLATFORM_HEAP_FREE(PlatformHeapFree)
{
    free(Data);
}

internal inline 
PLATFORM_HEAP_REALLOC(PlatformHeapRealloc)
{
    return(realloc(Data, NewSize));
}

internal inline
PLATFORM_VIRTUAL_ALLOC(PlatformVirtualAlloc)
{
    return(malloc(Size));
}

internal inline
PLATFORM_VIRTUAL_FREE(PlatformVirtualFree)
{
    return(free(Data));
}
#endif

/*
  NOTE(Sleepster): These are DYNAMICALLY GROWING memory arenas. 
*/

typedef struct memory_arena_t
{
    u8  *Base;
    u64  Capacity;
    u64  Used;

    u64  MinimumBlockSize;
    u32  ScratchCounter;
    u32  BlockCounter;
}memory_arena;

typedef struct memory_arena_footer_t
{
    u64  BlockUsed;
    u64  BlockCapacity;
    u8  *BlockBase;
}memory_arena_footer;

typedef struct scratch_memory_t
{
    memory_arena *Parent;
    u8           *Base;
    u64           Used;
}scratch_memory;

// functions
internal memory_arena                ArenaCreateNew(u64 MinimumBlockSize);
internal memory_arena                ArenaCreateFromBase(void *Base, u64 Capcity);
internal void*                       BootstrapAllocateStruct_(uint64 StructSize, uint64 OffsetToArena, uint64 BaseAllocation);
internal void*                       PushSize_(memory_arena *Arena, u64 SizeInit, u64 Alignment);
internal u64                         GetEffectiveAllocationSize(memory_arena *Arena, u64 SizeInit, u64 Alignment);
internal inline u64                  GetAlignmentOffset(memory_arena *Arena, u64 Alignment);
internal inline memory_arena_footer* ArenaGetFooter(memory_arena *Arena);
internal scratch_memory              BeginTemporaryMemory(memory_arena *Arena);
internal void                        EndTemporaryMemory(scratch_memory *Scratch);
internal void                        ArenaClear(memory_arena *Arena);
// functions

#define PushSize(arena, size, alignment)                         PushSize_(arena, size, alignment)
#define PushStruct(arena, type, alignment)               (type *)PushSize_(arena, sizeof(type), alignment)
#define PushArray(arena, type, count, alignment)         (type *)PushSize_(arena, (sizeof(type) * count), alignment)
#define BootstrapAllocateStruct(type, member, alloc)     (type *)BootstrapAllocateStruct_(sizeof(type),  offsetof(type, member), alloc)

internal inline memory_arena
ArenaCreateNew(u64 MinimumBlockSize)
{
    memory_arena Result     = {};
    Result.MinimumBlockSize = MinimumBlockSize;
    Result.Base             = PlatformVirtualAlloc(MinimumBlockSize);
    Result.Capacity         = MinimumBlockSize;
    
    return(Result);
}

internal memory_arena
ArenaCreateFromBase(void *Base, u64 Capacity)
{
    memory_arena Result     = {};
    Result.Base             = Base;
    Result.Capacity         = Capacity;
    Result.MinimumBlockSize = Capacity;

    return Result;
}

internal u64
GetAlignmentOffset(memory_arena *Arena, u64 Alignment)
{
    u64 Offset        = (u64)(Arena->Base + Arena->Used);
    u64 AlignmentMask = Alignment - 1;

    u64 AlignmentOffset = 0;
    if(Offset & AlignmentMask)
    {
        AlignmentOffset = Alignment - (Offset & AlignmentMask);
    }

    return(AlignmentOffset);
}

internal inline u64
GetEffectiveAllocationSize(memory_arena *Arena, u64 SizeInit, u64 Alignment)
{
    u64 Result = SizeInit;
    u64 Offset = GetAlignmentOffset(Arena, Alignment);
    Result += Offset;

    return(Result);
}

internal inline memory_arena_footer*
ArenaGetFooter(memory_arena *Arena)
{
    memory_arena_footer *Result;
    Result = (memory_arena_footer *)(Arena->Base + Arena->Capacity);

    return(Result);
}

internal void*
BootstrapAllocateStruct_(uint64 StructSize, uint64 OffsetToArena, u64 BaseAllocation)
{
    memory_arena BootstrapArena = ArenaCreateNew(BaseAllocation);
    BootstrapArena.Base        += StructSize;
    BootstrapArena.Capacity    -= StructSize;
    *(memory_arena *)((uint8 *)BootstrapArena.Base + OffsetToArena) = BootstrapArena;

    return((void *)BootstrapArena.Base);
}

internal void*
PushSize_(memory_arena *Arena, u64 SizeInit, u64 Alignment)
{
    void *Result = null;
    u64 Size = GetEffectiveAllocationSize(Arena, SizeInit, Alignment);
    if(Arena->Used + Size > Arena->Capacity)
    {
        if(!Arena->MinimumBlockSize)
        {
            Arena->MinimumBlockSize = MB(10);
        }

        memory_arena_footer SaveFooter;
        SaveFooter.BlockBase     = Arena->Base;
        SaveFooter.BlockCapacity = Arena->Capacity;
        SaveFooter.BlockUsed     = Arena->Used;

        Size = SizeInit;
        u64 NewBlockSize = Size > (Arena->MinimumBlockSize + sizeof(memory_arena_footer)) ? Size : Arena->MinimumBlockSize;

        Arena->Capacity   = NewBlockSize - sizeof(memory_arena_footer);
        Arena->Base       = PlatformVirtualAlloc(NewBlockSize);
        Arena->BlockCounter++;

        memory_arena_footer *ArenaFooter = ArenaGetFooter(Arena);
        *ArenaFooter = SaveFooter;
    }

    Assert((Arena->Used + Size) <= Arena->Capacity, "Allocation would exceed the arena's capacity of '%d'...", Arena->Capacity);
    Assert(Size >= SizeInit, "Allocation size is wrong...\n");

    u64 AlignmentOffset = GetAlignmentOffset(Arena, Alignment);
    Result = (Arena->Base + Arena->Used + AlignmentOffset);
    Arena->Used += Size;

    return(Result);
}

internal void
FreeLastArenaBlock(memory_arena *Arena)
{
    u8 *Free = Arena->Base;
    u64 OldCapacity = Arena->Capacity;

    memory_arena_footer *Footer = ArenaGetFooter(Arena);
    Arena->Base     = Footer->BlockBase;
    Arena->Used     = Footer->BlockUsed;
    Arena->Capacity = Footer->BlockCapacity;

    PlatformVirtualFree(Free, OldCapacity);
    Arena->BlockCounter--;
}

internal scratch_memory
BeginTemporaryMemory(memory_arena *Arena)
{
    scratch_memory Result;

    Result.Parent = Arena;
    Result.Base   = Arena->Base;
    Result.Used   = Arena->Used;
    Arena->ScratchCounter++;

    return(Result);
}

internal void
EndTemporaryMemory(scratch_memory *Scratch)
{
    memory_arena *Arena = Scratch->Parent;
    while(Arena->Base != Scratch->Base)
    {
        FreeLastArenaBlock(Arena);
    }

    Assert(Arena->Used >= Scratch->Used, "Something went wrong...\n");
    Assert(Arena->ScratchCounter > 0, "Scratch Counter is >= 0...\n");

    Arena->Used = Scratch->Used;
    Arena->Base = Scratch->Base;
    Arena->ScratchCounter--;
}

internal void
ArenaClear(memory_arena *Arena)
{
    while(Arena->BlockCounter > 0)
    {
        FreeLastArenaBlock(Arena);
    }
    Arena->Used = 0;
}

#endif
