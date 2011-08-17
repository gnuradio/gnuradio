/*
 * Copyright 2010 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_GRUEL_ATTRIBUTES_H
#define INCLUDED_GRUEL_ATTRIBUTES_H

////////////////////////////////////////////////////////////////////////
// Cross-platform attribute macros
////////////////////////////////////////////////////////////////////////
#if defined __GNUC__
#  define __GR_ATTR_ALIGNED(x) __attribute__((aligned(x)))
#  define __GR_ATTR_UNUSED     __attribute__((unused))
#  define __GR_ATTR_INLINE     __attribute__((always_inline))
#  define __GR_ATTR_DEPRECATED __attribute__((deprecated))
#  if __GNUC__ >= 4
#    define __GR_ATTR_EXPORT   __attribute__((visibility("default")))
#    define __GR_ATTR_IMPORT   __attribute__((visibility("default")))
#  else
#    define __GR_ATTR_EXPORT
#    define __GR_ATTR_IMPORT
#  endif
#elif _MSC_VER
#  define __GR_ATTR_ALIGNED(x) __declspec(align(x))
#  define __GR_ATTR_UNUSED
#  define __GR_ATTR_INLINE     __forceinline
#  define __GR_ATTR_DEPRECATED __declspec(deprecated)
#  define __GR_ATTR_EXPORT     __declspec(dllexport)
#  define __GR_ATTR_IMPORT     __declspec(dllimport)
#else
#  define __GR_ATTR_ALIGNED(x)
#  define __GR_ATTR_UNUSED
#  define __GR_ATTR_INLINE
#  define __GR_ATTR_DEPRECATED
#  define __GR_ATTR_EXPORT
#  define __GR_ATTR_IMPORT
#endif

////////////////////////////////////////////////////////////////////////
// define inline when building C
////////////////////////////////////////////////////////////////////////
#if defined(_MSC_VER) && !defined(__cplusplus) && !defined(inline)
#  define inline __inline
#endif

////////////////////////////////////////////////////////////////////////
// suppress warnings
////////////////////////////////////////////////////////////////////////
#ifdef _MSC_VER
#  pragma warning(disable: 4251) // class 'A<T>' needs to have dll-interface to be used by clients of class 'B'
#  pragma warning(disable: 4275) // non dll-interface class ... used as base for dll-interface class ...
#  pragma warning(disable: 4244) // conversion from 'double' to 'float', possible loss of data
#  pragma warning(disable: 4305) // 'initializing' : truncation from 'double' to 'float'
#  pragma warning(disable: 4290) // C++ exception specification ignored except to indicate a function is not __declspec(nothrow)
#endif

#endif /* INCLUDED_GRUEL_ATTRIBUTES_H */
