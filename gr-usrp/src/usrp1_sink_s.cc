/* -*- c++ -*- */
/*
 * Copyright 2004,2006 Free Software Foundation, Inc.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <usrp1_sink_s.h>
#include <gr_io_signature.h>
#include <usrp_standard.h>
#include <usrp_bytesex.h>

usrp1_sink_s_sptr
usrp1_make_sink_s (int which_board,
		   unsigned int interp_rate,
		   int nchan,
		   int mux,
		   int fusb_block_size,
		   int fusb_nblocks,
		   const std::string fpga_filename,
		   const std::string firmware_filename
		   ) throw (std::runtime_error)
{
  return usrp1_sink_s_sptr (new usrp1_sink_s (which_board,
					      interp_rate,
					      nchan,
					      mux,
					      fusb_block_size,
					      fusb_nblocks,
					      fpga_filename,
					      firmware_filename
					      ));
}


usrp1_sink_s::usrp1_sink_s (int which_board,
			    unsigned int interp_rate,
			    int nchan,
			    int mux,
			    int fusb_block_size,
			    int fusb_nblocks,
			    const std::string fpga_filename,
			    const std::string firmware_filename
			    ) throw (std::runtime_error)
  : usrp1_sink_base ("usrp1_sink_s",
		     gr_make_io_signature (1, 1, sizeof (short)),
		     which_board, interp_rate, nchan, mux,
		     fusb_block_size, fusb_nblocks,
		     fpga_filename, firmware_filename)
{
  set_output_multiple (512 / sizeof(short));	// don't change
}

usrp1_sink_s::~usrp1_sink_s ()
{
  // NOP
}

/*
 * Take one short input stream and format it into interleaved short I & Q
 * for the usrp.
 */
void
usrp1_sink_s::copy_to_usrp_buffer (gr_vector_const_void_star &input_items,
				   int  input_index,
				   int	input_items_available,
				   int  &input_items_consumed,	// out
				   void *usrp_buffer,
				   int  usrp_buffer_length,
				   int	&bytes_written)		// out
{
  short *in = &((short *) input_items[0])[input_index];
  short *dst = (short *) usrp_buffer;

  static const int usrp_bytes_per_input_item = sizeof (short);

  int nitems = std::min (input_items_available,
			 usrp_buffer_length / usrp_bytes_per_input_item);

  for (int i = 0; i < nitems; i++){	// FIXME unroll
    dst[i] = host_to_usrp_short(in[i]);
  }

  input_items_consumed = nitems;
  bytes_written = nitems * usrp_bytes_per_input_item;
}

