// Copyright (c) Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#ifndef _ALIMER_H
#define _ALIMER_H

#include <stdbool.h>
#include <stdlib.h> // malloc, free
#include <string.h> // memset

/* Compiler defines */
#if defined(__clang__)
#define ALIMER_LIKELY(x) __builtin_expect(!!(x), 1)
#define ALIMER_UNLIKELY(x) __builtin_expect(!!(x), 0)
#define ALIMER_UNREACHABLE() __builtin_unreachable()
#define ALIMER_DEBUG_BREAK() __builtin_trap()
// CLANG ENABLE/DISABLE WARNING DEFINITION
#define ALIMER_DISABLE_WARNINGS() \
    _Pragma("clang diagnostic push")\
	_Pragma("clang diagnostic ignored \"-Wall\"") \
	_Pragma("clang diagnostic ignored \"-Wextra\"") \
	_Pragma("clang diagnostic ignored \"-Wtautological-compare\"") \
    _Pragma("clang diagnostic ignored \"-Wnullability-completeness\"") \
    _Pragma("clang diagnostic ignored \"-Wnullability-extension\"") \
    _Pragma("clang diagnostic ignored \"-Wunused-parameter\"") \
    _Pragma("clang diagnostic ignored \"-Wunused-function\"") \
    _Pragma("clang diagnostic ignored \"-Wtypedef-redefinition\"") \
    _Pragma("clang diagnostic ignored \"-Wdeprecated-declarations\"")

#define ALIMER_ENABLE_WARNINGS() _Pragma("clang diagnostic pop")
#elif defined(__GNUC__) || defined(__GNUG__)
#define ALIMER_LIKELY(x) __builtin_expect(!!(x), 1)
#define ALIMER_UNLIKELY(x) __builtin_expect(!!(x), 0)
#define ALIMER_UNREACHABLE() __builtin_unreachable()
#define ALIMER_DEBUG_BREAK() __builtin_trap()
// GCC ENABLE/DISABLE WARNING DEFINITION
#	define ALIMER_DISABLE_WARNINGS() \
	_Pragma("GCC diagnostic push") \
	_Pragma("GCC diagnostic ignored \"-Wall\"") \
	_Pragma("GCC diagnostic ignored \"-Wextra\"") \
	_Pragma("GCC diagnostic ignored \"-Wtautological-compare\"") \
    _Pragma("GCC diagnostic ignored \"-Wunused-parameter\"") \
    _Pragma("GCC diagnostic ignored \"-Wunused-function\"") \
    _Pragma("GCC diagnostic ignored \"-Wmissing-field-initializers\"") \
    _Pragma("GCC diagnostic ignored \"-Wunused-but-set-variable\"") \
    _Pragma("GCC diagnostic ignored \"-Wunused-result\"")

#define ALIMER_ENABLE_WARNINGS() _Pragma("GCC diagnostic pop")
#elif defined(_MSC_VER)
#define ALIMER_LIKELY(x) (x)
#define ALIMER_UNLIKELY(x) (x)
#define ALIMER_UNREACHABLE() __assume(false)
#define ALIMER_DEBUG_BREAK() __debugbreak()
#define ALIMER_DISABLE_WARNINGS() __pragma(warning(push, 0))
#define ALIMER_ENABLE_WARNINGS() __pragma(warning(pop))
#endif

#define ALIMER_STRINGIZE_HELPER(X) #X
#define ALIMER_STRINGIZE(X) ALIMER_STRINGIZE_HELPER(X)
#define ALIMER_UNUSED(x) (void)(x)

// Macro for determining size of arrays.
#if defined(_MSC_VER)
#   define ALIMER_ARRAYSIZE(arr) _countof(arr)
#else
#   define ALIMER_ARRAYSIZE(arr) (sizeof(arr) / sizeof(arr[0]))
#endif

#ifndef ALIMER_ASSERT
#   include <assert.h>
#   define ALIMER_ASSERT(c) assert(c)
#endif

#define ALIMER_MIN(a, b) (a < b ? a : b)
#define ALIMER_MAX(a, b) (a > b ? a : b)
#define ALIMER_CLAMP(val, min, max) ALIMER_MAX(min, ALIMER_MIN(val, max))
#define ALIMER_DEF(val, def) (((val) == 0) ? (def) : (val))
#define ALIMER_DEF_FLOAT(val, def) (((val) == 0.0f) ? (def) : (val))
#define _check_flags(flags, flag) (((flags) & (flag)) != 0)

#endif /* _ALIMER_H */
