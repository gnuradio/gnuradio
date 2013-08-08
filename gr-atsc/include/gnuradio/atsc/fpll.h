/* -*- c++ -*- */
/*
 * Copyright 2006 Free Software Foundation, Inc.
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

#ifndef INCLUDED_ATSC_FPLL_H
#define INCLUDED_ATSC_FPLL_H

#include <gnuradio/atsc/api.h>
#include <gnuradio/sync_block.h>
#include <gnuradio/nco.h>
#include <gnuradio/filter/single_pole_iir.h>
#include <gnuradio/analog/agc.h>
#include <stdio.h>
#include <gnuradio/atsc/diag_output_impl.h>

using namespace gr;

class atsc_fpll;
typedef boost::shared_ptr<atsc_fpll> atsc_fpll_sptr;

ATSC_API atsc_fpll_sptr atsc_make_fpll();

/*!
 * \brief ATSC FPLL (2nd Version)
 * \ingroup atsc
 *
 *  A/D --> GrFIRfilterFFF ----> GrAtscFPLL ---->
 *
 * We use GrFIRfilterFFF to bandpass filter the signal of interest.
 *
 * This class accepts a single real input and produces a single real output
 */

class ATSC_API atsc_fpll : public gr::sync_block
{
  friend ATSC_API atsc_fpll_sptr atsc_make_fpll();

  atsc_fpll();

public:

  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);

  void reset() { /* nop */ }

  void initialize () ;

 protected:

  double                        initial_freq;
  double                        initial_phase;
  bool                          debug_no_update;
  gr::nco<float,float>          nco;
  analog::kernel::agc_ff        agc;    // automatic gain control
  filter::single_pole_iir<float,float,float> afci;
  filter::single_pole_iir<float,float,float> afcq;


};


#endif /* INCLUDED_ATSC_FPLL_H */
