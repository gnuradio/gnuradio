/* -*- c++ -*- */
/*
 * Copyright 2004,2010,2012 Free Software Foundation, Inc.
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

// @WARNING@

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "@NAME@.h"
#include <gnuradio/io_signature.h>
#include <iostream>
#include <gnuradio/trellis/core_algorithms.h>

namespace gr {
  namespace trellis {

    static const float INF = 1.0e9;

    @BASE_NAME@::sptr
    @BASE_NAME@::make(const fsm &FSM1, int ST10, int ST1K,
		      const fsm &FSM2, int ST20, int ST2K,
		      const interleaver &INTERLEAVER,
		      int blocklength,
		      int repetitions,
		      siso_type_t SISO_TYPE)
    {
      return gnuradio::get_initial_sptr
	(new @IMPL_NAME@(FSM1, ST10, ST1K,
			 FSM2, ST20, ST2K,
			 INTERLEAVER,
			 blocklength,
			 repetitions,
			 SISO_TYPE));
    }

    @IMPL_NAME@::@IMPL_NAME@(const fsm &FSM1, int ST10, int ST1K,
			     const fsm &FSM2, int ST20, int ST2K,
			     const interleaver &INTERLEAVER,
			     int blocklength,
			     int repetitions,
			     siso_type_t SISO_TYPE)
    : block("@BASE_NAME@",
	       io_signature::make(1, 1, sizeof(float)),
	       io_signature::make(1, 1, sizeof(@O_TYPE@))),
      d_FSM1(FSM1), d_ST10(ST10), d_ST1K(ST1K),
      d_FSM2(FSM2), d_ST20(ST20), d_ST2K(ST2K),
      d_INTERLEAVER(INTERLEAVER),
      d_blocklength(blocklength),
      d_repetitions(repetitions),
      d_SISO_TYPE(SISO_TYPE)
    {
      set_relative_rate (1.0 / ((double) d_FSM1.O() * d_FSM2.O()));
      set_output_multiple (d_blocklength);
    }

    @IMPL_NAME@::~@IMPL_NAME@()
    {
    }

    void
    @IMPL_NAME@::forecast(int noutput_items,
			  gr_vector_int &ninput_items_required)
    {
      int input_required = d_FSM1.O() * d_FSM2.O() * noutput_items;
      ninput_items_required[0] = input_required;
    }

    //===========================================================

    int
    @IMPL_NAME@::general_work(int noutput_items,
			      gr_vector_int &ninput_items,
			      gr_vector_const_void_star &input_items,
			      gr_vector_void_star &output_items)
    {
      int nblocks = noutput_items / d_blocklength;

      float (*p2min)(float, float) = NULL;
      if(d_SISO_TYPE == TRELLIS_MIN_SUM)
	p2min = &min;
      else if(d_SISO_TYPE == TRELLIS_SUM_PRODUCT)
	p2min = &min_star;

      const float *in = (const float *) input_items[0];
      @O_TYPE@ *out = (@O_TYPE@ *) output_items[0];
      for(int n=0;n<nblocks;n++) {
	pccc_decoder(d_FSM1, d_ST10, d_ST1K,
		     d_FSM2, d_ST20, d_ST2K,
		     d_INTERLEAVER, d_blocklength, d_repetitions,
		     p2min,
		     &(in[n*d_blocklength*d_FSM1.O()*d_FSM2.O()]),
		     &(out[n*d_blocklength]));
      }

      consume_each(d_FSM1.O() * d_FSM2.O() * noutput_items);
      return noutput_items;
    }

  } /* namespace trellis */
} /* namespace gr */
