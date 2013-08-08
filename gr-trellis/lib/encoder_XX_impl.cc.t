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
    @BASE_NAME@::make(const fsm &FSM, int ST)
    {
      return gnuradio::get_initial_sptr
	(new @IMPL_NAME@(FSM,ST));
    }

    @IMPL_NAME@::@IMPL_NAME@(const fsm &FSM, int ST)
    : sync_block("@BASE_NAME@",
		    io_signature::make(1, -1, sizeof(@I_TYPE@)),
		    io_signature::make(1, -1, sizeof(@O_TYPE@))),
      d_FSM(FSM),
      d_ST(ST)
    {
    }

    @IMPL_NAME@::~@IMPL_NAME@()
    {
    }

    int
    @IMPL_NAME@::work(int noutput_items,
		      gr_vector_const_void_star &input_items,
		      gr_vector_void_star &output_items)
    {
      int ST_tmp = 0;
      int nstreams = input_items.size();

      for(int m=0;m<nstreams;m++) {
	const @I_TYPE@ *in = (const @I_TYPE@*)input_items[m];
	@O_TYPE@ *out = (@O_TYPE@ *) output_items[m];
	ST_tmp = d_ST;

	// per stream processing
	for(int i = 0; i < noutput_items; i++) {
	  out[i] = (@O_TYPE@)d_FSM.OS()[ST_tmp*d_FSM.I()+in[i]]; // direction of time?
	  ST_tmp = (int)d_FSM.NS()[ST_tmp*d_FSM.I()+in[i]];
	}
	// end per stream processing
      }
      d_ST = ST_tmp;

      return noutput_items;
    }

  } /* namespace trellis */
} /* namespace gr */
