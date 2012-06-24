/* -*- c++ -*- */
/*
 * Copyright 2002 Free Software Foundation, Inc.
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
#ifndef _GRATSCCONVERT2XTO20_H_
#define _GRATSCCONVERT2XTO20_H_

#include <VrDecimatingSigProc.h>
#include <gr_mmse_fir_interpolator.h>

class GrAtscConvert2xTo20 : public VrDecimatingSigProc<float,float> {
  gr_mmse_fir_interpolator	d_interp;
  double			d_frac_part;
  VrSampleIndex			d_next_input;

public:
  GrAtscConvert2xTo20 ();
  ~GrAtscConvert2xTo20 ();

  virtual const char *name () { return "GrAtscConvert2xTo20"; }

  virtual int forecast (VrSampleRange output,
			VrSampleRange inputs[]);

  virtual int work (VrSampleRange output, void *o[],
		    VrSampleRange inputs[], void *i[]);

  void pre_initialize ();
  int checkOutputSamplingFrequency(float) { return 0; } // bogus, but required

};

#endif /* _GRATSCCONVERT2XTO20_H_ */
