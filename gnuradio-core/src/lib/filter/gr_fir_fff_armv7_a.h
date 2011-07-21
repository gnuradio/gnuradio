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
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#ifndef INCLUDED_GR_FIR_FFF_ARMV7_A_H
#define INCLUDED_GR_FIR_FFF_ARMV7_A_H

#include <gr_core_api.h>
#include <gr_fir_fff_generic.h>

/*!
 * \brief armv7_a using NEON coprocessor version of gr_fir_fff
 */
class GR_CORE_API gr_fir_fff_armv7_a : public gr_fir_fff_generic
{
protected:

  size_t    d_naligned_taps;  // number of taps (multiple of 4)
  float	   *d_aligned_taps;   // 16-byte aligned, and zero padded to multiple of 4

public:
  gr_fir_fff_armv7_a();
  gr_fir_fff_armv7_a(const std::vector<float> &taps);
  ~gr_fir_fff_armv7_a();

  virtual void set_taps (const std::vector<float> &taps);
  virtual float filter (const float input[]);
};

#endif /* INCLUDED_GR_FIR_FFF_ARMV7_A*_H */
