/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
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

#include <gr_io_signature.h>
#include "plateau_detector_fb_impl.h"

namespace gr {
  namespace analog {

plateau_detector_fb::sptr
plateau_detector_fb::make(int max_len, float threshold)
{
  return gnuradio::get_initial_sptr (new plateau_detector_fb_impl(max_len, threshold));
}

plateau_detector_fb_impl::plateau_detector_fb_impl(int max_len, float threshold)
  : gr_sync_block("plateau_detector_fb",
    	      gr_make_io_signature(1, 1, sizeof (float)),
    	      gr_make_io_signature(1, 1, sizeof (char))),
	d_max_len(max_len),
	d_threshold(threshold)
{}

plateau_detector_fb_impl::~plateau_detector_fb_impl()
{
}

int
plateau_detector_fb_impl::work(int noutput_items,
    		  gr_vector_const_void_star &input_items,
    		  gr_vector_void_star &output_items)
{
  const float *in = (const float *) input_items[0];
  unsigned char *out = (unsigned char *) output_items[0];
  int flank_start;

  memset((void *) out, 0x00, noutput_items);
  int i;
  for (i = 0; i < noutput_items; i++) {
    if (in[i] >= d_threshold) {
      if (noutput_items-i < 2*d_max_len) { // If we can't finish, come back later
	break;
      }
      flank_start = i;
      while (i < noutput_items && in[i] >= d_threshold)
	i++;
      if ((i - flank_start) > 1) { // 1 Sample is not a plateau
	out[flank_start + (i-flank_start)/2] = 1;
	i = std::min(i+d_max_len, noutput_items-1);
      }
    }
  }

  return i;
}

  } /* namespace analog */
} /* namespace gr */

