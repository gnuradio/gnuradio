/* -*- c++ -*- */
/*
 * Copyright 2008,2009 Free Software Foundation, Inc.
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

#ifndef INCLUDED_VRT_SOURCE_32FC_H
#define INCLUDED_VRT_SOURCE_32FC_H

#include <vrt_source_base.h>
#include <missing_pkt_checker.h>

class vrt_source_32fc : public vrt_source_base
{
protected:
  vrt_source_32fc(const char *name);

  std::vector< std::complex<float> >	d_remainder;
  missing_pkt_checker			d_checker;
  double d_actual_lo_freq;
  double d_actual_cal_freq;

public:
  ~vrt_source_32fc();

  int work(int noutput_items,
	   gr_vector_const_void_star &input_items,
	   gr_vector_void_star &output_items);

  void reset() { d_remainder.clear(); }
};

#endif /* INCLUDED_VRT_SOURCE_32FC_H */
