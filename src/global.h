/**
 * globally applicable definitions and declarations
 */

#ifndef GLOBAL_H
#define GLOBAL_H

/** OS detection */
#if defined(__FreeBSD__) || defined(__NetBSD__) || \
    defined(__OpenBSD__) || defined(__bsdi__)   || \
    defined(__DragonFly__)
# define HOLOREN_OS_BSD
#elif defined(_WIN32) || defined(__WIN32__) || defined(__TOS_WIN__) || defined(__WINDOWS__)
# define HOLOREN_OS_WIN
#elif defined(__linux__) || defined(linux) || defined(__linux)
# define HOLOREN_OS_LINUX
#else
# error Unsupported operating system
#endif

/** Compiler detection */
#if defined(_MSC_VER)
# define HOLOREN_CC_MSVC
#elif defined(__GNUC__)
# define HOLOREN_CC_GCC
#else
# error Unsupported compiler
#endif

/**
 * Structure packing macro
 * @see http://stackoverflow.com/questions/1537964/visual-c-equivalent-of-gccs-attribute-packed
 */
#if defined(HOLOREN_CC_MSVC)
# define HOLOREN_PACKED_STRUCT(decl) __pragma(pack(push, 1)) decl __pragma(pack(pop))
#elif defined(HOLOREN_CC_GCC)
# define HOLOREN_PACKED_STRUCT(decl) decl __attribute__((packed))
#else
# define HOLOREN_PACKED_STRUCT(decl) decl
#endif

/** Define portable line, function, and file macros */
# define HOLOREN_FILE __FILE__
# define HOLOREN_LINE __LINE__
#if defined(HOLOREN_CC_MSVC)
# define HOLOREN_FUNC __FUNCSIG__
#elif defined(HOLOREN_CC_GCC)
# define HOLOREN_FUNC __PRETTY_FUNCTION__
#else
# define HOLOREN_FUNC __FUNCTION__
#endif

/** Silence MSVC CRT warnings */
#if defined(HOLOREN_CC_MSVC)
# define _CRT_SECURE_NO_WARNINGS
#endif

/** A macro to designate unused parameters */
#define HOLOREN_UNUSED(x) ((void) (x))

/** A macro for assert statements */
#include <cassert>
#define HOLOREN_ASSERT(x) assert(x)

/** Portable types */
#include <stdint.h>

/* A debugging macro */
#ifdef HOLOREN_DEBUG
#  include <iostream>
#  define DBG(x) std::cerr << x << std::endl
#else
#  define DBG(x)
#endif

/* A macro to print diagnostic messages */
#ifndef HOLOREN_NO_WARNINGS
#  include <iostream>
#  define WARN(x) std::cerr << x << std::endl
#else
#  define WARN(x)
#endif

#endif
