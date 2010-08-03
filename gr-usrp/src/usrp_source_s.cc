/* -*- c++ -*- */
/*
 * Copyright 2004,2006,2009,2010 Free Software Foundation, Inc.
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

#include <usrp_source_s.h>
#include <gr_io_signature.h>
#include <usrp/usrp_standard.h>
#include <usrp/usrp_bytesex.h>

static const int NBASIC_SAMPLES_PER_ITEM = 1;

usrp_source_s_sptr
usrp_make_source_s (int which_board, 
		     unsigned int decim_rate,
		     int nchan,
		     int mux,
		     int mode,
		     int fusb_block_size,
		     int fusb_nblocks,
		     const std::string fpga_filename,
		     const std::string firmware_filename
		     ) throw (std::runtime_error)
{
  return gnuradio::get_initial_sptr(new usrp_source_s (which_board,
						  decim_rate, 
						  nchan,
						  mux,
						  mode,
						  fusb_block_size,
						  fusb_nblocks,
						  fpga_filename,
						  firmware_filename
						  ));
}


usrp_source_s::usrp_source_s (int which_board,
				unsigned int decim_rate,
				int nchan,
				int mux,
				int mode,
				int fusb_block_size,
				int fusb_nblocks,
				const std::string fpga_filename,
				const std::string firmware_filename
				) throw (std::runtime_error)
  : usrp_source_base ("usrp_source_s",
		       gr_make_io_signature (1, 1, sizeof (short)),
		       which_board, decim_rate, nchan, mux, mode,
		       fusb_block_size,
		       fusb_nblocks,
		       fpga_filename, firmware_filename)
{
}

usrp_source_s::~usrp_source_s ()
{
  // NOP
}

int
usrp_source_s::ninput_bytes_reqd_for_noutput_items (int noutput_items)
{
  return noutput_items * NBASIC_SAMPLES_PER_ITEM * sizeof_basic_sample();
}

/*
 * Convert interleaved 8 or 16-bit I & Q from usrp buffer into a single
 * short output stream.
 */
void
usrp_source_s::copy_from_usrp_buffer (gr_vector_void_star &output_items,
				       int output_index,
				       int output_items_available,
				       int &output_items_produced,
				       const void *usrp_buffer,
				       int usrp_buffer_length,
				       int &bytes_read)
{
  short *out = &((short *) output_items[0])[output_index];
  unsigned sbs = sizeof_basic_sample();
  unsigned nusrp_bytes_per_item = NBASIC_SAMPLES_PER_ITEM * sbs;

  int nitems = std::min (output_items_available,
			 (int)(usrp_buffer_length / nusrp_bytes_per_item));

  signed char *s8 = (signed char *) usrp_buffer;
  short *s16 = (short *) usrp_buffer;

  switch(sbs){
  case 1:
    for (int i = 0; i < nitems; i++){
      out[i] = s8[i] << 8;
    }
    break;

  case 2:
    for (int i = 0; i < nitems; i++){
      out[i] = usrp_to_host_short(s16[i]);
    }
    break;
    
  default:
    assert(0);
  }

  output_items_produced = nitems;
  bytes_read = nitems * nusrp_bytes_per_item;
}
