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

#ifndef _GRATSCBITTIMINGLOOP_H_
#define _GRATSCBITTIMINGLOOP_H_

#include <gnuradio/blocks/nco.h>
#include <VrSigProc.h>
#include <VrHistoryProc.h>
#include <VrDecimatingSigProc.h>
#include <gnuradio/atsc/interleaver_fifo.h>
#include <gnuradio/filter/single_pole_iir.h>
#include <gnuradio/filter/mmse_fir_interpolator.h>
#include <gnuradio/atsc/slicer_agc_impl.h>
#include <stdio.h>
#include <gnuradio/atsc/diag_output_impl.h>


/*!
 * \brief ATSC BitTimingLoop
 *
 * This class accepts a single real input and produces a single real output
 */

class GrAtscBitTimingLoop : public VrDecimatingSigProc<float,float> {

 public:

  GrAtscBitTimingLoop ();
  virtual ~GrAtscBitTimingLoop () { };

  virtual const char *name () { return "GrAtscBitTimingLoop"; }

  virtual int forecast (VrSampleRange output,
			VrSampleRange inputs[]);

  virtual int work (VrSampleRange output, void *o[],
		    VrSampleRange inputs[], void *i[]);

  // debug
  void set_mu (double a_mu) { mu = a_mu; }
  void set_no_update (bool a_no_update) { debug_no_update = a_no_update; }
  void set_loop_filter_tap (double tap)  { loop.set_taps (tap); }
  void set_timing_rate (double rate)     { d_timing_rate = rate; }

 protected:

  typedef float iType;
  typedef float oType;

  iType produce_sample (const iType *in, unsigned int &index);
  double filter_error (double e);

  VrSampleIndex				next_input;
  gr_mmse_fir_interpolator		intr;
  double				w;		// timing control word
  double				mu;		// fractional delay
  iType					last_right;	// last right hand sample
  gr_single_pole_iir<double,double,double>	loop;
  bool					debug_no_update;// debug

  double				d_loop_filter_tap;
  double				d_timing_rate;

#ifdef _BT_DIAG_OUTPUT_
  FILE					*fp_loop;
  FILE					*fp_ps;
#endif
};

#endif // _GRATSCBITTIMINGLOOP_H_
