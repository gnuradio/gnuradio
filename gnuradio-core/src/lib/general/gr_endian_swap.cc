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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gr_endian_swap.h>
#include <gr_io_signature.h>
#include <volk/volk.h>

gr_endian_swap_sptr
gr_make_endian_swap (size_t item_size_bytes)
{
  return gnuradio::get_initial_sptr(new gr_endian_swap (item_size_bytes));
}

gr_endian_swap::gr_endian_swap (size_t item_size_bytes)
  : gr_sync_block ("gr_endian_swap",
		   gr_make_io_signature (1, 1, item_size_bytes),
		   gr_make_io_signature (1, 1, item_size_bytes))
{
  const int alignment_multiple = volk_get_alignment();
  set_alignment(std::max(1,alignment_multiple));
}

int
gr_endian_swap::work (int noutput_items,
			 gr_vector_const_void_star &input_items,
			 gr_vector_void_star &output_items)
{
  const char *in = (const char *) input_items[0];
  char *out = (char *) output_items[0];

  int nbytes( output_signature()->sizeof_stream_item(0) );
  if(is_unaligned()) {
      switch(nbytes){
        case 1:
            memcpy(out,in,noutput_items);
            break;
        case 2:
            memcpy(out,in,2*noutput_items);
            volk_16u_byteswap_u((uint16_t*)out,noutput_items);
            break;
        case 4:
            memcpy(out,in,4*noutput_items);
            volk_32u_byteswap_u((uint32_t*)out,noutput_items);
            break;
        case 8:
            memcpy(out,in,8*noutput_items);
            volk_64u_byteswap_u((uint64_t*)out,noutput_items);
            break;
        default:
            throw std::runtime_error("itemsize is not valid for gr_endian_swap!");
        } 
    } else {
      switch(nbytes){
        case 1:
            memcpy(out,in,noutput_items);
            break;
        case 2:
            memcpy(out,in,2*noutput_items);
            volk_16u_byteswap_a((uint16_t*)out,noutput_items);
            break;
        case 4:
            memcpy(out,in,4*noutput_items);
            volk_32u_byteswap_a((uint32_t*)out,noutput_items);
            break;
        case 8:
            memcpy(out,in,8*noutput_items);
            volk_64u_byteswap_a((uint64_t*)out,noutput_items);
            break;
        default:
            throw std::runtime_error("itemsize is not valid for gr_endian_swap!");
        }
    }

  return noutput_items;
}



