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

#ifndef INCLUDED_CALC_METRIC_H
#define INCLUDED_CALC_METRIC_H

#include <vector>
#include <gr_complex.h>
#include <digital_metric_type.h>


template <class T> 
void calc_metric(int O, int D, const std::vector<T> &TABLE, const T *in, float *metric, trellis_metric_type_t type);

/*
void calc_metric(int O, int D, const std::vector<short> &TABLE, const short *in, float *metric, trellis_metric_type_t type);

void calc_metric(int O, int D, const std::vector<int> &TABLE, const int *in, float *metric, trellis_metric_type_t type);

void calc_metric(int O, int D, const std::vector<float> &TABLE, const float *in, float *metric, trellis_metric_type_t type);
*/

void calc_metric(int O, int D, const std::vector<gr_complex> &TABLE, const gr_complex *in, float *metric, trellis_metric_type_t type);



#endif
