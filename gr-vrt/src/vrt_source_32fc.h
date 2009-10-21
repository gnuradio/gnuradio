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

  //caldiv settings parsed from if context
  double d_cal_freq;
  bool   d_cal_locked;
  bool   d_cal_enabled;
  double d_lo_freq;
  bool   d_lo_locked;
  double d_caldiv_temp;
  int    d_caldiv_rev;
  int    d_caldiv_ser;
  int    d_caldiv_mod;
  
  //gps settings parsed from if context
  int    d_utc_time;
  double d_altitude;
  double d_longitude;
  double d_latitude;

public:
  ~vrt_source_32fc();

  int work(int noutput_items,
	   gr_vector_const_void_star &input_items,
	   gr_vector_void_star &output_items);

  void reset() { d_remainder.clear(); }
};

#endif /* INCLUDED_VRT_SOURCE_32FC_H */
