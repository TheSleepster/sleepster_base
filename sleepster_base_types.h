#if !defined(SLEEPSTER_BASE_TYPES_H)
/* ========================================================================
   $File: sleepster_base_types.h $
   $Date: Sat, 19 Apr 25: 07:55PM $
   $Revision: $
   $Creator: Justin Lewis $
   ======================================================================== */

#define SLEEPSTER_BASE_TYPES_H
#include <stdint.h>
#include <stdbool.h>

#define global        static
#define local_persist static
#define internal      static

#define external extern "C"

typedef uint8_t  uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef int8_t   int8;
typedef int16_t  int16;
typedef int32_t  int32;
typedef int64_t  int64;
typedef size_t   usize;

typedef int8     bool8;
typedef int32    bool32;

typedef float    real32;
typedef double   real64;

typedef float    float32;
typedef double   float64;

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t   s8;
typedef int16_t  s16;
typedef int32_t  s32;
typedef int64_t  s64;
typedef size_t   usize;

typedef int8     b8;
typedef int32    b32;

typedef float    real32;
typedef double   real64;

typedef float    float32;
typedef double   float64;

#define ArrayCount(x) (sizeof(x) / sizeof(*(x)))

#define IntFromPtr(p) (uint32)((char *)p - (char *)0)
#define PtrFromInt(I) (void *)((char *)0 + (I))

#define MemberHelper(type, member)    (((type *)0)->member)
#define GetMemberOffset(type, member) IntFromPtr(&MemberHelper(type, member))

// MEMORY OVERRIDE FUNCTIONS
#define PLATFORM_VIRTUAL_ALLOC(name) void *name(u64 Size)
#define PLATFORM_VIRTUAL_FREE(name)  void  name(void *Data, u64 Size)

#define PLATFORM_HEAP_ALLOC(name)    void *name(u64 Size)
#define PLATFORM_HEAP_REALLOC(name)  void *name(void *Data, u64 NewSize)
#define PLATFORM_HEAP_FREE(name)     void  name(void *Data)
// MEMORY OVERRIDE FUNCTIONS

#define null NULL
#endif
