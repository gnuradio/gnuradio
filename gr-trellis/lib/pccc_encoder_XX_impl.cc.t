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

namespace gr {
  namespace trellis {

    @BASE_NAME@::sptr
    @BASE_NAME@::make(const fsm &FSM1, int ST1,
		      const fsm &FSM2, int ST2,
		      const interleaver &INTERLEAVER,
		      int blocklength)
    {
      return gnuradio::get_initial_sptr
	(new @IMPL_NAME@(FSM1, ST1, FSM2, ST2, INTERLEAVER, blocklength));
    }

    @IMPL_NAME@::@IMPL_NAME@(const fsm &FSM1, int ST1,
			     const fsm &FSM2, int ST2,
			     const interleaver &INTERLEAVER,
			     int blocklength)
    : sync_block("@BASE_NAME@",
		    io_signature::make(1, 1, sizeof(@I_TYPE@)),
		    io_signature::make(1, 1, sizeof(@O_TYPE@))),
      d_FSM1(FSM1), d_ST1(ST1),
      d_FSM2(FSM2), d_ST2(ST2),
      d_INTERLEAVER(INTERLEAVER),
      d_blocklength(blocklength)
    {
      assert(d_FSM1.I() == d_FSM2.I());
      set_output_multiple(d_blocklength);
      d_buffer.resize(d_blocklength);
    }

    @IMPL_NAME@::~@IMPL_NAME@()
    {
    }

    int
    @NAME@::work(int noutput_items,
		 gr_vector_const_void_star &input_items,
		 gr_vector_void_star &output_items)
    {
      for(int b = 0 ; b<noutput_items/d_blocklength; b++) {
	const @I_TYPE@ *in = (const @I_TYPE@*)input_items[0]+b*d_blocklength;
	@O_TYPE@ *out = (@O_TYPE@*)output_items[0]+b*d_blocklength;

	int ST1_tmp = d_ST1;
	int ST2_tmp = d_ST2;
	for(int i = 0; i < d_blocklength; i++) {
	  int k = d_INTERLEAVER.INTER()[i];
	  int o1 = d_FSM1.OS()[ST1_tmp*d_FSM1.I()+in[i]];
	  ST1_tmp = (int) d_FSM1.NS()[ST1_tmp*d_FSM1.I()+in[i]];
	  int o2 = d_FSM2.OS()[ST2_tmp*d_FSM2.I()+in[k]];
	  ST2_tmp = (int) d_FSM2.NS()[ST2_tmp*d_FSM2.I()+in[k]];
	  out[i] = (@O_TYPE@) (o1*d_FSM2.O() + o2);
	}
      }
      return noutput_items;
    }

  } /* namespace trellis */
} /* namespace gr */
