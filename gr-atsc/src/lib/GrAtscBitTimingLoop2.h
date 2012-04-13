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

#ifndef _GRATSCBITTIMINGLOOP2_H_
#define _GRATSCBITTIMINGLOOP2_H_

#include <gr_nco.h>
#include <VrSigProc.h>
#include <VrHistoryProc.h>
#include <VrDecimatingSigProc.h>
#include <interleaver_fifo.h>
#include <gr_single_pole_iir.h>
#include <gr_mmse_fir_interpolator.h>

/*!
 * \brief ATSC BitTimingLoop
 *
 * This class accepts a single real input and produces a single real output
 */

class GrAtscBitTimingLoop2 : public VrDecimatingSigProc<float,float> {

 public:

  GrAtscBitTimingLoop2 ();
  virtual ~GrAtscBitTimingLoop2 () { };

  virtual const char *name () { return "GrAtscBitTimingLoop2"; }

  virtual int forecast (VrSampleRange output,
			VrSampleRange inputs[]);

  virtual int work (VrSampleRange output, void *o[],
		    VrSampleRange inputs[], void *i[]);

  // debug
  void set_mu (float a_mu) {
    assert (0 <= a_mu && a_mu <= 1.9);
    use_right_p = a_mu < 1.0;
    mu = a_mu - floor (a_mu);
    cerr << "BTL2:  mu: " << mu << " use_right_p: " << use_right_p << endl;
  }

 protected:

  typedef float iType;
  typedef float oType;

  iType produce_sample (const iType *in, unsigned int &index);
  float filter_error (float e);

  VrSampleIndex				next_input;
  gr_single_pole_iir<float,float,float>	dc;		// used to estimate DC component
  gr_mmse_fir_interpolator		intr;
  float					mu;		// fractional delay
  iType					last_right;	// last right hand sample

  bool					use_right_p;	// ...else middle
};

#endif // _GRATSCBITTIMINGLOOP2_H_
