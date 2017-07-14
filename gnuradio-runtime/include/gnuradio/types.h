/* -*- c++ -*- */
/*
 * Copyright 2004,2013 Free Software Foundation, Inc.
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

#include <gnuradio/api.h>
#include <boost/shared_ptr.hpp>
#include <vector>
#include <stddef.h>				// size_t

#include <gnuradio/gr_complex.h>

#include <stdint.h>

typedef std::vector<int>			gr_vector_int;
typedef std::vector<unsigned int>		gr_vector_uint;
typedef std::vector<float>			gr_vector_float;
typedef std::vector<double>			gr_vector_double;
typedef std::vector<void *>			gr_vector_void_star;
typedef std::vector<const void *>		gr_vector_const_void_star;


#endif /* INCLUDED_GR_TYPES_H */
