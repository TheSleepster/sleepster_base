#if !defined(MEMORY_H)
/* ========================================================================
   $File: sleepster_base_memory.h $
   $Date: Thu, 17 Apr 25: 07:32PM $
   $Revision: $
   $Creator: Justin Lewis $
   ======================================================================== */

#define MEMORY_H

#include <stdlib.h>
#include <stddef.h>
#include <sleepster_base_types.h>
#include <sleepster_base_debug.h>

#define KB(x) (x     * 1000)
#define MB(x) (KB(x) * 1000)
#define GB(x) (MB(x) * 1000)

#ifndef SL_OVERRIDE_MEMORY_FUNCTIONS
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

struct memory_arena
{
    uint8  *Base;
    uint64  Used;
    uint64  Capacity;

    uint32  ScratchCounter;
};

struct scratch_arena
{
    memory_arena *Parent;
    uint8        *Base;
    uint64        Used;
};

#define ArenaPushSize(Arena, size, ...)                    PushSize_(Arena, size, ##__VA_ARGS__)
#define ArenaPushStruct(Arena, type, ...)          (type *)PushSize_(Arena, sizeof(type), ##__VA_ARGS__)
#define ArenaPushArray(Arena, type, Count, ...)    (type *)PushSize_(Arena, sizeof(type) * (Count), ##__VA_ARGS__)
#define BootstrapAllocateStruct(type, member, ...) (type *)BootstrapAllocateStruct_(sizeof(type),  offsetof(type, member), ##__VA_ARGS__)

internal memory_arena
ArenaCreate(uint64 Size, uint64 Alignment = 4)
{
    memory_arena Result;
    Result.Used = 0;
    Result.Capacity = Size;
    Result.Base = (uint8 *)PlatformVirtualAlloc(Size);

    return Result;
}

internal uint64
GetAlignmentOffset(memory_arena *Arena, uint64 Alignment = 4)
{
    uint64 Result = (uint64)(Arena->Base + Arena->Used);
    uint64 AlignmentMask = Alignment - 1;

    uint64 Offset = 0;
    if(Result & AlignmentMask)
    {
        Offset = Alignment - (Result & AlignmentMask);
    }

    return(Offset);
}

internal uint64
GetEffectionAllocationSize(memory_arena *Arena, uint64 SizeInit, uint64 Alignment = 4)
{
    uint64 Result = SizeInit;
    Result += GetAlignmentOffset(Arena, Alignment);

    return(Result);
}

internal void* 
PushSize_(memory_arena *Arena, uint64 SizeInit, uint64 Alignment = 4)
{
    void *Result = 0;
    uint64 Size = GetEffectionAllocationSize(Arena, SizeInit, Alignment);
    Assert(Arena->Used + Size <= Arena->Capacity, "Allocation would exceed arena capacity of: '%d'...", Arena->Capacity);

    uint64 AlignmentOffset = GetAlignmentOffset(Arena, Alignment);
    Result                 = (void *)((uint8 *)Arena->Base + (Arena->Used + AlignmentOffset));
    Arena->Used           += Size;

    return(Result);
}

internal void*
BootstrapAllocateStruct_(uint64 StructSize, uint64 OffsetToArena, uint64 BaseAllocation = MB(100), uint64 Alignment = 4)
{
    memory_arena BootstrapArena = ArenaCreate(BaseAllocation, Alignment);
    BootstrapArena.Base       += StructSize;
    BootstrapArena.Capacity   -= StructSize;
    *(memory_arena *)((uint8 *)BootstrapArena.Base + OffsetToArena) = BootstrapArena;

    return(BootstrapArena.Base);
}

internal scratch_arena
BeginTemporaryMemory(memory_arena *Arena, uint64 Size)
{
    scratch_arena Result;
    Result.Parent = Arena;
    Result.Used   = Arena->Used;
    Result.Base   = Arena->Base;
    ++Arena->ScratchCounter;

    return(Result);
}

internal void
EndTemporaryMemory(scratch_arena *Scratch)
{
    Assert(Scratch->Parent->ScratchCounter > 0, "Arena has an invalid scratch counter...");
    --Scratch->Parent->ScratchCounter;


    Scratch->Parent->Used = Scratch->Used;
    Scratch->Parent->Base = Scratch->Base;
}

internal void
ArenaReset(memory_arena *Arena)
{
    Arena->Used = 0;
}

#endif
