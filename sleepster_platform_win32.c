/* ========================================================================
   $File: sleepster_platform_win32.c $
   $Date: Sat, 19 Apr 25: 11:19PM $
   $Revision: $
   $Creator: Justin Lewis $
   ======================================================================== */

#define WIN32_LEAN_AND_MEAN
#define NO_MIN_MAX
#include <windows.h>

// NOTE(Sleepster): Forward declaring these is unfortunately necessary.
//                  Our beloved C doesn't know it's literally 2 lines below the #include 
#define SL_OVERRIDE_C_STBLIB

PLATFORM_HEAP_ALLOC(PlatformHeapAlloc);
PLATFORM_HEAP_REALLOC(PlatformHeapRealloc);
PLATFORM_HEAP_FREE(PlatformHeapFree);
PLATFORM_VIRTUAL_ALLOC(PlatformVirtualAlloc);
PLATFORM_VIRTUAL_FREE(PlatformVirtualFree);

#include <sleepster_base_types.h>
#include <sleepster_base_debug.h>
#include <sleepster_base_memory.h>

internal
PLATFORM_HEAP_ALLOC(PlatformHeapAlloc)
{
    void *Result = {};
    
    HANDLE HeapHandle = GetProcessHeap();
    if(HeapHandle)
    {
        Result = HeapAlloc(HeapHandle, HEAP_ZERO_MEMORY, Size);
        if(Result)
        {
            return(Result);
        }
        HRESULT Error = HRESULT_FROM_WIN32(GetLastError());
        Log(SL_LOG_FATAL, "HeapAlloc has failed... Error Code: %d", Error);
        DebugHalt();
    }
    return(null);
}

internal
PLATFORM_HEAP_REALLOC(PlatformHeapRealloc)
{
    void *Result;
    
    HANDLE HeapHandle = GetProcessHeap();
    if(HeapHandle)
    {
        Result = HeapReAlloc(HeapHandle, 0, Data, NewSize);
        if(Result)
        {
            return(Result);
        }
        HRESULT Error = GetLastError();
        Log(SL_LOG_FATAL, "HeapReAlloc has failed... Error: %d", Error);
        DebugHalt();
    }
    else
    {
        Log(SL_LOG_FATAL, "Failed to get the heap...");
        DebugHalt();
    }

    Result = 0;
    return(Result);
}

internal 
PLATFORM_HEAP_FREE(PlatformHeapFree)
{
    HANDLE HeapHandle = GetProcessHeap();
    if(HeapHandle)
    {
        BOOL Success = HeapFree(HeapHandle, 0, Data);
        HRESULT Error = GetLastError();
        if(!Success)
        {
            Log(SL_LOG_FATAL, "HeapFree has failed... Error: %d", Error);
        }        
    }
    else
    {
        Log(SL_LOG_FATAL, "Failed to get the heap...");
        DebugHalt();
    }
}

internal inline
PLATFORM_VIRTUAL_ALLOC(PlatformVirtualAlloc)
{
    return(VirtualAlloc(0, Size, MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE));
}

internal inline
PLATFORM_VIRTUAL_FREE(PlatformVirtualFree)
{
    VirtualFree(Data, 0, MEM_RELEASE);
}

internal
PLATFORM_READ_ENTIRE_FILE(PlatformReadEntireFile)
{
    string_u8 Result = {};
    HANDLE FileHandle = CreateFileA(c_str(Filepath),
                                   GENERIC_READ,
                                   FILE_SHARE_READ,
                                   0,
                                   OPEN_EXISTING,
                                   FILE_ATTRIBUTE_NORMAL,
                                   0);
    if(FileHandle != INVALID_HANDLE_VALUE)
    {
        DWORD FileSize = GetFileSize(FileHandle, 0);
        if(FileSize != 0)
        {
            Result.Length = FileSize;
            if(Arena) Result.Data = ArenaPushArray(Arena, uint8, FileSize);
            else      Result.Data = (uint8 *)PlatformHeapAlloc(sizeof(uint8) * FileSize);
            Assert(Result.Data, "Failed to allocate '%d' from our arena...\n", FileSize * sizeof(uint8));

            BOOL Success = ReadFile(FileHandle, Result.Data, Result.Length, 0, 0);
            if(!Success)
            {
                HRESULT Error = HRESULT_FROM_WIN32(GetLastError());
                DebugHalt();
            }
        }
        else
        {
            HRESULT Error = HRESULT_FROM_WIN32(GetLastError());
            DebugHalt();
        }
    }
    else
    {
        HRESULT Error = HRESULT_FROM_WIN32(GetLastError());
        DebugHalt();
    }

    CloseHandle(FileHandle);
    return(Result);
}
