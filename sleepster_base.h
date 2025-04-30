#if !defined(SLEEPSTER_BASE_H)
/* ========================================================================
   $File: sleepster_base.h $
   $Date: April 10 2025 12:45 pm $
   $Revision: $
   $Creator: Justin Lewis $
   ======================================================================== */

#define SLEEPSTER_BASE_H

// MSVC
#if defined(_MSC_VER)
    #define COMPILER_CL 1
    #if defined(_WIN_32)
        #define OS_WINDOWS 1
    #endif

    // ARCH
    #if defined(_M_AMD64)
        #define ARCH_X64 1
    #elif defined(_M_I86)
        #define ARCH_X86 1
    #elif defined(_M_ARM)
        #define ARCH_ARM 1
    #else
        #error ARCH not found...
    #endif

// CLANG
#elif defined(__clang__)
    #define COMPILER_CLANG 1
    #if defined(_WIN_32)
        #define OS_WINDOWS 1
    #elif defined(__gnu_linux__)
        #define OS_LINUX 1
    #elif defined(__APPLE__) && defined(__MACH__)
        #define OS_MAC 1
    #else
        #error OS not detected...
    #endif

    // ARCH
    #if defined(__amd64__)
        #define ARCH_X64 1
    #elif defined(__i386__)
        #define ARCH_X86 1
    #elif defined(__arm__)
        #define ARCH_ARM 1
    #elif defined(__aarch_64__)
        #define ARCH_ARM64 1
    #else
        #error x86/x64 is the only supported architecture at the moment...
    #endif

// GNU C
#elif defined(__GNUC__)
    #define COMPILER_GCC 1
    #if defined(_WIN_32)
        #define OS_WINDOWS 1
    #elif defined(__gnu_linux__)
        #define OS_LINUX 1
    #elif defined(__APPLE__) && defined(__MACH__)
        #define OS_MAC 1
    #else
        #error OS not detected...
    #endif
    
    // ARCH
    #if defined(__amd64__)
        #define ARCH_X64 1
    #elif defined(__i386__)
        #define ARCH_X86 1
    #elif defined(__arm__)
        #define ARCH_ARM 1
    #elif defined(__aarch_64__)
        #define ARCH_ARM64 1
    #else
        #error cannot find ARCH...
    #endif
#else
    #error unable to distinguish this compiler...
#endif

// COMPILERS
#if !defined(COMPILER_CL)
    #define COMPILER_CL 0
#endif
#if !defined(COMPILER_CLANG)
    #define COMPILER_CLANG 0
#endif
#if !defined(COMPILER_GCC)
    #define COMPILER_GCC 0
#endif

// OPERATING SYSTEMS
#if !defined(OS_WINDOWS)
    #define OS_WINDOWS 0
#endif
#if !defined(OS_LINUX)
    #define OS_LINUX 0
#endif
#if !defined(OS_MAC)
    #define OS_MAC 0
#endif

// ARCH
#if !defined(ARCH_X64)
    #define ARCH_X64 0
#endif
#if !defined(ARCH_X86)
    #define ARCH_X86 0
#endif
#if !defined(ARCH_ARM)
    #define ARCH_ARM 0
#endif
#if !defined(ARCH_ARM64)
    #define ARCH_ARM64 0
#endif

#include "sleepster_base_types.h"
#include "sleepster_base_debug.h"
#include "sleepster_base_memory.h"
#include "sleepster_base_darray.h"
#include "sleepster_base_string.h"
#include "sleepster_base_hash_table.h"

#endif // SLEEPSTER_BASE_H

