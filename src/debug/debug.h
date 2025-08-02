#ifndef DEBUG_H
#define DEBUG_H

#include "hardware/sync.h"
#include "pico/types.h"

typedef enum debug_data_type {
    DEBUG_BOOL = 0,
    DEBUG_FLOAT = 1,
    DEBUG_INT = 2,
    DEBUG_COLOR = 3,
} debug_data_type;

typedef struct debug_type {
    debug_data_type ty;

    union {
        struct _debug_float_range {
            float min, max;
        } float_range;

        struct _debug_int_data {
            enum _debug_int_ty {
                DEBUG_U8,
                DEBUG_U16,
                DEBUG_U32,

                DEBUG_I8,
                DEBUG_I16,
                DEBUG_I32,
            } ty;

            int64_t min, max;
        } int_data;
    };
} debug_type;

bool debug_connect_server(void);
void debug_printf(const char* format, ...);

// macro that counts the number of its arguments
#define _PP_NARG(...) _PP_NARG_(__VA_ARGS__, _PP_RSEQ_N())
#define _PP_NARG_(...) _PP_NARG_N(__VA_ARGS__)

#define _PP_NARG_N(                                                                      \
    _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16,          /**/ \
    _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, /**/ \
    _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, /**/ \
    _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, N, ...    \
)                                                                                        \
    N

#define _PP_RSEQ_N()                                                     \
    63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48, /**/ \
        47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, /**/ \
        32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, /**/ \
        17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0

#define _ARG0(_0, ...) _0
#define _ARG1(_0, _1, ...) _1

#define _DEBUG_EXP_ARGS(var) \
    _Generic(                \
        (var),               \
        uint8_t: 3,          \
        uint16_t: 3,         \
        uint32_t: 3,         \
                             \
        int8_t: 3,           \
        int16_t: 3,          \
        int32_t: 3,          \
                             \
        float: 3,            \
                             \
        bool: 1,             \
        hsv_color: 1         \
    )

#define _DEBUG_VAR_MAX_NAME 64

void debug_add_remote_var_fn(volatile void* ptr, const char* name, debug_type type);

// for ints and floats a min and a max value are needed
#define debug_add_remote_var(var, ...)                                                                            \
    do {                                                                                                          \
        static_assert(                                                                                            \
            sizeof(#var) - 1 < _DEBUG_VAR_MAX_NAME,                                                               \
            "Debug variable name longer than maximum (" _UTIL_EXPAND_AND_QUOTE(_DEBUG_VAR_MAX_NAME) ")"           \
        );                                                                                                        \
        static_assert(                                                                                            \
            !(_PP_NARG(0 __VA_OPT__(, __VA_ARGS__)) < _DEBUG_EXP_ARGS(var)),                                      \
            "debug_add_remote_var expects min and max values for ints and floats"                                 \
        );                                                                                                        \
        static_assert(                                                                                            \
            !(_PP_NARG(0 __VA_OPT__(, __VA_ARGS__)) > _DEBUG_EXP_ARGS(var)),                                      \
            "too many args for debug_add_remote_var, only the variable and min, max are needed (if int or float)" \
        );                                                                                                        \
                                                                                                                  \
        debug_add_remote_var_fn(                                                                                  \
            (volatile void*) &var, #var,                                                                          \
            _Generic(                                                                                             \
                (var),                                                                                            \
                uint8_t: (debug_type) { .ty = DEBUG_INT,                                                          \
                                        __VA_OPT__(                                                               \
                                                .int_data = { .ty = DEBUG_U8,                                     \
                                                              .min = (uint8_t) (_ARG0(__VA_ARGS__)),              \
                                                              .max = (uint8_t) (_ARG1(__VA_ARGS__)) }             \
                                        ) },                                                                      \
                uint16_t: (debug_type) { .ty = DEBUG_INT,                                                         \
                                         __VA_OPT__(                                                              \
                                                 .int_data = { .ty = DEBUG_U16,                                   \
                                                               .min = (uint16_t) (_ARG0(__VA_ARGS__)),            \
                                                               .max = (uint16_t) (_ARG1(__VA_ARGS__)) }           \
                                         ) },                                                                     \
                uint32_t: (debug_type) { .ty = DEBUG_INT,                                                         \
                                         __VA_OPT__(                                                              \
                                                 .int_data = { .ty = DEBUG_U32,                                   \
                                                               .min = (uint32_t) (_ARG0(__VA_ARGS__)),            \
                                                               .max = (uint32_t) (_ARG1(__VA_ARGS__)) }           \
                                         ) },                                                                     \
                                                                                                                  \
                int8_t: (debug_type) { .ty = DEBUG_INT,                                                           \
                                       __VA_OPT__(                                                                \
                                               .int_data = { .ty = DEBUG_I8,                                      \
                                                             .min = (int8_t) (_ARG0(__VA_ARGS__)),                \
                                                             .max = (int8_t) (_ARG1(__VA_ARGS__)) }               \
                                       ) },                                                                       \
                int16_t: (debug_type) { .ty = DEBUG_INT,                                                          \
                                        __VA_OPT__(                                                               \
                                                .int_data = { .ty = DEBUG_I16,                                    \
                                                              .min = (int16_t) (_ARG0(__VA_ARGS__)),              \
                                                              .max = (int16_t) (_ARG1(__VA_ARGS__)) }             \
                                        ) },                                                                      \
                int32_t: (debug_type) { .ty = DEBUG_INT,                                                          \
                                        __VA_OPT__(                                                               \
                                                .int_data = { .ty = DEBUG_I32,                                    \
                                                              .min = (int32_t) (_ARG0(__VA_ARGS__)),              \
                                                              .max = (int32_t) (_ARG1(__VA_ARGS__)) }             \
                                        ) },                                                                      \
                                                                                                                  \
                float: (debug_type) { .ty = DEBUG_FLOAT,                                                          \
                                      __VA_OPT__(                                                                 \
                                              .float_range = { .min = (float) (_ARG0(__VA_ARGS__)),               \
                                                               .max = (float) (_ARG1(__VA_ARGS__)) }              \
                                      ) },                                                                        \
                                                                                                                  \
                bool: (debug_type) { .ty = DEBUG_BOOL },                                                          \
                hsv_color: (debug_type) { .ty = DEBUG_COLOR }                                                     \
            )                                                                                                     \
        );                                                                                                        \
    } while (0)

void debug_send_remote_vars(void);

int32_t debug_parse_val_recv(const uint8_t* buf, size_t buf_size);

#define DEBUG_DISABLE_IRQ                                                             \
    for (uint32_t __run = 1, __saved_irq_status = save_and_disable_interrupts(); /**/ \
         __run == 1;                                                             /**/ \
         restore_interrupts_from_disabled(__saved_irq_status), __run = 0)

#endif
