#pragma once

#include "IO/Log.hpp"

#ifdef PK_DEBUG
#define PK_ASSERT(cond, ...)                     \
    do                                           \
    {                                            \
        if (!(cond))                             \
        {                                        \
            __PK_ASSERT_MSG(#cond, __VA_ARGS__); \
            exit(1);                             \
        }                                        \
    } while (0)

#define __PK_ASSERT_MSG(cond, msg, ...) \
    CoreLogger::Error("Assertion failed: "##cond " [" __FILE__ ", " __PK_TO_STRING(__LINE__) "] " msg, __VA_ARGS__)
#else
#define PK_ASSERT(cond, ...) \
    do                       \
    {                        \
    } while (0)
#endif

#define __PK_TO_STRING(x) __PK_STRINGIFY(x)
#define __PK_STRINGIFY(x) #x