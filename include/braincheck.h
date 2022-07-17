/*
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef BRAINCHECK_H
#define BRAINCHECK_H

#ifdef __cplusplus
extern "C" {
#endif

// The developer should define "BRAINCHECK_PRINTF" to a function that prints *and* flushes output
// This is used when outputting debug information specific to the developer
#ifndef BRAINCHECK_PRINTF
#error "The developer should define BRAINCHECK_PRINTF prior to including braincheck header"
#endif

#ifdef BRAINCHECK_DISABLE
#define BRAINCHECK_DISABLE_DEBUG
#define BRAINCHECK_DISABLE_BACKTRACE
#define BRAINCHECK_DISABLE_PERROR
#define BRAINCHECK_DISABLE_ASSERT
#endif

#if defined(BRAINCHECK_DISABLE_ASSERT) && !defined(BRAINCHECK_NON_FATAL_ASSERTS)
#define BRAINCHECK_ASSERTS_FATAL
#endif

#ifndef BRAINCHECK_DISABLE_DEBUG
#include <stdbool.h>
#endif

#ifndef BRAINCHECK_DISABLE_PERROR
#include <errno.h>
#include <string.h>
#endif

#ifndef BRAINCHECK_DISABLE_BACKTRACE
#include <execinfo.h>
#endif

#ifndef BRAINCHECK_DISABLE_ASSERT
#include <stdarg.h>
#ifdef BRAINCHECK_ASSERTS_FATAL
#include <assert.h>
#endif
#endif

#if defined(__clang__) || defined(__GNUC__)
#define BRAINCHECK_PRINTF_FUNCTION(format_position, varargs_start_position) __attribute__((format(printf, format_position, varargs_start_position)))
#else
#define BRAINCHECK_PRINTF_FUNCTION(format_position, varargs_start_position)
#endif

#if defined(__clang__) || defined(__GNUC__)
#define BRAINCHECK_FUNCTION_NAME __PRETTY_FUNCTION__
#elif defined(_MSC_VER)
#define BRAINCHECK_FUNCTION_NAME __FUNCSIG__
#else
#define BRAINCHECK_FUNCTION_NAME __func__
#endif

#ifndef BRAINCHECK_DISABLE

#ifdef BRAINCHECK_ANSI_FORMAT
#define BRAINCHECK_FORMAT_START_LOCATION "\x1b[0m\x1b[02m"
#define BRAINCHECK_FORMAT_END_LOCATION "\x1b[0m"
#define BRAINCHECK_FORMAT_START_FUNCTION "\x1b[0m\x1b[34m"
#define BRAINCHECK_FORMAT_END_FUNCTION "\x1b[0m"
#define BRAINCHECK_FORMAT_START_EXPRESSION "\x1b[36m\x1b[1m"
#define BRAINCHECK_FORMAT_END_EXPRESSION "\x1b[0m"
#define BRAINCHECK_FORMAT_START_VALUE "\x1b[39m\x1b[1m"
#define BRAINCHECK_FORMAT_END_VALUE "\x1b[0m"
#define BRAINCHECK_FORMAT_START_ASSERT "\x1b[31m\x1b[1m"
#define BRAINCHECK_FORMAT_END_ASSERT "\x1b[0m"
#endif

#ifndef BRAINCHECK_FORMAT_START_LOCATION
#define BRAINCHECK_FORMAT_START_LOCATION ""
#endif
#ifndef BRAINCHECK_FORMAT_END_LOCATION
#define BRAINCHECK_FORMAT_END_LOCATION ""
#endif
#ifndef BRAINCHECK_FORMAT_START_FUNCTION
#define BRAINCHECK_FORMAT_START_FUNCTION ""
#endif
#ifndef BRAINCHECK_FORMAT_END_FUNCTION
#define BRAINCHECK_FORMAT_END_FUNCTION ""
#endif
#ifndef BRAINCHECK_FORMAT_START_EXPRESSION
#define BRAINCHECK_FORMAT_START_EXPRESSION ""
#endif
#ifndef BRAINCHECK_FORMAT_END_EXPRESSION
#define BRAINCHECK_FORMAT_END_EXPRESSION ""
#endif
#ifndef BRAINCHECK_FORMAT_START_VALUE
#define BRAINCHECK_FORMAT_START_VALUE ""
#endif
#ifndef BRAINCHECK_FORMAT_END_VALUE
#define BRAINCHECK_FORMAT_END_VALUE ""
#endif
#ifndef BRAINCHECK_FORMAT_START_ASSERT
#define BRAINCHECK_FORMAT_START_ASSERT ""
#endif
#ifndef BRAINCHECK_FORMAT_END_ASSERT
#define BRAINCHECK_FORMAT_END_ASSERT ""
#endif

#define BRAINCHECK_FORMAT_LOCATION BRAINCHECK_FORMAT_START_LOCATION "%s:%d:" BRAINCHECK_FORMAT_END_LOCATION
#define BRAINCHECK_FORMAT_FUNCTION BRAINCHECK_FORMAT_START_FUNCTION "%s:" BRAINCHECK_FORMAT_END_FUNCTION
#define BRAINCHECK_FORMAT_EXPRESSION BRAINCHECK_FORMAT_START_EXPRESSION "%s" BRAINCHECK_FORMAT_END_EXPRESSION
#define BRAINCHECK_FORMAT_VALUE BRAINCHECK_FORMAT_START_VALUE "%s" BRAINCHECK_FORMAT_END_VALUE
#define BRAINCHECK_FORMAT_ASSERT BRAINCHECK_FORMAT_START_ASSERT "%s" BRAINCHECK_FORMAT_END_ASSERT

#endif

#ifdef BRAINCHECK_DISABLE_DEBUG
#define braincheck_debug(expr)
#define braincheck_debug_array(expr, length)
#define braincheck_debug_hexdump(expr, length)
#else

/**
 * Print value of expression where the expression is a non-array atomic value.
 */
#define braincheck_debug(expr)                      \
    _Generic((expr),                                \
             char                                   \
             : braincheck_debug_char,               \
               signed char                          \
             : braincheck_debug_signed_char,        \
               unsigned char                        \
             : braincheck_debug_unsigned_char,      \
               short                                \
             : braincheck_debug_short,              \
               unsigned short                       \
             : braincheck_debug_unsigned_short,     \
               int                                  \
             : braincheck_debug_int,                \
               unsigned int                         \
             : braincheck_debug_unsigned_int,       \
               long                                 \
             : braincheck_debug_long,               \
               unsigned long                        \
             : braincheck_debug_unsigned_long,      \
               long long                            \
             : braincheck_debug_long_long,          \
               unsigned long long                   \
             : braincheck_debug_unsigned_long_long, \
               float                                \
             : braincheck_debug_float,              \
               double                               \
             : braincheck_debug_double,             \
               bool                                 \
             : braincheck_debug_bool,               \
               default                              \
             : braincheck_debug_pointer)(__FILE__, __LINE__, BRAINCHECK_FUNCTION_NAME, #expr, expr)

#define braincheck_debug_array(expr, length)              \
    _Generic((expr),                                      \
             bool*                                        \
             : braincheck_debug_bool_array,               \
               const bool*                                \
             : braincheck_debug_bool_array,               \
               char*                                      \
             : braincheck_debug_char_array,               \
               const char*                                \
             : braincheck_debug_char_array,               \
               signed char*                               \
             : braincheck_debug_signed_char_array,        \
               const signed char*                         \
             : braincheck_debug_signed_char_array,        \
               unsigned char*                             \
             : braincheck_debug_unsigned_char_array,      \
               const unsigned char*                       \
             : braincheck_debug_unsigned_char_array,      \
               short*                                     \
             : braincheck_debug_short_array,              \
               const short*                               \
             : braincheck_debug_short_array,              \
               unsigned short*                            \
             : braincheck_debug_unsigned_short_array,     \
               const unsigned short*                      \
             : braincheck_debug_unsigned_short_array,     \
               int*                                       \
             : braincheck_debug_int_array,                \
               const int*                                 \
             : braincheck_debug_int_array,                \
               unsigned int*                              \
             : braincheck_debug_unsigned_int_array,       \
               const unsigned int*                        \
             : braincheck_debug_unsigned_int_array,       \
               long*                                      \
             : braincheck_debug_long_array,               \
               const long*                                \
             : braincheck_debug_long_array,               \
               unsigned long*                             \
             : braincheck_debug_unsigned_long_array,      \
               const unsigned long*                       \
             : braincheck_debug_unsigned_long_array,      \
               long long*                                 \
             : braincheck_debug_long_long_array,          \
               const long long*                           \
             : braincheck_debug_long_long_array,          \
               unsigned long long*                        \
             : braincheck_debug_unsigned_long_long_array, \
               const unsigned long long*                  \
             : braincheck_debug_unsigned_long_long_array, \
               float*                                     \
             : braincheck_debug_float_array,              \
               const float*                               \
             : braincheck_debug_float_array,              \
               double*                                    \
             : braincheck_debug_double_array,             \
               const double*                              \
             : braincheck_debug_double_array)(__FILE__, __LINE__, BRAINCHECK_FUNCTION_NAME, #expr, expr, length)

/**
 * Print value of array expression as a hexdump.
 */
#define braincheck_debug_hexdump(expr, length)              \
    _Generic((expr),                                        \
             bool*                                          \
             : braincheck_debug_bool_hexdump,               \
               const bool*                                  \
             : braincheck_debug_bool_hexdump,               \
               char*                                        \
             : braincheck_debug_char_hexdump,               \
               const char*                                  \
             : braincheck_debug_char_hexdump,               \
               signed char*                                 \
             : braincheck_debug_signed_char_hexdump,        \
               const signed char*                           \
             : braincheck_debug_signed_char_hexdump,        \
               unsigned char*                               \
             : braincheck_debug_unsigned_char_hexdump,      \
               const unsigned char*                         \
             : braincheck_debug_unsigned_char_hexdump,      \
               short*                                       \
             : braincheck_debug_short_hexdump,              \
               const short*                                 \
             : braincheck_debug_short_hexdump,              \
               unsigned short*                              \
             : braincheck_debug_unsigned_short_hexdump,     \
               const unsigned short*                        \
             : braincheck_debug_unsigned_short_hexdump,     \
               int*                                         \
             : braincheck_debug_int_hexdump,                \
               const int*                                   \
             : braincheck_debug_int_hexdump,                \
               unsigned int*                                \
             : braincheck_debug_unsigned_int_hexdump,       \
               const unsigned int*                          \
             : braincheck_debug_unsigned_int_hexdump,       \
               long*                                        \
             : braincheck_debug_long_hexdump,               \
               const long*                                  \
             : braincheck_debug_long_hexdump,               \
               unsigned long*                               \
             : braincheck_debug_unsigned_long_hexdump,      \
               const unsigned long*                         \
             : braincheck_debug_unsigned_long_hexdump,      \
               long long*                                   \
             : braincheck_debug_long_long_hexdump,          \
               const long long*                             \
             : braincheck_debug_long_long_hexdump,          \
               unsigned long long*                          \
             : braincheck_debug_unsigned_long_long_hexdump, \
               const unsigned long long*                    \
             : braincheck_debug_unsigned_long_long_hexdump, \
               float*                                       \
             : braincheck_debug_float_hexdump,              \
               const float*                                 \
             : braincheck_debug_float_hexdump,              \
               double*                                      \
             : braincheck_debug_double_hexdump,             \
               const double*                                \
             : braincheck_debug_double_hexdump)(__FILE__, __LINE__, BRAINCHECK_FUNCTION_NAME, #expr, expr, length)

static inline void braincheck_hexdump_ascii(const char* data, const unsigned int size)
{
    const unsigned row_width = 16;
    const unsigned column_group_size = 8;

    // Add whitespace to align ascii section
    for (unsigned int i = 0; i < row_width - size; i++) {
        BRAINCHECK_PRINTF("   ");
    }
    if (size <= column_group_size) {
        BRAINCHECK_PRINTF(" ");
    }

    BRAINCHECK_PRINTF("'");

    for (unsigned int i = 0; i < size; i++) {
        BRAINCHECK_PRINTF("%c", isprint((int)data[i]) ? data[i] : '.');
    }

    BRAINCHECK_PRINTF("'");
}

static inline void braincheck_hexdump(const char* file, const int line, const char* function, const char* expression, const void* value, const unsigned int length)
{
    BRAINCHECK_PRINTF(BRAINCHECK_FORMAT_LOCATION " " BRAINCHECK_FORMAT_FUNCTION " " BRAINCHECK_FORMAT_EXPRESSION " (length = %u)\n" BRAINCHECK_FORMAT_START_VALUE, file, line, function, expression, length);

    const unsigned row_width = 16;
    const unsigned column_group_size = 8;
    const char* data = (const char*)value;
    unsigned int i = 0;

    while (i < length) {
        unsigned int column = i % row_width;
        if (0 == column) {
            // Address header
            BRAINCHECK_PRINTF("    %08x: ", i);
        } else if (0 == (column % column_group_size)) {
            BRAINCHECK_PRINTF(" ");
        }
        BRAINCHECK_PRINTF("%02x ", data[i] & 0xFF);

        if (column == (row_width - 1)) {
            braincheck_hexdump_ascii(&data[i - 15], 16);
            BRAINCHECK_PRINTF("\n");
        }

        i++;
    }

    unsigned int last_row = i % row_width;
    if (0 != last_row) {
        braincheck_hexdump_ascii(&data[i - last_row], last_row);
        BRAINCHECK_PRINTF("\n");
    }
    BRAINCHECK_PRINTF(BRAINCHECK_FORMAT_END_FUNCTION);
}

#define BRAINCHECK_INTERNAL_DEBUG_FUNC(name, type, formatter_specifier)                                                                                                                                                                                  \
    static inline void braincheck_debug_##name(const char* file, const int line, const char* function, const char* expression, const type value)                                                                                                         \
    {                                                                                                                                                                                                                                                    \
        BRAINCHECK_PRINTF(BRAINCHECK_FORMAT_LOCATION " " BRAINCHECK_FORMAT_FUNCTION " " BRAINCHECK_FORMAT_EXPRESSION " = " BRAINCHECK_FORMAT_START_VALUE formatter_specifier BRAINCHECK_FORMAT_END_VALUE "\n", file, line, function, expression, value); \
    }                                                                                                                                                                                                                                                    \
    static inline void braincheck_debug_##name##_array(const char* file, const int line, const char* function, const char* expression, const type* value, const unsigned int length)                                                                     \
    {                                                                                                                                                                                                                                                    \
        BRAINCHECK_PRINTF(BRAINCHECK_FORMAT_LOCATION " " BRAINCHECK_FORMAT_FUNCTION " " BRAINCHECK_FORMAT_EXPRESSION " = " BRAINCHECK_FORMAT_START_VALUE "[", file, line, function, expression);                                                         \
        for (int i = 0; i < length; i++) {                                                                                                                                                                                                               \
            if (0 != i) {                                                                                                                                                                                                                                \
                BRAINCHECK_PRINTF(", ");                                                                                                                                                                                                                 \
            }                                                                                                                                                                                                                                            \
            BRAINCHECK_PRINTF(formatter_specifier, value[i]);                                                                                                                                                                                            \
        }                                                                                                                                                                                                                                                \
        BRAINCHECK_PRINTF("]" BRAINCHECK_FORMAT_END_VALUE "\n");                                                                                                                                                                                         \
    }                                                                                                                                                                                                                                                    \
    static inline void braincheck_debug_##name##_hexdump(const char* file, const int line, const char* function, const char* expression, const type* value, const unsigned int length)                                                                   \
    {                                                                                                                                                                                                                                                    \
        braincheck_hexdump(file, line, function, expression, (const char*)value, length);                                                                                                                                                                \
    }

BRAINCHECK_INTERNAL_DEBUG_FUNC(char, char, "%c");
BRAINCHECK_INTERNAL_DEBUG_FUNC(signed_char, signed char, "%c");
BRAINCHECK_INTERNAL_DEBUG_FUNC(unsigned_char, unsigned char, "%c");
BRAINCHECK_INTERNAL_DEBUG_FUNC(short, short, "%hd");
BRAINCHECK_INTERNAL_DEBUG_FUNC(unsigned_short, unsigned short, "%hu");
BRAINCHECK_INTERNAL_DEBUG_FUNC(int, int, "%d");
BRAINCHECK_INTERNAL_DEBUG_FUNC(unsigned_int, unsigned int, "%u");
BRAINCHECK_INTERNAL_DEBUG_FUNC(long, long, "%ld");
BRAINCHECK_INTERNAL_DEBUG_FUNC(unsigned_long, unsigned long, "%lu");
BRAINCHECK_INTERNAL_DEBUG_FUNC(long_long, long long, "%lld");
BRAINCHECK_INTERNAL_DEBUG_FUNC(unsigned_long_long, unsigned long long, "%llu");
BRAINCHECK_INTERNAL_DEBUG_FUNC(float, float, "%f");
BRAINCHECK_INTERNAL_DEBUG_FUNC(double, double, "%f");

static inline void braincheck_debug_bool(const char* file, const int line, const char* function, const char* expression, const bool value)
{
    BRAINCHECK_PRINTF(BRAINCHECK_FORMAT_LOCATION " " BRAINCHECK_FORMAT_FUNCTION " " BRAINCHECK_FORMAT_EXPRESSION " = " BRAINCHECK_FORMAT_VALUE "\n", file, line, function, expression, value ? "true" : "false");
}

static inline void braincheck_debug_bool_array(const char* file, const int line, const char* function, const char* expression, const bool* value, const unsigned int length)
{
    BRAINCHECK_PRINTF(BRAINCHECK_FORMAT_LOCATION " " BRAINCHECK_FORMAT_FUNCTION " " BRAINCHECK_FORMAT_EXPRESSION " = " BRAINCHECK_FORMAT_START_VALUE "[", file, line, function, expression);
    for (unsigned int i = 0; i < length; i++) {
        if (0 != i) {
            BRAINCHECK_PRINTF(", ");
        }
        BRAINCHECK_PRINTF("%s", value[i] ? "true" : "false");
    }
    BRAINCHECK_PRINTF(BRAINCHECK_FORMAT_END_VALUE "]\n");
}

static inline void braincheck_debug_bool_hexdump(const char* file, const int line, const char* function, const char* expression, const bool* value, const unsigned int length)
{
    braincheck_hexdump(file, line, function, expression, (const char*)value, length);
}

static inline void braincheck_debug_pointer(const char* file, const int line, const char* function, const char* expression, const void* value)
{
    BRAINCHECK_PRINTF(BRAINCHECK_FORMAT_LOCATION " " BRAINCHECK_FORMAT_FUNCTION " " BRAINCHECK_FORMAT_EXPRESSION " = " BRAINCHECK_FORMAT_START_VALUE "%p" BRAINCHECK_FORMAT_END_VALUE "\n", file, line, function, expression, value);
}

#undef BRAINCHECK_INTERNAL_DEBUG_FUNC

#endif

#ifdef BRAINCHECK_DISABLE_PERROR
#define braincheck_perror(label)
#else

#define braincheck_perror(label) braincheck_error(__FILE__, __LINE__, BRAINCHECK_FUNCTION_NAME, label, errno)

static inline void braincheck_error(const char* file, const int line, const char* function, const char* label, const int error)
{
    BRAINCHECK_PRINTF(BRAINCHECK_FORMAT_LOCATION " " BRAINCHECK_FORMAT_FUNCTION " " BRAINCHECK_FORMAT_EXPRESSION ": " BRAINCHECK_FORMAT_VALUE "\n", file, line, function, label, strerror(error));
}
#endif

#ifdef BRAINCHECK_DISABLE_BACKTRACE
#define braincheck_backtrace_n(max_frame_count)
#define braincheck_backtrace()
#else

#ifndef BRAINCHECK_MAX_BACKTRACE_FRAMES
#define BRAINCHECK_MAX_BACKTRACE_FRAMES 128
#endif

/**
 * Print a backtrace with at most max_frame_count frames.
 */
#define braincheck_backtrace_n(max_frame_count)                                         \
    do {                                                                                \
        void* callstack[(max_frame_count) + 1];                                         \
                                                                                        \
        braincheck_internal_backtrace(__FILE__,                                         \
                                      __LINE__,                                         \
                                      BRAINCHECK_FUNCTION_NAME,                         \
                                      &callstack[0],                                    \
                                      backtrace(&callstack[0], (max_frame_count) + 1)); \
    } while (0)

/**
 * Print a backtrace with at most BRAINCHECK_MAX_BACKTRACE_FRAMES frames.
 */
#define braincheck_backtrace() braincheck_backtrace_n(BRAINCHECK_MAX_BACKTRACE_FRAMES)

static inline void braincheck_internal_backtrace(const char* file, int line, const char* function, void** callstack_buffer, const int max_callstack_depth)
{
    BRAINCHECK_PRINTF(BRAINCHECK_FORMAT_LOCATION " " BRAINCHECK_FORMAT_FUNCTION "\n", file, line, function);
    char** callstack = backtrace_symbols(callstack_buffer, max_callstack_depth);
    if (NULL != callstack) {
        BRAINCHECK_PRINTF(BRAINCHECK_FORMAT_START_VALUE);
        // Starting at 1 skips the current frame which should be ok given the function parameter indicates frame
        for (int i = 1; i < max_callstack_depth; i++) {
            BRAINCHECK_PRINTF("  %s\n", callstack[i]);
        }
        BRAINCHECK_PRINTF(BRAINCHECK_FORMAT_END_FUNCTION);

        free(callstack);
    }
}
#endif

#ifdef BRAINCHECK_DISABLE_ASSERT

#define braincheck_assert_fail(message)

#define braincheck_assert(v)
#define braincheck_assert_nonnull(v)
#define braincheck_assert_eq(v1, v2)
#define braincheck_assert_neq(v1, v2)
#define braincheck_assert_lt(v1, v2)
#define braincheck_assert_lte(v1, v2)
#define braincheck_assert_gt(v1, v2)
#define braincheck_assert_gte(v1, v2)

#define braincheck_assert_m(v1, message)
#define braincheck_assert_nonnull_m(v, message)
#define braincheck_assert_eq_m(v1, v2, message)
#define braincheck_assert_neq_m(v1, v2, message)
#define braincheck_assert_lt_m(v1, v2, message)
#define braincheck_assert_lte_m(v1, v2, message)
#define braincheck_assert_gt_m(v1, v2, message)
#define braincheck_assert_gte_m(v1, v2, message)

#define braincheck_once_assert_fail(message)

#define braincheck_once_assert(v)
#define braincheck_once_assert_nonnull(v)
#define braincheck_once_assert_eq(v1, v2)
#define braincheck_once_assert_neq(v1, v2)
#define braincheck_once_assert_lt(v1, v2)
#define braincheck_once_assert_lte(v1, v2)
#define braincheck_once_assert_gt(v1, v2)
#define braincheck_once_assert_gte(v1, v2)

#define braincheck_once_assert_m(v1, message)
#define braincheck_once_assert_nonnull_m(v1, message)
#define braincheck_once_assert_eq_m(v1, v2, message)
#define braincheck_once_assert_neq_m(v1, v2, message)
#define braincheck_once_assert_lt_m(v1, v2, message)
#define braincheck_once_assert_lte_m(v1, v2, message)
#define braincheck_once_assert_gt_m(v1, v2, message)
#define braincheck_once_assert_gte_m(v1, v2, message)

#else

#define BRAINCHECK_VALUE_FORMAT(expr) \
    _Generic((expr),                  \
             char                     \
             : "%c",                  \
               signed char            \
             : "%c",                  \
               unsigned char          \
             : "%c",                  \
               short                  \
             : "%hd",                 \
               unsigned short         \
             : "%hu",                 \
               int                    \
             : "%d",                  \
               unsigned int           \
             : "%u",                  \
               long                   \
             : "%ld",                 \
               unsigned long          \
             : "%lu",                 \
               long long              \
             : "%lld",                \
               unsigned long long     \
             : "%llu",                \
               float                  \
             : "%f",                  \
               double                 \
             : "%lf",                 \
               bool                   \
             : "%s",                  \
               default                \
             : "%p")

#define BRAINCHECK_VALUE(expr)              \
    _Generic((expr),                        \
             bool                           \
             : ((expr) ? "true" : "false"), \
               default                      \
             : (expr))

// The compiler should predict that the expression is true. Used when predicting branches.
#if defined(__clang__) || defined(__GNUC__)
#define BRAINCHECK_UNLIKELY(expr) __builtin_expect(!!(expr), 0)
#else
#define BRAINCHECK_UNLIKELY(expr) (!!(expr))
#endif

#define braincheck_assert_comparison(op, v1, v2)                                               \
    do {                                                                                       \
        if (BRAINCHECK_UNLIKELY(!((v1)op(v2)))) {                                              \
            char v1Str[50];                                                                    \
            char v2Str[50];                                                                    \
            snprintf(v1Str, sizeof(v1Str), BRAINCHECK_VALUE_FORMAT(v1), BRAINCHECK_VALUE(v1)); \
            snprintf(v2Str, sizeof(v2Str), BRAINCHECK_VALUE_FORMAT(v2), BRAINCHECK_VALUE(v2)); \
            braincheck_internal_fail(__FILE__,                                                 \
                                     __LINE__,                                                 \
                                     BRAINCHECK_FUNCTION_NAME,                                 \
                                     NULL,                                                     \
                                     "Failed assertion: v1 " #op " v2 (v1 = %s, v2 = %s)",     \
                                     v1Str,                                                    \
                                     v2Str);                                                   \
        }                                                                                      \
    } while (0)

#define braincheck_assert_comparison_m(op, v1, v2, message)                                    \
    do {                                                                                       \
        if (BRAINCHECK_UNLIKELY(!((v1)op(v2)))) {                                              \
            char v1Str[50];                                                                    \
            char v2Str[50];                                                                    \
            snprintf(v1Str, sizeof(v1Str), BRAINCHECK_VALUE_FORMAT(v1), BRAINCHECK_VALUE(v1)); \
            snprintf(v2Str, sizeof(v2Str), BRAINCHECK_VALUE_FORMAT(v2), BRAINCHECK_VALUE(v2)); \
            braincheck_internal_fail(__FILE__,                                                 \
                                     __LINE__,                                                 \
                                     BRAINCHECK_FUNCTION_NAME,                                 \
                                     message,                                                  \
                                     "Failed assertion: v1 " #op " v2 (v1 = %s, v2 = %s)",     \
                                     v1Str,                                                    \
                                     v2Str);                                                   \
        }                                                                                      \
    } while (0)

#define braincheck_assert(v)                                                                 \
    do {                                                                                     \
        if (BRAINCHECK_UNLIKELY(!(v))) {                                                     \
            char v1Str[50];                                                                  \
            snprintf(v1Str, sizeof(v1Str), BRAINCHECK_VALUE_FORMAT(v), BRAINCHECK_VALUE(v)); \
            braincheck_internal_fail(__FILE__,                                               \
                                     __LINE__,                                               \
                                     BRAINCHECK_FUNCTION_NAME,                               \
                                     NULL,                                                   \
                                     "Failed assertion: " #v " => %s",                       \
                                     v1Str);                                                 \
        }                                                                                    \
    } while (0)

#define braincheck_assert_m(v, message)                                                      \
    do {                                                                                     \
        if (BRAINCHECK_UNLIKELY(!(v))) {                                                     \
            char v1Str[50];                                                                  \
            snprintf(v1Str, sizeof(v1Str), BRAINCHECK_VALUE_FORMAT(v), BRAINCHECK_VALUE(v)); \
            braincheck_internal_fail(__FILE__,                                               \
                                     __LINE__,                                               \
                                     BRAINCHECK_FUNCTION_NAME,                               \
                                     message,                                                \
                                     "Failed assertion: " #v " => %s",                       \
                                     v1Str);                                                 \
        }                                                                                    \
    } while (0)

#define braincheck_assert_nonnull(v)                                      \
    do {                                                                  \
        if (BRAINCHECK_UNLIKELY(NULL == (v))) {                           \
            braincheck_internal_fail(__FILE__,                            \
                                     __LINE__,                            \
                                     BRAINCHECK_FUNCTION_NAME,            \
                                     NULL,                                \
                                     "Failed assertion: " #v " => NULL"); \
        }                                                                 \
    } while (0)

#define braincheck_assert_nonnull_m(v, message)                           \
    do {                                                                  \
        if (BRAINCHECK_UNLIKELY(NULL == (v))) {                           \
            braincheck_internal_fail(__FILE__,                            \
                                     __LINE__,                            \
                                     BRAINCHECK_FUNCTION_NAME,            \
                                     message,                             \
                                     "Failed assertion: " #v " => NULL"); \
        }                                                                 \
    } while (0)

// clang-format off
#define braincheck_assert_fail(message) braincheck_internal_fail(__FILE__, __LINE__, BRAINCHECK_FUNCTION_NAME, NULL, "Failed assertion: %s", message)

#define braincheck_assert_eq(v1, v2) braincheck_assert_comparison(==, v1, v2)
#define braincheck_assert_neq(v1, v2) braincheck_assert_comparison(!=, v1, v2)
#define braincheck_assert_lt(v1, v2) braincheck_assert_comparison(<, v1, v2)
#define braincheck_assert_lte(v1, v2) braincheck_assert_comparison(<=, v1, v2)
#define braincheck_assert_gt(v1, v2) braincheck_assert_comparison(>, v1, v2)
#define braincheck_assert_gte(v1, v2) braincheck_assert_comparison(>=, v1, v2)

#define braincheck_assert_eq_m(v1, v2, message) braincheck_assert_comparison_m(==, v1, v2, message)
#define braincheck_assert_neq_m(v1, v2, message) braincheck_assert_comparison_m(!=, v1, v2, message)
#define braincheck_assert_lt_m(v1, v2, message) braincheck_assert_comparison_m(<, v1, v2, message)
#define braincheck_assert_lte_m(v1, v2, message) braincheck_assert_comparison_m(<=, v1, v2, message)
#define braincheck_assert_gt_m(v1, v2, message) braincheck_assert_comparison_m(>, v1, v2, message)
#define braincheck_assert_gte_m(v1, v2, message) braincheck_assert_comparison_m(>=, v1, v2, message)

#define braincheck_once_assert_fail(message) do { static bool print = true; if (print) { braincheck_assert_fail(message); print = false; } } while(0)

#define braincheck_once_assert(v) do { static bool print = true; if (print) {braincheck_assert(v); print = false; } } while(0)
#define braincheck_once_assert_nonnull(v) do { static bool print = true; if (print) {braincheck_assert_nonnull(v); print = false; } } while(0)
#define braincheck_once_assert_eq(v1, v2) do { static bool print = true; if (print) {braincheck_assert_eq(v1, v2); print = false; } } while(0)
#define braincheck_once_assert_neq(v1, v2) do { static bool print = true; if (print) {braincheck_assert_neq(v1, v2); print = false; } } while(0)
#define braincheck_once_assert_lt(v1, v2) do { static bool print = true; if (print) {braincheck_assert_lt(v1, v2); print = false; } } while(0)
#define braincheck_once_assert_lte(v1, v2) do { static bool print = true; if (print) {braincheck_assert_lte(v1, v2); print = false; } } while(0)
#define braincheck_once_assert_gt(v1, v2) do { static bool print = true; if (print) {braincheck_assert_gt(v1, v2); print = false; } } while(0)
#define braincheck_once_assert_gte(v1, v2) do { static bool print = true; if (print) {braincheck_assert_gte(v1, v2); print = false; } } while(0)

#define braincheck_once_assert_m(v1, message) do { static bool print = true; if (print) {braincheck_assert_m(v1, message); print = false; } } while(0)
#define braincheck_once_assert_nonnull_m(v1, message) do { static bool print = true; if (print) {braincheck_assert_nonnull_m(v1, message); print = false; } } while(0)
#define braincheck_once_assert_eq_m(v1, v2, message) do { static bool print = true; if (print) {braincheck_assert_eq_m(v1, v2, message); print = false; } } while(0)
#define braincheck_once_assert_neq_m(v1, v2, message) do { static bool print = true; if (print) {braincheck_assert_neq_m(v1, v2, message); print = false; } } while(0)
#define braincheck_once_assert_lt_m(v1, v2, message) do { static bool print = true; if (print) {braincheck_assert_lt_m(v1, v2, message); print = false; } } while(0)
#define braincheck_once_assert_lte_m(v1, v2, message) do { static bool print = true; if (print) {braincheck_assert_lte_m(v1, v2, message); print = false; } } while(0)
#define braincheck_once_assert_gt_m(v1, v2, message) do { static bool print = true; if (print) {braincheck_assert_gt_m(v1, v2, message); print = false; } } while(0)
#define braincheck_once_assert_gte_m(v1, v2, message) do { static bool print = true; if (print) {braincheck_assert_gte_m(v1, v2, message); print = false; } } while(0)

// clang-format on

static BRAINCHECK_PRINTF_FUNCTION(5, 6) inline void braincheck_internal_fail(const char* file, int line, const char* function, const char* user_message, const char* format, ...)
{
    char message[256];
    va_list args;
    va_start(args, format);
    vsnprintf(message, sizeof(message), format, args);
    va_end(args);
    if (NULL == user_message) {
        BRAINCHECK_PRINTF(BRAINCHECK_FORMAT_LOCATION " " BRAINCHECK_FORMAT_FUNCTION " " BRAINCHECK_FORMAT_ASSERT "\n", file, line, function, message);
    } else {
        BRAINCHECK_PRINTF(BRAINCHECK_FORMAT_LOCATION " " BRAINCHECK_FORMAT_FUNCTION " " BRAINCHECK_FORMAT_ASSERT ": %s\n", file, line, function, message, user_message);
    }
#ifdef BRAINCHECK_ASSERTS_FATAL
    assert(0);
#endif
}

#endif

#ifdef __cplusplus
}
#endif

#endif
