/* -*- c++ -*- */
/*
 * Copyright 2006,2010 Free Software Foundation, Inc.
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

#include <gr_ofdm_bpsk_demapper.h>
#include <gr_io_signature.h>

gr_ofdm_bpsk_demapper_sptr
gr_make_ofdm_bpsk_demapper (unsigned int occupied_carriers)
{
  return gnuradio::get_initial_sptr(new gr_ofdm_bpsk_demapper (occupied_carriers));
}

gr_ofdm_bpsk_demapper::gr_ofdm_bpsk_demapper (unsigned occupied_carriers)
  : gr_block ("ofdm_bpsk_demapper",
	      gr_make_io_signature (1, 1, sizeof(gr_complex)*occupied_carriers),
	      gr_make_io_signature (1, 1, sizeof(unsigned char))),
    d_occupied_carriers(occupied_carriers),
    d_byte_offset(0), d_partial_byte(0)
{
}

gr_ofdm_bpsk_demapper::~gr_ofdm_bpsk_demapper(void)
{
}

unsigned char gr_ofdm_bpsk_demapper::slicer(gr_complex x)
{
  return (unsigned char)(x.real() > 0 ? 1 : 0);
}

void
gr_ofdm_bpsk_demapper::forecast (int noutput_items, gr_vector_int &ninput_items_required)
{
  unsigned ninputs = ninput_items_required.size ();
  for (unsigned i = 0; i < ninputs; i++)
    ninput_items_required[i] = 1;
}

int
gr_ofdm_bpsk_demapper::general_work(int noutput_items,
				    gr_vector_int &ninput_items,
				    gr_vector_const_void_star &input_items,
				    gr_vector_void_star &output_items)
{
  const gr_complex *in = (const gr_complex *)input_items[0];
  unsigned char *out = (unsigned char *) output_items[0];

  unsigned int i=0, bytes_produced=0;

  while(i < d_occupied_carriers) {

    while((d_byte_offset < 8) && (i < d_occupied_carriers)) {
      //fprintf(stderr, "%f+j%f\n", in[i].real(), in[i].imag());
      d_partial_byte |= slicer(in[i++]) << (d_byte_offset++);
    }

    if(d_byte_offset == 8) {
      out[bytes_produced++] = d_partial_byte;
      d_byte_offset = 0;
      d_partial_byte = 0;
    }
  }

#if 0
printf("demod out: ");
  for(i = 0; i < bytes_produced; i++) {
    printf("%4x", out[i]);
  }
  printf(" \tlen: %d\n", i);
#endif

  consume_each(1);
  return bytes_produced;
}
