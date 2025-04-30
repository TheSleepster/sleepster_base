#if !defined(SLEEPSTER_BASE_DEBUG_H)
/* ========================================================================
   $File: sleepster_base_debug.h $
   $Date: Sat, 19 Apr 25: 07:57PM $
   $Revision: $
   $Creator: Justin Lewis $
   ======================================================================== */

#define SLEEPSTER_BASE_DEBUG_H

#include "sleepster_base_types.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#ifdef INTERNAL_DEBUG
// MSVC
    #ifdef _MSC_VER
        #include <intrin.h>
        #define DebugHalt() __debugbreak()
    #else
        #define DebugHalt() __builtin_trap()
    #endif

#define Assert(Condition, Message, ...)                                                    \
{                                                                                          \
    if (!(Condition))                                                                      \
    {                                                                                      \
        Log(SL_LOG_FATAL, "Assertion failed:\n (%s) " Message, #Condition, ##__VA_ARGS__); \
        DebugHalt();                                                                       \
    }                                                                                      \
}

#define DEBUGBreak() DebugHalt()
#else

#define Assert(Condition, Message, ...)
#define DebugHalt()
#define DEBUGBreak() DebugHalt()

#endif

#define InvalidCodePath DebugHalt()
typedef enum debug_log_level_t
{
    SL_LOG_TRACE,
    SL_LOG_INFO,
    SL_LOG_WARNING,
    SL_LOG_ERROR,
    SL_LOG_FATAL
}debug_log_level;

#define Log(Level, Message, ...) _Log(Level, Message, __FILE__, __LINE__, ##__VA_ARGS__)

#define VINFO(Message, ...)    Log(LOG_INFO, Message, ##__VA_ARGS__)
#define VWARNING(Message, ...) Log(LOG_WARNING, Message, ##__VA_ARGS__)
#define VERROR(Message, ...)   Log(LOG_ERROR, Message, ##__VA_ARGS__)
#define VFATAL(Message, ...)   Log(LOG_FATAL, Message, ##__VA_ARGS__)
#define VVERBOSE(Message, ...) Log(LOG_TRACE, Message, ##__VA_ARGS__)

internal void
_Log(debug_log_level LogLevel, const char *Message, char *File, int32 Line, ...)
{
    const char *InfoStrings[] = {"[TRACE]: ", "[INFO]: ", "[WARNING]:", "[NON-FATAL ERROR]: ", "[FATAL ERROR]: "};
    const char *ColorCodes[]  =
    {
        "\033[36m",          // LOG_TRACE:   Teal
        "\033[32m",          // LOG_INFO:    Green
        "\033[33m",          // LOG_WARNING: Yellow
        "\033[31m",          // LOG_ERROR:   Red
        "\033[41m\033[30m"   // LOG_FATAL:   Red background, Black text
    };
    bool8 IsError = (LogLevel > 2);

    char Buffer[32000];
    memset(Buffer, 0, sizeof(Buffer));

    va_list ArgPtr;
    va_start(ArgPtr, Line);
    vsnprintf(Buffer, sizeof(Buffer), Message, ArgPtr);
    va_end(ArgPtr);

    char OutBuffer[32000];
    memset(OutBuffer, 0, sizeof(OutBuffer));

    if(IsError)
    {
        sprintf(OutBuffer, "%s%s[File: %s, Line %d] %s\033[0m\n", ColorCodes[LogLevel], InfoStrings[LogLevel], File, Line, Buffer);
    }
    else
    {
        sprintf(OutBuffer, "%s%s%s\033[0m\n", ColorCodes[LogLevel], InfoStrings[LogLevel], Buffer);
    }

    printf("%s", OutBuffer);
}

#endif
