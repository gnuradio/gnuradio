/* -*- c++ -*- */
/*
 * Copyright 2005 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_MISC_H
#define INCLUDED_GR_MISC_H

#include <gnuradio/api.h>
#include <gnuradio/types.h>

GR_RUNTIME_API unsigned int
gr_rounduppow2(unsigned int n);

// FIXME should be template
GR_RUNTIME_API void gr_zero_vector(std::vector<float> &v);
GR_RUNTIME_API void gr_zero_vector(std::vector<double> &v);
GR_RUNTIME_API void gr_zero_vector(std::vector<int> &v);
GR_RUNTIME_API void gr_zero_vector(std::vector<gr_complex> &v);


#endif /* INCLUDED_GR_MISC_H */
