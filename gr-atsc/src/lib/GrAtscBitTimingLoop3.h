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

#ifndef _GRATSCBITTIMINGLOOP3_H_
#define _GRATSCBITTIMINGLOOP3_H_

#include <cstdio>
#include <VrDecimatingSigProc.h>
#include <atsci_diag_output.h>
#include <atsci_sssr.h>
#include <atsci_syminfo.h>

/*!
 * \brief ATSC BitTimingLoop3
 *
 * This class accepts a single real input and produces two outputs,
 *  the raw symbol (float) and the tag (atsc_syminfo)
 */

class GrAtscBitTimingLoop3 : public VrDecimatingSigProc<float,float> {

 public:

  GrAtscBitTimingLoop3 (double ratio_of_rx_clock_to_symbol_freq);
  virtual ~GrAtscBitTimingLoop3 () { };

  virtual const char *name () { return "GrAtscBitTimingLoop3"; }

  virtual int forecast (VrSampleRange output,
			VrSampleRange inputs[]);

  virtual int work (VrSampleRange output, void *o[],
		    VrSampleRange inputs[], void *i[]);

  // debug (NOPs)
  void set_mu (double a_mu) {  }
  void set_no_update (bool a_no_update) {  }
  void set_loop_filter_tap (double tap)  { }
  void set_timing_rate (double rate)     { }

 protected:

  typedef float 	iType;
  typedef float 	oDataType;
  typedef atsc::syminfo	oTagType;

  atsci_sssr			d_sssr;
  atsci_interpolator		d_interp;
  VrSampleIndex			d_next_input;
  double 			d_rx_clock_to_symbol_freq;
};

#endif // _GRATSCBITTIMINGLOOP3_H_
