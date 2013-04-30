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


#ifndef _GRATSCFPLL_H_
#define _GRATSCFPLL_H_

#include <gnuradio/blocks/nco.h>
#include <gnuradio/filter/iir.h>
#include <gnuradio/filter/single_pole_iir.h>
#include <gnuradio/analog/agc.h>
#include <VrSigProc.h>
#include <stdio.h>
#include <gnuradio/atsc/diag_output_impl.h>

/*!
 * \brief ATSC FPLL (2nd Version)
 *
 * Used as follows:
 *                         float	      float
 *  A/D --> GrFIRfilterFFF ----> GrAtscFPLL ---->
 *
 * We use GrFIRfilterFFF to bandpass filter the signal of interest.
 *
 * This class accepts a single real input and produces a single real output
 */

class GrAtscFPLL : public VrSigProc {
 protected:

  typedef float  iType;
  typedef float	 oType;

 public:

  GrAtscFPLL (double a_initial_freq);
  virtual ~GrAtscFPLL () {}

  virtual const char *name () { return "GrAtscFPLL"; }

  virtual void initialize ();

  virtual int work (VrSampleRange output, void *o[],
		    VrSampleRange inputs[], void *i[]);


  // diagnostic routines
  void set_initial_phase (double phase) { initial_phase = phase; }	   // radians
  void set_no_update (bool a_no_update) { debug_no_update = a_no_update; }


 protected:

  double			initial_freq;
  double			initial_phase;
  bool				debug_no_update;
  gr_nco<float,float>		nco;
  gr_agc			agc;	// automatic gain control
  gr_single_pole_iir<float,float,float>	afci;
  gr_single_pole_iir<float,float,float>	afcq;

#ifdef _FPLL_DIAG_OUTPUT_
  FILE				*fp;
#endif

};


#endif // _GRATSCFPLL_H_
