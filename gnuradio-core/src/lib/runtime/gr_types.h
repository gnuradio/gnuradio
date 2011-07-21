/* -*- c++ -*- */
/*
 * Copyright 2004 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_TYPES_H
#define INCLUDED_GR_TYPES_H

#include <gr_core_api.h>
#include <boost/shared_ptr.hpp>
#include <vector>
#include <stddef.h>				// size_t

#include <gr_complex.h>

typedef std::vector<int>			gr_vector_int;
typedef std::vector<float>			gr_vector_float;
typedef std::vector<double>			gr_vector_double;
typedef std::vector<void *>			gr_vector_void_star;
typedef std::vector<const void *>		gr_vector_const_void_star;

/*
 * #include <config.h> must be placed beforehand 
 * in the source file including gr_types.h for
 * the following to work correctly
 */
#ifdef HAVE_STDINT_H
#include <stdint.h>
typedef int16_t			gr_int16;
typedef int32_t			gr_int32;
typedef int64_t			gr_int64;
typedef uint16_t		gr_uint16;
typedef uint32_t 		gr_uint32;
typedef uint64_t		gr_uint64;
#else
/*
 * Note: these defaults may be wrong on 64-bit systems
 */
typedef short 			gr_int16;
typedef int   			gr_int32;
typedef long long		gr_int64;
typedef unsigned short 		gr_uint16;
typedef unsigned int   		gr_uint32;
typedef unsigned long long	gr_uint64;
#endif	/* HAVE_STDINT_H */

#endif /* INCLUDED_GR_TYPES_H */
