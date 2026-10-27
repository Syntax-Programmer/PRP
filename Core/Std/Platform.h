#ifndef DEF_STD_PLATFORM_
#define DEF_STD_PLATFORM_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Compile-time configuration macros recognized by PRP.
 *
 * These macros may be defined externally (e.g., via a compiler -D flag or a
 * build system) before including any PRP header to control engine behavior:
 *
 * PRP_NDEBUG
 *     Builds PRP in release mode. Disables debug-only diagnostics,
 *     assertions, and validation checks.
 *
 * PRP_WINDOW_BACKEND_USE_WAYLAND
 *     When building on Linux, selects the Wayland window system backend.
 *     If absent, X11 is used as the default Linux backend.
 *
 * PRP_BUILD_DLL
 *     Indicates that PRP itself is being compiled as a shared library.
 *     Public symbols are decorated for export.
 *
 * PRP_USE_DLL
 *     Indicates that a consumer is linking against PRP as a shared library.
 *     Public symbols are decorated for import.
 */

/* ----  COMPILE MODE SELECTION ---- */

#ifdef PRP_NDEBUG
    #define STD_COMPILE_MODE_RELEASE 1
#else
    #define STD_COMPILE_MODE_DEBUG 1
#endif

/* ----  OS DETECTION ---- */

#if defined(_WIN32) || defined(_WIN64)
    #define STD_PLATFORM_WINDOW 1
#elifdef __ANDROID__
    #define STD_PLATFORM_ANDROID 1
#elifdef __linux__
    #define STD_PLATFORM_LINUX 1
#elifdef __APPLE__
    #include <TargetConditionals.h>
    #ifdef TARGET_OS_IPHONE
        #define STD_PLATFORM_IOS 1
    #else
        #define STD_PLATFORM_MACOS 1
    #endif
#else
    #error Unsupported platform detected.
#endif

/* ---- WINDOW SYSTEM BACKEND DETECTION ---- */

#ifdef STD_PLATFORM_WINDOWS
    #define STD_WINDOW_BACKEND_WIN32 1
#elifdef STD_PLATFORM_MACOS
    #define STD_WINDOW_BACKEND_COCOA 1
#elifdef STD_PLATFORM_LINUX
    #ifdef PRP_WINDOW_BACKEND_USE_WAYLAND
        #define STD_WINDOW_BACKEND_WAYLAND 1
    #else
        #define STD_WINDOW_BACKEND_USE_X11 1
    #endif
#elifdef STD_PLATFORM_ANDROID
    #define STD_WINDOW_BACKEND_ANDROID 1
#elifdef STD_PLAFORM_IOS
    #define STD_WINDOW_BACKEND_UIKIT 1
#elifdef STD_PLATFORM_WEB
    #define STD_WINDOW_BACKEND_EMSCRIPTEN 1
#else
    #error Unsupported window backend detected.
#endif

/* ---- COMPILER DETECTION ---- */

#ifdef __clang__
    #define STD_COMPILER_CLANG 1
#elifdef __GNUC__
    #define STD_COMPILER_GCC 1
#elifdef _MSC_VER
    #define STD_COMPILER_MSVC 1
#else
    #error Unsupported compiler detected.
#endif

/* ---- C STANDARD DETECTION ---- */

#ifdef __cplusplus
/* Compiling as C++, skip C standard checks. */
#else
    #if !defined(__STDC_VERSION__) || (__STDC_VERSION__ < 202311L)
        #error PRP requires C23 or newer.
    #elif __STDC_VERSION__ >= 202311L
        #define STD_STANDARD_C23 1
    #endif
#endif

/* ----  SHARED LIBRARY SUPPORT ---- */

/**
 * Symbol export attribute for the current compiler/platform.
 *
 * On Windows, expands to __declspec(dllexport).
 * On GCC/Clang, expands to __attribute__((visibility("default"))).
 * Applied to public symbols when PRP is being built as a shared library.
 */
#ifdef STD_PLATFORM_WINDOWS
    #define STD_ATTR_EXPORT __declspec(dllexport)
    #define STD_ATTR_IMPORT __declspec(dllimport)
#else
    #define STD_ATTR_EXPORT __attribute__((visibility("default")))
    #define STD_ATTR_IMPORT
#endif

/* ---- API ---- */

#if defined(PRP_BUILD_DLL)
    #define STD_API STD_ATTR_EXPORT
#elif defined(PRP_USE_DLL)
    #define STD_API STD_ATTR_IMPORT
#else
    #define STD_API
#endif

/* ---- CALLING ---- */

#ifdef STD_COMPILER_MSVC
    #define STD_CDECL    __cdecl
    #define STD_STDCALL  __stdcall
    #define STD_FASTCALL __fastcall
#else
    #define STD_CDECL
    #define STD_STDCALL
    #define STD_FASTCALL
#endif

#define PRP_CALL PRP_CDECL

/* ----  VARADIC ARG ATTR THING ---- */

#if defined(STD_COMPILER_CLANG) || defined(STD_COMPILER_GCC)
    #define STD_PRINTF_FORMAT(fmt_idx, arg_idx)                                \
        __attribute__((format(printf, fmt_idx, arg_idx)))
#else
    #define STD_PRINTF_FORMAT(fmt_idx, arg_idx)
#endif

/* ----  FORCEINLINE and NOINLINE ---- */

#ifdef STD_COMPILER_MSVC
    #define STD_ATTR_FORCEINLINE __forceinline
    #define STD_ATTR_NOINLINE    __declspec(noinline)
#elif defined(STD_COMPILER_CLANG) || defined(STD_COMPILER_GCC)
    #define STD_ATTR_FORCEINLINE __attribute__((always_inline))
    #define STD_ATTR_NOINLINE    __attribute__((noinline))
#else
    #define STD_ATTR_FORCEINLINE
    #define STD_ATTR_NOINLINE
#endif

/* ---- RESTRICT ---- */

#if defined(STD_COMPILER_MSVC)
    #define STD_RESTRICT __restrict
#else
    #define STD_RESTRICT restrict
#endif

/* ---- ABORT ---- */

#define STD_ABORT() abort()

/* ---- UNREACHABLE ---- */

#if defined(STD_COMPILER_MSVC)
    #define STD_UNREACHABLE() __assume(0)
#elif defined(STD_COMPILER_CLANG) || defined(STD_COMPILER_GCC)
    #define STD_UNREACHABLE() __builtin_unreachable()
#else
    #define STD_UNREACHABLE() abort()
#endif

/* ---- DEBUG BREAK ---- */

#ifdef STD_COMPILER_MSVC
    #include <intrin.h>
    #define STD_DEBUG_BREAK() __debugbreak()

#elifdef __has_builtin
    #if __has_builtin(__builtin_debugtrap)
        #define STD_DEBUG_BREAK() __builtin_debugtrap()
    #elif __has_builtin(__builtin_trap)
        #define STD_DEBUG_BREAK() __builtin_trap()
    #else
        #define STD_DEBUG_BREAK() abort()
    #endif

#elif defined(STD_COMPILER_CLANG) || defined(STD_COMPILER_GCC)
    #define STD_DEBUG_BREAK() __builtin_trap()

#else
    #define STD_DEBUG_BREAK() abort()
#endif

/* ---- BRANCH PREDICTION ---- */

#if defined(STD_COMPILER_GCC) || defined(STD_COMPILER_CLANG)
    #define STD_LIKELY(x)   __builtin_expect(!!(x), 1)
    #define STD_UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
    #define STD_LIKELY(x)   (x)
    #define STD_UNLIKELY(x) (x)
#endif

/* ---- ASSUMPTION ---- */

#ifdef STD_COMPILER_MSVC
    #define STD_ASSUME(x) __assume(x)
#else
    #define STD_ASSUME(x)                                                      \
        do {                                                                   \
            if (!(x)) {                                                        \
                STD_UNREACHABLE();                                             \
            }                                                                  \
        } while (0)
#endif

#ifdef __cplusplus
}
#endif

#endif
