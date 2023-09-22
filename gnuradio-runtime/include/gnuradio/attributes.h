/*
 * Copyright 2010 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GNURADIO_ATTRIBUTES_H
#define INCLUDED_GNURADIO_ATTRIBUTES_H

////////////////////////////////////////////////////////////////////////
// Cross-platform attribute macros
////////////////////////////////////////////////////////////////////////
#if defined(__clang__) && (!defined(_MSC_VER))
// AppleClang also defines __GNUC__, so do this check first.  These
// will probably be the same as for __GNUC__, but let's keep them
// separate just to be safe.
#define __GR_ATTR_ALIGNED(x) __attribute__((aligned(x)))
#define __GR_ATTR_UNUSED __attribute__((unused))
#define __GR_ATTR_INLINE __attribute__((always_inline))
#define __GR_ATTR_DEPRECATED __attribute__((deprecated))
#define __GR_ATTR_EXPORT __attribute__((visibility("default")))
#define __GR_ATTR_IMPORT __attribute__((visibility("default")))
#elif defined __GNUC__
#define __GR_ATTR_ALIGNED(x) __attribute__((aligned(x)))
#define __GR_ATTR_UNUSED __attribute__((unused))
#define __GR_ATTR_INLINE __attribute__((always_inline))
#define __GR_ATTR_DEPRECATED __attribute__((deprecated))
#define __GR_ATTR_EXPORT __attribute__((visibility("default")))
#define __GR_ATTR_IMPORT __attribute__((visibility("default")))
#elif _MSC_VER
#define __GR_ATTR_ALIGNED(x) __declspec(align(x))
#define __GR_ATTR_UNUSED
#define __GR_ATTR_INLINE __forceinline
#define __GR_ATTR_DEPRECATED __declspec(deprecated)
#define __GR_ATTR_EXPORT __declspec(dllexport)
#define __GR_ATTR_IMPORT __declspec(dllimport)
#else
#define __GR_ATTR_ALIGNED(x)
#define __GR_ATTR_UNUSED
#define __GR_ATTR_INLINE
#define __GR_ATTR_DEPRECATED
#define __GR_ATTR_EXPORT
#define __GR_ATTR_IMPORT
#endif

////////////////////////////////////////////////////////////////////////
// define inline when building C
////////////////////////////////////////////////////////////////////////
#if defined(_MSC_VER) && !defined(__cplusplus) && !defined(inline)
#define inline __inline
#endif

////////////////////////////////////////////////////////////////////////
// suppress warnings
////////////////////////////////////////////////////////////////////////
#ifdef _MSC_VER
#pragma warning(disable : 4251) // class 'A<T>' needs to have dll-interface to be used by
                                // clients of class 'B'
#pragma warning(disable : 4275) // non dll-interface class ... used as base for
                                // dll-interface class ...
#pragma warning( \
    disable : 4244) // conversion from 'double' to 'float', possible loss of data
#pragma warning(disable : 4305) // 'initializing' : truncation from 'double' to 'float'
#pragma warning(disable : 4290) // C++ exception specification ignored except to indicate
                                // a function is not __declspec(nothrow)
#endif
#endif /* INCLUDED_GNURADIO_ATTRIBUTES_H */
