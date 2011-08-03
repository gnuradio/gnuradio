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
#ifndef INCLUDED_ATSC_BIT_TIMING_LOOP_H
#define INCLUDED_ATSC_BIT_TIMING_LOOP_H

#include <atsc_api.h>
#include <cstdio>
#include <gr_block.h>
#include <atsci_diag_output.h>
#include <atsci_sssr.h>
#include <atsci_syminfo.h>

class atsc_bit_timing_loop;
typedef boost::shared_ptr<atsc_bit_timing_loop> atsc_bit_timing_loop_sptr;

ATSC_API atsc_bit_timing_loop_sptr atsc_make_bit_timing_loop();

/*!
 * \brief ATSC BitTimingLoop3
 * \ingroup atsc
 *
 * This class accepts a single real input and produces two outputs,
 *  the raw symbol (float) and the tag (atsc_syminfo)
 */
class ATSC_API atsc_bit_timing_loop : public gr_block
{
  friend ATSC_API atsc_bit_timing_loop_sptr atsc_make_bit_timing_loop();

  atsc_bit_timing_loop();

public:
  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);

  void reset() { /* nop */ }

  ~atsc_bit_timing_loop () { };

  void forecast (int noutput_items, gr_vector_int &ninput_items_required);

  int  general_work (int noutput_items,
                     gr_vector_int &ninput_items,
                     gr_vector_const_void_star &input_items,
                     gr_vector_void_star &output_items);


  // debug (NOPs)
  void set_mu (double a_mu) {  }
  void set_no_update (bool a_no_update) {  }
  void set_loop_filter_tap (double tap)  { }
  void set_timing_rate (double rate)     { }


 protected:

  atsci_sssr                    d_sssr;
  atsci_interpolator            d_interp;
  unsigned long long            d_next_input;
  double                        d_rx_clock_to_symbol_freq;
  int				d_si;


};

#endif /* INCLUDED_ATSC_BIT_TIMING_LOOP_H */



