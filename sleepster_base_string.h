#if !defined(SLEEPSTER_BASE_STRING_H)
/* ========================================================================
   $File: sleepster_base_string.h $
   $Date: Thu, 13 Mar 25: 03:04PM $
   $Revision: $
   $Creator: Justin Lewis $
   ======================================================================== */

#define SLEEPSTER_BASE_STRING_H

#include "sleepster_base_types.h"
#include "sleepster_base_memory.h"

// NOTE(Sleepster): THESE ARE READ ONLY
struct string_u8
{
    uint8 *Data;
    uint32 Length;
};

#define str_lit(x) (StringCreate(x, GetStringLength(x)))
#define c_str(x)   ((const char *)x.Data)

#define stringify_(s) #s
#define stringify(s) stringify_(s)
#define glue__(a, b) a##b
#define glue(a, b)   glue__(a, b)

constexpr string_u8 NullString = {};

internal string_u8
StringCreate(char *StringData, uint32 Length)
{
    string_u8 Result;
    Result.Data   = (uint8 *)StringData;
    Result.Length = Length;

    return(Result);
}

internal void
StringMakeHeap(memory_arena *Arena, string_u8 *String)
{
    string_u8 Temp = {};
    Temp.Data = (uint8 *)ArenaPushSize(Arena, String->Length * sizeof(uint8));
    memcpy(Temp.Data, String->Data, String->Length * sizeof(uint8));

    String->Data = Temp.Data;
}

internal string_u8*
StringCopy(memory_arena *Arena, string_u8 String)
{
    string_u8 *Result;
    Result = ArenaPushStruct(Arena, string_u8);
    if(Result)
    {
        Result->Length = String.Length;
        Result->Data   = ArenaPushArray(Arena, uint8, String.Length);
        if(Result->Data)
        {
            memcpy(Result->Data, String.Data, String.Length * sizeof(uint8));
        }
        else
        {
            Log(SL_LOG_ERROR, "Failed to allocate the string's data...");
        }
    }
    else
    {
        Log(SL_LOG_ERROR, "Failed to arena alloc Result...\n");
    }
    return(Result);
}

internal string_u8
StringSubFromLeft(string_u8 String, uint32 Index)
{
    string_u8 Result;
    Result.Data   = String.Data + Index;
    Result.Length = String.Length - Index;

    return(Result);
}

internal string_u8
StringSubFromRight(string_u8 String, uint32 Index)
{
    string_u8 Result;
    Result.Data   = (String.Data + String.Length) - Index;
    Result.Length = Index;

    return(Result);
}

internal string_u8
StringSubstring(string_u8 String, uint32 FirstIndex, uint32 LastIndex)
{
    string_u8 Result;
    Result.Data   = (String.Data + FirstIndex);
    Result.Length = LastIndex - FirstIndex;

    return(Result);
}

internal string_u8
StringConcat(memory_arena *Arena, string_u8 A, string_u8 B)
{
    string_u8 Result;

    uint32 NewLength = A.Length + B.Length;
    Result.Data   = ArenaPushArray(Arena, uint8, NewLength);
    Result.Length = NewLength;

    memcpy(Result.Data, A.Data, A.Length);
    memcpy(Result.Data + A.Length, B.Data, B.Length);

    return(Result);
}

// NOTE(Sleepster): Compare By Pointer
internal inline bool32
StringCompare(string_u8 *A, string_u8 *B)
{
    bool32 Result = false;
    if(A && B)
    {
        if(A->Length != B->Length) Result = false;
        if(A->Data == B->Data)     Result = true;
    
        Result = (memcmp(A->Data, B->Data, A->Length) == 0);
    }
    return Result;
}

// NOTE(Sleepster): OVERLOAD compare by value
internal inline bool32
StringCompare(string_u8 A, string_u8 B)
{
    if(A.Length != B.Length) return(0);
    if(A.Data == B.Data)     return(1);
    
    return(memcmp(A.Data, B.Data, A.Length) == 0);
}

internal inline uint32
GetStringLength(const char *String)
{
    if(String)
    {
        uint32 Length = 0;
        while(*String != 0)
        {
            ++Length;
            ++String;
        }
        return(Length);
    }
    else
    {
        return(0);
    }
}

internal inline int32
FindFirstCharFromLeft(string_u8 String, char Character)
{
    int32 Result = -1;
    for(uint32 Index = 0;
        Index < String.Length;
        ++Index)
    {
        char Found = (char)String.Data[Index];
        if(Found == Character)
        {
            Result = Index;
            break;
        }
    }

    return(Result);
}

internal inline int32
FindFirstCharFromRight(string_u8 String, char Character)
{
    int32 Result = -1;
    for(uint32 Index = String.Length;
        Index > 0;
        --Index)
    {
        char Found = (char)String.Data[Index];
        if(Found == Character)
        {
            Result = Index;
            break;
        }
    }

    return(Result);
}

// TODO(Sleepster): Strip the ext???
internal inline string_u8
GetFilenameFromPath(string_u8 Filepath)
{
    string_u8 Result = {};
    int32 ExtStart = FindFirstCharFromRight(Filepath, '/');
    if(ExtStart != -1)
    {
        Result = StringSubstring(Filepath, ExtStart + 1, Filepath.Length);
    }

    return(Result);
}

internal inline string_u8
GetFileExtFromPath(string_u8 Filepath)
{
    string_u8 Result = {};
    int32 ExtStart = FindFirstCharFromRight(Filepath, '.');
    if(ExtStart != -1)
    {
        Result = StringSubstring(Filepath, ExtStart, Filepath.Length);
    }

    return(Result);
}

#endif
