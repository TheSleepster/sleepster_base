#if !defined(SLEEPSTER_TYPES_H)
/* ========================================================================
   $File: sleepster_base_types.h $
   $Date: Wed, 12 Mar 25: 05:19AM $
   $Revision: $
   $Creator: Justin Lewis $
   ======================================================================== */

#define SLEEPSTER_TYPES_H
#include <stdint.h>

#define global   static
#define local    static
#define internal static

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

#define ArrayCount(x) (sizeof(x) / sizeof(*(x)))

#define IntFromPtr(p) (uint32)((char *)p - (char *)0)
#define PtrFromInt(I) (void *)((char *)0 + (I))

#define MemberHelper(type, member)    (((type *)0)->member)
#define GetMemberOffset(type, member) IntFromPtr(&MemberHelper(type, member))

#define null nullptr

#if _MSC_VER
// TODO(Sleepster): Double check this stuff
#define WriteBarrier     _WriteBarrier(); _mm_sfence()
#define ReadBarrier      _ReadBarrier();  _mm_sfence()
#define ReadWriteBarrier _ReadWriteBarrier(); _mm_lfence()
#endif


#endif
