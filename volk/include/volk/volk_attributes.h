/*
 * Copyright 2011 Free Software Foundation, Inc.
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

#ifndef INCLUDED_VOLK_ATTRIBUTES_H
#define INCLUDED_VOLK_ATTRIBUTES_H

////////////////////////////////////////////////////////////////////////
// Cross-platform attribute macros
////////////////////////////////////////////////////////////////////////
#if defined __GNUC__
#  define __VOLK_ATTR_ALIGNED(x) __attribute__((aligned(x)))
#  define __VOLK_ATTR_UNUSED     __attribute__((unused))
#  define __VOLK_ATTR_INLINE     __attribute__((always_inline))
#  define __VOLK_ATTR_DEPRECATED __attribute__((deprecated))
#  if __GNUC__ >= 4
#    define __VOLK_ATTR_EXPORT   __attribute__((visibility("default")))
#    define __VOLK_ATTR_IMPORT   __attribute__((visibility("default")))
#  else
#    define __VOLK_ATTR_EXPORT
#    define __VOLK_ATTR_IMPORT
#  endif
#elif _MSC_VER
#  define __VOLK_ATTR_ALIGNED(x) __declspec(align(x))
#  define __VOLK_ATTR_UNUSED
#  define __VOLK_ATTR_INLINE     __forceinline
#  define __VOLK_ATTR_DEPRECATED __declspec(deprecated)
#  define __VOLK_ATTR_EXPORT     __declspec(dllexport)
#  define __VOLK_ATTR_IMPORT     __declspec(dllimport)
#else
#  define __VOLK_ATTR_ALIGNED(x)
#  define __VOLK_ATTR_UNUSED
#  define __VOLK_ATTR_INLINE
#  define __VOLK_ATTR_DEPRECATED
#  define __VOLK_ATTR_EXPORT
#  define __VOLK_ATTR_IMPORT
#endif

#endif /* INCLUDED_VOLK_ATTRIBUTES_H */
